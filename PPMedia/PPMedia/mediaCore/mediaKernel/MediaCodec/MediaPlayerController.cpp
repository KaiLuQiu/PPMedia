//
//  MediaPlayerController.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/04.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaPlayerController.h"
#include "DemuxdeThread.h"
NS_MEDIA_BEGIN

MediaPlayerController::MediaPlayerController():
demuxerThread(NULL)
{
    memset(stream_index, -1, sizeof(stream_index));
    for(int i = 0; i < MAX_DCODEC_STREAM_NUM; i++) {
        mediaStream[i] = NULL;
    }
}

MediaPlayerController::~MediaPlayerController()
{
    
}

/*
 * 初始化操作
 */
int MediaPlayerController::init()
{
    return streamOpen();
}

int MediaPlayerController::streamOpen()
{
    int ret;
    int seek_by_bytes = -1;
    SAFE_DELETE(mediaContext);
    // 先创建一个播放器上下文
    mediaContext = new MediaContext();
    // 对AvFormat上下文内存空间的申请
    mediaContext->formatContext = avformat_alloc_context();
    if (NULL == mediaContext->formatContext) {
        printf("MediaPlayerController:avformat alloc fail\n");
        return false;
    }
    
    // 打开一个mediaPath文件并解析。可解析的内容包括：视频流、音频流、视频流参数、音频流参数、视频帧索引。
    ret = avformat_open_input(&mediaContext->formatContext, mediaPath.c_str(), mediaContext->Informat, NULL);
    if (ret < 0) {
        printf(mediaPath.c_str(), ret);
        return false;
    }
    
    // 去parse流的相关信息
    ret = avformat_find_stream_info(mediaContext->formatContext, NULL);
    if (ret < 0) {
        
        printf("avformat_find_stream_info  fail\n");
        return false;
    }
    
    if (mediaContext->formatContext->pb)
        mediaContext->formatContext->pb->eof_reached = 0; // FIXME hack, ffplay maybe should not use avio_feof() to test for the end
    
    mediaContext->nbStreams = mediaContext->formatContext->nb_streams;
    
    if (seek_by_bytes < 0) {
        seek_by_bytes = !!(mediaContext->formatContext->iformat->flags & AVFMT_TS_DISCONT) && strcmp("ogg", mediaContext->formatContext->iformat->name);
    }
    // 获取最大的帧duration
    mediaContext->max_frame_duration = (mediaContext->formatContext->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;
#ifdef Debug
        av_dump_format(mediaContext->formatContext, 0, mediaPath.c_str(), 0);
#endif
    //这边一般是获取第一个流
    stream_index[AVMEDIA_TYPE_VIDEO] =
        av_find_best_stream(mediaContext->formatContext, AVMEDIA_TYPE_VIDEO,
                            stream_index[AVMEDIA_TYPE_VIDEO], -1, NULL, 0);
    
    stream_index[AVMEDIA_TYPE_AUDIO] =
        av_find_best_stream(mediaContext->formatContext, AVMEDIA_TYPE_AUDIO,
                            stream_index[AVMEDIA_TYPE_AUDIO],
                            stream_index[AVMEDIA_TYPE_VIDEO],
                            NULL, 0);
    return ret;
}

/*
 * 进入准备状态
 */
int MediaPlayerController::prepareAsync()
{
    int ret;
    AVFormatContext*        avformatContext;
    AVStream*               avStream;
    avformatContext = mediaContext->formatContext;
    // 流的总数量
    int nb_streams = mediaContext->nbStreams;
    for (int i = 0; i < nb_streams; i++) {
        AVCodecContext *CodecContext = avformatContext->streams[i]->codec;
        avStream = avformatContext->streams[i];
        // 如果当前的流是video or audio的话则创建对应的packetQueue
        if (CodecContext->codec_type == AVMEDIA_TYPE_VIDEO ||
            CodecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (false == mediaContext->CreatePacketQueue(i)) {
                printf("MediaPlayerController: CreatePacketQueue fail\n");
                return -1;
            }
        }
        // 视频流
        if ((CodecContext->codec_type == AVMEDIA_TYPE_VIDEO) && (stream_index[AVMEDIA_TYPE_VIDEO] >= 0)) {
            // 创建对应的mediaStream类
            mediaStream[i] = new (std::nothrow)MediaStream();
            if(NULL == mediaStream[i]) {
                printf("MediaPlayerController: new mediaStream fail\n");
                continue;
            }
            // 设置对应media的索引
            mediaStream[i]->setStreamInfo(i);
            // 视频流数++
            mediaContext->video_streams++;
        }
        // 音频流
        else if ((CodecContext->codec_type == AVMEDIA_TYPE_AUDIO) && (stream_index[AVMEDIA_TYPE_AUDIO] >= 0)) {
            // 创建对应的mediaStream类
            mediaStream[i] = new (std::nothrow)MediaStream();
            if(NULL == mediaStream[i]) {
                printf("MediaPlayerController: new mediaStream fail\n");
                continue;
            }
            // 设置对应media的索引
            mediaStream[i]->setStreamInfo(i);
            // 音频流数++
            mediaContext->audio_streams++;
        }
        // 初始化当前流的解码器
        ret = mediaStream[i]->initDecoder(mediaContext);
        if (ret < 0) {
            printf("MediaPlayerController: initDecoder fail\n");
            SAFE_DELETE(mediaStream[i]);
            continue;
        }
        // 内部会创建decode线程
        ret = mediaStream[i]->openDecoder();
        if (ret < 0) {
            printf("MediaPlayerController: openDecoder fail\n");
            SAFE_DELETE(mediaStream[i]);
            continue;
        }
    }
    
    // 启动demuxer
    SAFE_DELETE(demuxerThread);
    // 创建线程封装对象
    demuxerThread = new (std::nothrow)PPThread();
    if(NULL == demuxerThread) {
        printf("MediaPlayerController: new demuxerThread fail\n");
        memset(stream_index, -1, sizeof(stream_index));
        for(int i = 0; i < MAX_DCODEC_STREAM_NUM; i++) {
            SAFE_DELETE(mediaStream[i]);
        }
        return -1;
    }
    // 设置demuxer线程的参数信息
    demuxerThread->setFunc(DemuxerThread, mediaContext, "demuxerThread");
    // 启动demuxer
    demuxerThread->start();
    return 1;
}

/*
 * 开始
 */
int MediaPlayerController::start()
{
    return 0;
}

/*
 * 停止播放
 */
int MediaPlayerController::stop()
{
    return 0;
}

/*
 * seek
 */
int MediaPlayerController::seek(float pos)
{
    return 0;
}

/*
 * 暂停
 */
int MediaPlayerController::pause()
{
    return 0;
}

/*
 * 获取媒体Frame
 */
MediaFrame *getFrame()
{
    return NULL;
}

/*
 * 依据时间获取获取媒体Frame
 */
MediaFrame *getFrame(int64_t syncTime)
{
    return NULL;
}

NS_MEDIA_END


