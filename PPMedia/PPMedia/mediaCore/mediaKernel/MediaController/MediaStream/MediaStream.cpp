//
//  MediaStream.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/29.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaStream.h"
#include "MediaSync.h"
#include "DecodeThread.h"
#include "AudioResSample.h"
#include "VideoConver.h"

NS_MEDIA_BEGIN

MediaStream::MediaStream():
curStreamIndex(-1),
decodeThread(NULL),
stream(NULL),
codec(NULL),
mediaContext(NULL),
codecContext(NULL),
frameQueue(NULL),
streamType(PP_STREAM_NONE),
decodeThreadController(NULL),
decodeFrame(NULL)
{

}
  
MediaStream::~MediaStream()
{
    
}
  
int MediaStream::initDecoder(MediaContext *MediaCtx, int streamIndex, StreamType type)
{
    // 参考https://www.jianshu.com/p/8de0fc796ef9
    int ret = 0;
    const char *        forcedCodecName = NULL;
    AVFormatContext*    formatCtx = NULL;
    AVDictionary*       opts = NULL;
    mediaContext = MediaCtx;
    // 设置当前的流索引信息
    this->curStreamIndex = streamIndex;
    this->streamType = type;
    // 获取当前的流
    stream = mediaContext->formatContext->streams[curStreamIndex];
    if (NULL == stream) {
        printf("MediaStream: stream is NULL fail!\n");
        return -1;
    }
    
    // 创建一个解码器上下文
    codecContext = new MediaDecoderContext();
    if (NULL == codecContext) {
        printf("MediaStream: CodecContext is NULL fail!\n");
        return -1;
    }
    // 申请AVCodecContext空间,可以选择性传递一个解码器，不传直接NULL
    codecContext->avctx = avcodec_alloc_context3(NULL);
    if (NULL == codecContext->avctx) {
        printf("MediaStream: codecContext->avctx is NULL fail!\n");
        return -1;
    }
    // 将流的信息直接复制到解码器上
    ret = avcodec_parameters_to_context(codecContext->avctx, stream->codecpar);
    if(ret < 0) {
        printf("MediaStream: avcodec_parameters_to_context is fail!\n");
        return -1;
    }
    // 设置流时钟基准
    av_codec_set_pkt_timebase(codecContext->avctx, stream->time_base);
    
    // 查找解码器
    codec = avcodec_find_decoder(codecContext->avctx->codec_id);
    // 指定解码器
    switch(codecContext->avctx->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            forcedCodecName = mediaContext->audioCodecName;
        break;
        case AVMEDIA_TYPE_VIDEO:
            forcedCodecName = mediaContext->videoCodecName;
        break;
        default:
        break;
    }
    // 查找指定的解码器，如果forcedCodecName不为空，则查找对应的codec
    if (NULL != forcedCodecName) {
        codec = avcodec_find_decoder_by_name(forcedCodecName);
    }
    // 判断是否成功得到解码器
    if (NULL == codec) {
        // 如果指定了这个codec，但是由于这个codec是有问题的，导致codec为空，那么就需要重新再一次find decoder根据codec_id
        if (NULL != forcedCodecName) {
            codec = avcodec_find_decoder(codecContext->avctx->codec_id);
            if (NULL == codec) {
                printf("MediaStream: codec is NULL\n");
                avcodec_free_context(&codecContext->avctx);
                return AVERROR(EINVAL);
            }
        } else {
            printf("MediaStream: codec is NULL\n");
            avcodec_free_context(&codecContext->avctx);
            return AVERROR(EINVAL);
        }
    }
    codecContext->avctx->codec_id = codec->id;
    // 设置一些播放参数
    int stream_lowres = mediaContext->lowres;
    if (stream_lowres > av_codec_get_max_lowres(codec)) {
        printf("The maximum value for lowres supported by the decoder is %d\n",
               av_codec_get_max_lowres(codec));
        stream_lowres = av_codec_get_max_lowres(codec);
    }
    av_codec_set_lowres(codecContext->avctx, stream_lowres);
#if FF_API_EMU_EDGE
    if (stream_lowres) {
        CodecContext->flags |= CODEC_FLAG_EMU_EDGE;
    }
#endif
    if (mediaContext->fast) {
        codecContext->avctx->flags2 |= AV_CODEC_FLAG2_FAST;
    }
#if FF_API_EMU_EDGE
    if (codec->capabilities & AV_CODEC_CAP_DR1) {
        mediaContext->flags |= CODEC_FLAG_EMU_EDGE;
    }
#endif
//    opts = filterCodecOptions(playerState->codec_opts, avctx->codec_id, pFormatCtx, pFormatCtx->streams[streamIndex], codec);
    if (!av_dict_get(opts, "threads", NULL, 0)) {
        av_dict_set(&opts, "threads", "auto", 0);
    }

    if (stream_lowres) {
        av_dict_set_int(&opts, "lowres", stream_lowres, 0);
    }

    if (codecContext->avctx->codec_type == AVMEDIA_TYPE_VIDEO || codecContext->avctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        av_dict_set(&opts, "refcounted_frames", "1", 0);
    }

    // 打开解码器
    if ((ret = avcodec_open2(codecContext->avctx, codec, &opts)) < 0) {
        printf("MediaStream: avcodec_open2 is fail\n");
        avcodec_free_context(&codecContext->avctx);
        av_dict_free(&opts);
        return -1;
    }
    //
    if (av_dict_get(opts, "", NULL, AV_DICT_IGNORE_SUFFIX)) {
        printf("MediaStream: av_dict_get is fail\n");
        avcodec_free_context(&codecContext->avctx);
        av_dict_free(&opts);
        return AVERROR_OPTION_NOT_FOUND;
    }
    
    // 释放字典参数
    av_dict_free(&opts);
    return ret;
}
  
int MediaStream::openDecoder()
{
    int ret = 0;
    PacketQueue*        packetQueue = NULL;
    
    if (NULL == decodeFrame) {
        decodeFrame = new Frame();
        if (NULL == decodeFrame->frame) {
            decodeFrame->frame = av_frame_alloc();
            if (NULL == decodeFrame->frame) {
                printf("MediaStream: frame is NULL fail\n");
                return -1;
            }
        }
    }
    
    SAFE_DELETE(decodeThread);
    decodeThread = new (std::nothrow)PPThread();
    if(NULL == decodeThread) {
        printf("MediaStream: openDecoder decodeThread is fail\n");
        return -1;
    }
    
    decodeThreadController = new (std::nothrow)ThreadController();
    if(NULL == decodeThreadController) {
        printf("MediaStream: openDecoder decodeThreadController is fail\n");
        return -1;
    }
    if(decodeThreadController->init()) {
        printf("MediaStream: openDecoder decodeThreadController init fail\n");
        return -1;
    }
    mediaContext->decodeThreadController = decodeThreadController;
    
    frameQueue = new (std::nothrow)FrameQueue();
    if(NULL == frameQueue) {
        SAFE_DELETE(decodeThread);
        printf("MediaStream: openDecoder frameQueue is fail\n");
        return -1;
    }
    packetQueue = mediaContext->queueManger->GetPacketQueue(curStreamIndex);
    if(NULL == packetQueue) {
        SAFE_DELETE(decodeThread);
        SAFE_DELETE(frameQueue);
        
        printf("MediaStream: openDecoder packetQueue is fail\n");
        return -1;
    }
    
    switch (streamType) {
        case PP_STREAM_AUDIO:
            // 初始化FrameQueue
            frameQueue->frame_queue_init(packetQueue, VIDEO_PICTURE_QUEUE_SIZE, 1);
            // 创建decoder线程
            decodeThread->createThreadEx(mediaContext->decodec_node->func_execute, this, "decodeThread");
            // 启动解码线程
            decodeThread->start();
            // TODO
            
            break;
        case PP_STREAM_VIDEO:
            // 初始化FrameQueue
            frameQueue->frame_queue_init(packetQueue, VIDEO_PICTURE_QUEUE_SIZE, 1);
            // 创建decoder线程
            decodeThread->createThreadEx(mediaContext->decodec_node->func_execute, this, "decodeThread");
            // 启动解码线程
            decodeThread->start();
            // TODO
            break;
        case PP_STREAM_SUBTITLE:
            
            break;
        default:
            break;
    }
    
    return ret;
}
  
int MediaStream::closeDecoder()
{
    int ret = 1;
    if (NULL == mediaContext) {
        printf("MediaStream: closeDecoder mediaContext is NULL fail\n");
        return -1;
    }
    
    if (decodeFrame) {
        if (decodeFrame->frame) {
            av_frame_free(&decodeFrame->frame);
            decodeFrame->frame = NULL;
        }
        delete decodeFrame;
        decodeFrame = NULL;
    }
    
    // 销毁frameQueue
    if (frameQueue) {
        frameQueue->frame_queue_destory();
        delete frameQueue;
        frameQueue = NULL;
    }
    // 销毁packetQueue
    if(mediaContext->queueManger->GetPacketQueue(curStreamIndex)) {
        mediaContext->queueManger->GetPacketQueue(curStreamIndex)->packet_queue_abort();
        // 销毁当前的这个packetQueue
        mediaContext->queueManger->ReleasePacketQueue(curStreamIndex);
    }

    // 销毁线程
    if(decodeThread) {
        // 释放信号量
        if (decodeThreadController) {
            decodeThreadController->singal();
            delete decodeThreadController;
            decodeThreadController = NULL;
        }
        decodeThread->exit();
        decodeThread->join();
        delete decodeThread;
        decodeThread = NULL;
    }
    return ret;
}
  
int MediaStream::pause()
{
    int ret = 0;
    return ret;
}
  
int MediaStream::resume()
{
    int ret = 0;
    return ret;
}
  
int MediaStream::getDecodeFrame(MediaFrame* mediaFrame)
{
    int ret = 0;
    if (NULL == mediaFrame) {
        return -1;
    }
    double last_duration, duration, delay;
    Frame *vp, *lastvp;
    // 每remaining_time运行一次循环（刷新一次屏幕）
    double remaining_time = 0.0;
    double time;
    
    PacketQueue *packetQueue = mediaContext->GetPacketQueue(curStreamIndex);

    // 释放解码器信号量，让解码器开始解码
    if (decodeThreadController) {
        decodeThreadController->singal();
    }
    
    // 从frameQueue中获取一个当前需要显示的帧
    for(;;) {
        // 判断decoder queue中是否存在数据
        if (frameQueue->frame_queue_nb_remaining() == 0) {
            ret = -1;
            goto out;
        } else {
            // 从frameQueue中获取当前播放器显示的帧
            lastvp = frameQueue->frame_queue_peek_last();
            // 获取下一笔要现实的帧
            vp = frameQueue->frame_queue_peek();
            // 当前的这笔数据流不连续，则跳过获取下一笔
            if (vp->serial != packetQueue->serial) {
               frameQueue->frame_queue_next();
                continue;
            }
            // 如果上一笔和当前的这笔serial不对，表示不连续。这边应该从新获取frame_timer的时间
            if (lastvp->serial != vp->serial)
            {
                mediaContext->frame_timer = av_gettime_relative() / 1000000.0; //
            }
            // 是否需要进行avsync
            if (mediaContext->needSync) {
                // 计算上笔应该持续的时间
                last_duration = MediaSync::vp_duration(lastvp, vp, mediaContext);
                
                // 根据当前的视频和主时钟（audio时钟）计算差值diff,根据不同情况调整delay值
                delay = MediaSync::compute_target_delay(last_duration, mediaContext);

                // 获取当前的系统时间值
                time = av_gettime_relative() / 1000000.0;
                
                // 如果上一帧显示时长未满，重复显示上一帧
                // 判断当前frame_timer + delay值是否大于当前的系统时间，如果大于计算剩余时间，继续显示当前帧
                if (time < mediaContext->frame_timer + delay) {
                    // remaining_time剩余时间是，video超前了，需要delay的时间
                    mediaContext->remaining_time = FFMIN(mediaContext->frame_timer + delay - time, remaining_time);
                    // 获取上一笔显示
                    decodeFrame = frameQueue->frame_queue_peek_last();
                    printf("getFrame: video refresh thread show last frame time %f, frame_timer %f, delay %f\n", time, mediaContext->frame_timer, delay);
                    break;
                }
                
                // 更新frame_timer时间，frame_timer更新为上一帧结束时刻，也是当前帧开始时刻
                mediaContext->frame_timer += delay;
                
                // 如果delay大于0
                if (delay > 0 && time - mediaContext->frame_timer > AV_SYNC_THRESHOLD_MAX)
                {
                    printf("getFrame: video refresh thread AV SYNC THRESHOLD MAX \n");
                    mediaContext->frame_timer = time;
                }
                if (!isnan(vp->pts))
                {
                    printf("getFrame: video refresh thread video Clock = %f\n", vp->pts);
                    MediaSync::update_video_clock(vp->pts, vp->serial, mediaContext);
                }

                // 丢帧模式
                if (frameQueue->frame_queue_nb_remaining() > 1)
                {
                    Frame *nextvp = frameQueue->frame_queue_peek_next();
                    duration = MediaSync::vp_duration(vp, nextvp, mediaContext);
                    // 如果当前时间要比这笔显示结束的时间（也就是下一笔开始时间）还大，则丢这一帧
                    if(time > mediaContext->frame_timer + duration)
                    {
                        printf("getFrame: video refresh thread drop video frame_drops_late %d\n", mediaContext->frame_drops_late);
                        mediaContext->frame_drops_late++;
                        frameQueue->frame_queue_next();
                        continue;
                    }
                }
                // 则正常显示
                frameQueue->frame_queue_next();
                // 获取上一笔显示
                decodeFrame = frameQueue->frame_queue_peek_last();
                break;
            } else {
                if (!isnan(vp->pts))
                {
                    printf("getFrame: video refresh thread video Clock = %f\n", vp->pts);
                    MediaSync::update_video_clock(vp->pts, vp->serial, mediaContext);
                }
                // 则正常显示
                frameQueue->frame_queue_next();
                // 获取上一笔显示
                decodeFrame = frameQueue->frame_queue_peek_last();
                break;
            }
        }
    }
    
    switch (streamType) {
        case PP_STREAM_VIDEO: {
            // 如果是视频流
            videoParamInfo srcVideoParm;
            srcVideoParm.codecId = codecContext->avctx->codec_id;
            srcVideoParm.frameRate = codecContext->avctx->framerate;
            srcVideoParm.height = codecContext->avctx->height;
            srcVideoParm.width = codecContext->avctx->width;
            srcVideoParm.pixelFormat = codecContext->avctx->pix_fmt;
            
            mediaContext->srcVideoParam = srcVideoParm;
            // 设置当前的媒体帧为video帧
            mediaFrame->setMediaType(PP_FRAME_VIDEO);
            // 设置输入输出参数
            mediaFrame->setSrcVideoParam(srcVideoParm);
        }
        break;
        case PP_STREAM_AUDIO:{
            // 如果是音频流
            AudioParamInfo srcAudioParm;
            srcAudioParm.sample_rate = decodeFrame->frame->sample_rate;
            srcAudioParm.channel_layout = decodeFrame->frame->channel_layout;
            srcAudioParm.channels = decodeFrame->frame->channels;
            srcAudioParm.fmt = (AVSampleFormat)decodeFrame->frame->format;
            srcAudioParm.frame_size = decodeFrame->frame->nb_samples;
            // 更新srcAudioParam
            mediaContext->srcAudioParam = srcAudioParm;
            // 设置当前的媒体帧为audio帧
            mediaFrame->setMediaType(PP_FRAME_AUDIO);
            // 设置输入输出参数
            mediaFrame->setSrcAudioParam(srcAudioParm);
        }
        break;
        case PP_STREAM_SUBTITLE: {
        
        }
        break;
        default:
            break;
    }
    // 将frame写入mediaFrame中
    mediaFrame->writeFrame(decodeFrame->frame);
    
out:
    return ret;
}

NS_MEDIA_END

