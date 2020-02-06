//
//  DecodeThread.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/15.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "DecodeThread.h"
#include "MediaStream.h"
#include <pthread.h>
#include "FrameQueue.h"
#include "PacketQueue.h"
#include "FFmpegInit.h"
#include "AvSyncClock.h"

NS_MEDIA_BEGIN

// 将video frame 送入对应frameQueue中
static int queue_picture(FrameQueue *queue, AVFrame *src_frame, double pts, double duration, int64_t pos, int serial)
{
    Frame *vp;

    // 从videoDecodeRingBuffer中获取一块Frame大小的可写内存，如果当前的size = max_size的话，说明写满了则返回为空
    if (!(vp = queue->frame_queue_peek_writable()))
        return -1;
    
    vp->sar = src_frame->sample_aspect_ratio;
    vp->uploaded = 0;
    
    vp->width = src_frame->width;
    vp->height = src_frame->height;
    vp->format = src_frame->format;

    vp->pts = pts;
    vp->duration = duration;
    vp->pos = pos;
    vp->serial = serial;
    
    // 解除本frame对本frame中所有缓冲区的引用，并复位frame中各成员。
    av_frame_unref(vp->frame);
    // 复制AVFrame元数据, 元数据指视频的一些非数据字段
    av_frame_copy_props(vp->frame, src_frame);
    // 将src中所有数据拷贝到dst中，并复位src。
    // 看到这个函数声明，第一感觉是它也会先 reset dst，其实错了!如果你没有先调用 av_frame_unref(dst)，这个函数会导致大量的内存泄漏
    // 为避免内存泄漏，在调用av_frame_move_ref(dst, src)之前应先调用av_frame_unref(dst) 。
    av_frame_move_ref(vp->frame, src_frame);
    // 主要时将写索引往前移动
    queue->frame_queue_push();
    return 0;
}

// 将audio frame 送入对应frameQueue中
static int queue_audio(FrameQueue *queue, AVFrame *src_frame, double pts, double duration, int64_t pos, int serial)
{
    Frame *ap;
    // 从audioDecodeRingBuffer中获取一块Frame大小的可写内存，如果当前的size = max_size的话，说明写满了则返回为空
    if (!(ap = queue->frame_queue_peek_writable()))
        return -1;
    
    ap->sar = src_frame->sample_aspect_ratio;
    ap->uploaded = 0;

    ap->format = src_frame->format;
    
    ap->pts = pts;
    ap->duration = duration;
    ap->pos = pos;
    ap->serial = serial;
    
    // 解除本frame对本frame中所有缓冲区的引用，并复位frame中各成员。
    av_frame_unref(ap->frame);
    // 复制AVFrame元数据, 元数据指视频的一些非数据字段
    av_frame_copy_props(ap->frame, src_frame);
    // 将src中所有数据拷贝到dst中，并复位src。
    // 看到这个函数声明，第一感觉是它也会先 reset dst，其实错了!如果你没有先调用 av_frame_unref(dst)，这个函数会导致大量的内存泄漏
    // 为避免内存泄漏，在调用av_frame_move_ref(dst, src)之前应先调用av_frame_unref(dst) 。
    av_frame_move_ref(ap->frame, src_frame);
    // 主要时将写索引往前移动
    queue->frame_queue_push();
    return 0;
}

static int queue_subtitle(FrameQueue *queue, AVFrame *src_frame, double pts, double duration, int64_t pos, int serial)
{
    // TODO
    return 0;
}

// 参考ffplay
static int decoder_decode_frame(MediaStream *mediaStream, AVFrame *frame)
{
    int ret = AVERROR(EAGAIN);
    MediaContext* mediaContext = mediaStream->mediaContext;
    int streamIndex = mediaStream->curStreamIndex;
    // 获取解码上下文
    MediaDecoderContext* codecContext = mediaStream->codecContext;
    AVCodecContext* avctx = codecContext->avctx;
    // 获取当前的packetQueue
    PacketQueue* packetQueue = mediaContext->GetPacketQueue(streamIndex);
    for (;;) {
        AVPacket pkt;
        // 只有当serial才去解码获取帧
        // 先去接受帧(codecContext->pkt_serial初始值为-1)
        if(packetQueue->serial == codecContext->pkt_serial) {
            do {
                if (packetQueue->abort_request) {
                    // 如果当前的被abort则直接返回
                    return -1;
                }
                switch (codecContext->avctx->codec_type) {
                    case AVMEDIA_TYPE_VIDEO:
                        ret = avcodec_receive_frame(avctx, frame);
                        if (ret >= 0) {
                            // 在流时基中使用各种启发式方法估计pts
                            frame->pts = frame->best_effort_timestamp;
                        }
                        break;
                    case AVMEDIA_TYPE_AUDIO:
                        ret = avcodec_receive_frame(avctx, frame);
                        if (ret >= 0) {
                            AVRational tb = (AVRational){1, frame->sample_rate};
                            if (frame->pts != AV_NOPTS_VALUE)
                                frame->pts = av_rescale_q(frame->pts, av_codec_get_pkt_timebase(avctx), tb);
                            else if (codecContext->next_pts != AV_NOPTS_VALUE)
                                frame->pts = av_rescale_q(codecContext->next_pts, codecContext->next_pts_tb, tb);
                            if (frame->pts != AV_NOPTS_VALUE) {
                                codecContext->next_pts = frame->pts + frame->nb_samples;
                                codecContext->next_pts_tb = tb;
                            }
                        }
                        break;
                    case AVMEDIA_TYPE_SUBTITLE:
                        
                        break;
                    default:
                        break;
                }
            }while (ret != AVERROR(EAGAIN));
            // AVERROR(EAGAIN) 指的是：再试一次/* no packets available, try the next filter up the chain */
        }
        
        // 从packetQueue中读取包
        do {
            if (packetQueue->nb_packets == 0)
                pthread_cond_signal(codecContext->empty_queue_cond);
            if (codecContext->packet_pending) {
                av_packet_move_ref(&pkt, &codecContext->pkt);
                codecContext->packet_pending = 0;
            } else {
                if (packetQueue->packet_queue_get(&pkt, 1, &codecContext->pkt_serial) < 0)
                    return -1;
            }
        } while (packetQueue->serial != codecContext->pkt_serial);
        
        // 读取到一包数据后，送入解码
        if (pkt.data == FFmpegInit::flushPkt->data) {
            // 如果当前的是flushPkt的话，则flush avcodec
            avcodec_flush_buffers(avctx);
            // 让packetQueue也flush掉，
            packetQueue->packet_queue_flush();
            codecContext->finished = 0;
            codecContext->next_pts = codecContext->start_pts;
            codecContext->next_pts_tb = codecContext->start_pts_tb;
        } else {
            if (avctx->codec_type == AVMEDIA_TYPE_SUBTITLE) {
                // 暂不支持subtitle
            } else {
                // 发送包下去解码
                int ret = avcodec_send_packet(avctx, &pkt);
                if (ret == AVERROR(EAGAIN)) {
                    printf("Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
                    codecContext->packet_pending = 1;
                    av_packet_move_ref(&codecContext->pkt, &pkt);
                }
                av_packet_unref(&pkt);
                if (ret < 0 && ret != AVERROR(EAGAIN)) {
                    //TODO 挂起线程
                }
            }
        }
    }
}

static int get_video_frame(MediaStream *mediaStream, AVFrame *frame)
{
    int got_picture;
    MediaContext* mediaContext = mediaStream->mediaContext;
    AVStream* videoStream = mediaStream->stream;
    // 获取解码上下文
    MediaDecoderContext* codecContext = mediaStream->codecContext;
    Clock* videoClock = mediaContext->videoClock;
    // 获取当前的packetQueue
    PacketQueue* videoPacketQueue = mediaContext->GetPacketQueue(mediaStream->curStreamIndex);
    if ((got_picture = decoder_decode_frame(mediaStream, frame)) < 0)
        return -1;

    // 计算是否需要丢帧
    if (got_picture) {
        double dpts = NAN;

        if (frame->pts != AV_NOPTS_VALUE)
            dpts = av_q2d(videoStream->time_base) * frame->pts;
        
        frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(mediaContext->formatContext, videoStream, frame);

        // 默认都是丢帧处理
        if (AvSyncClock::get_master_sync_type(mediaContext) != SYNC_AUDIO_CLOCK) {
            if (frame->pts != AV_NOPTS_VALUE) {
                double diff = dpts - AvSyncClock::get_master_clock(mediaContext);
                if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD &&
                    codecContext->pkt_serial == videoClock->serial &&
                    videoPacketQueue->nb_packets) {
                    mediaContext->frame_drops_early++;
                    av_frame_unref(frame);
                    got_picture = 0;
                }
            }
        }
    }

    return got_picture;
}

static int get_audio_frame(MediaStream *mediaStream, AVFrame *frame)
{
    int ret = 0;
    MediaContext* mediaContext = mediaStream->mediaContext;
    ret = decoder_decode_frame(mediaStream, frame);
    if (ret < 0) {
        return -1;
    }
    frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(mediaContext->formatContext, mediaContext->formatContext->streams[mediaStream->curStreamIndex], frame);
    return ret;
}

static int get_subtitle_frame(MediaStream *mediaStream, AVFrame *frame)
{
    int ret = 0;
    return ret;
}

void *DecoderThread(void *arg)
{
    // 获取当前的媒体流
    MediaStream *mediaStream = (MediaStream *)arg;
    // 获取当前媒体文件播放上下文
    MediaContext* mediaContext = mediaStream->mediaContext;
    // 获取当前解码流的类型
    StreamType streamType = mediaStream->streamType;
    // 获取当前解码流的索引
    int streamIndex = mediaStream->curStreamIndex;
    // 当前解码后用于存放的Queue
    FrameQueue* frameQueue = NULL;
    // 猜测视频帧率
    AVRational frame_rate = av_guess_frame_rate(mediaContext->formatContext, mediaContext->formatContext->streams[streamIndex], NULL);
    AVFrame *frame = av_frame_alloc();
    AVRational tb;
    while(false == mediaContext->stopCodecThread) {
        switch (streamType) {
            case PP_STREAM_VIDEO: {
                // 获取一笔解码后的帧
                int ret = get_video_frame(mediaStream, frame);
                if (ret > 0) {
                    // 将pts时间转成秒
                    // timestamp(ffmpeg内部时间戳) = AV_TIME_BASE * time(秒)
                    // time(秒) = AV_TIME_BASE_Q * timestamp(ffmpeg内部时间戳)
                    double pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
                    double duration = (frame_rate.num && frame_rate.den ? av_q2d((AVRational){frame_rate.den, frame_rate.num}) : 0);
                    int64_t pos = frame->pkt_pos;
                    queue_picture(frameQueue, frame, pts, duration, pos, mediaStream->codecContext->pkt_serial);
                }
            }
            break;
            case PP_STREAM_AUDIO: {
                int ret = get_audio_frame(mediaStream, frame);
                if (ret > 0) {
                    tb = (AVRational){1, frame->sample_rate};
                    double pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
                    int64_t pos = frame->pkt_pos;
                    int serial =  mediaStream->codecContext->pkt_serial;
                    double duration = av_q2d((AVRational){frame->nb_samples, frame->sample_rate});
                    queue_audio(frameQueue, frame, pts, duration, pos, serial);
                }
            }
            break;
            case PP_STREAM_SUBTITLE:
                
            break;
            default:
                break;
        }
        
    }
    if (frame) {
        av_frame_free(&frame);
    }
    return NULL;
}

NS_MEDIA_END

