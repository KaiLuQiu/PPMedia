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
demuxerThread(NULL),
demuxerThreadController(NULL)
{
    memset(stream_index, -1, sizeof(stream_index));
    for(int i = 0; i < MAX_DCODEC_STREAM_NUM; i++) {
        mediaStream[i] = NULL;
    }
}

MediaPlayerController::~MediaPlayerController()
{
    
}

static int decode_interrupt_cb(void *ctx)
{
    MediaContext *mediaContext = (MediaContext *)ctx;
    return mediaContext->abort_request;
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
    if (NULL == this->pipleLine) {
        printf("MediaPlayerController:pipleLine is NULL fail\n");
        return -1;
    }
    SAFE_DELETE(mediaContext);
    // 先创建一个播放器上下文
    mediaContext = new MediaContext();
    // 对AvFormat上下文内存空间的申请
    mediaContext->formatContext = avformat_alloc_context();
    if (NULL == mediaContext->formatContext) {
        printf("MediaPlayerController:avformat alloc fail\n");
        return false;
    }
    
    // 设置中断函数，如果出错或者退出，就可以立刻退出
    mediaContext->formatContext->interrupt_callback.callback = decode_interrupt_cb;
    mediaContext->formatContext->interrupt_callback.opaque = mediaContext;
    
    // 打开一个mediaPath文件并解析。可解析的内容包括：视频流、音频流、视频流参数、音频流参数、视频帧索引。
    ret = avformat_open_input(&mediaContext->formatContext, mediaPath.c_str(), mediaContext->informat, NULL);
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
    StreamType              streamType;
    avformatContext = mediaContext->formatContext;
    // 流的总数量
    int nb_streams = mediaContext->nbStreams;
    // flag标志位设置初始值
    mediaContext->stopCodecThread = false;
    
    demuxerThreadController = new ThreadController();
    if(demuxerThreadController->init()) {
        printf("MediaPlayerController: decodeThreadController init fail\n");
        return -1;
    }
    mediaContext->demuxerThreadController = demuxerThreadController;
    
    this->decodec_node = MediaPipeline::get_video_decoder_node(this->pipleLine);
    if (NULL == this->decodec_node) {
        printf("MediaPlayerController: decodec_node is NULL fail\n");
    }
    mediaContext->decodec_node = this->decodec_node;
    
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
            // 设置对应stream类型
            streamType = PP_STREAM_VIDEO;
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
            // 设置对应stream类型
            streamType = PP_STREAM_AUDIO;
            // 音频流数++
            mediaContext->audio_streams++;
        }
        
        // 初始化当前流的解码器
        ret = mediaStream[i]->initDecoder(mediaContext, i, streamType);
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
        
        // 解码后更新音视频源参数信息
        if ((CodecContext->codec_type == AVMEDIA_TYPE_VIDEO) && (stream_index[AVMEDIA_TYPE_VIDEO] >= 0)) {
            // 更新源的视频格式信息
            mediaContext->srcVideoParam.srcWidth = mediaStream[i]->codecContext->avctx->width;
            mediaContext->srcVideoParam.srcHeight = mediaStream[i]->codecContext->avctx->height;
            mediaContext->srcVideoParam.codecId = mediaStream[i]->codecContext->avctx->codec_id;
            mediaContext->srcVideoParam.pixelFormat = mediaStream[i]->codecContext->avctx->pix_fmt;
            // 猜测视频帧率
            mediaContext->srcVideoParam.frameRate = av_guess_frame_rate(mediaContext->formatContext, mediaStream[i]->stream, NULL);
        } else if ((CodecContext->codec_type == AVMEDIA_TYPE_AUDIO) && (stream_index[AVMEDIA_TYPE_AUDIO] >= 0)) {
            
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
    if (this->pipleLine) {
        delete this->pipleLine;
        this->pipleLine = NULL;
    }
    
    if (this->decodec_node) {
        delete this->decodec_node;
        this->decodec_node = NULL;
    }
    
    if (this->aout_node) {
        delete this->aout_node;
        this->aout_node = NULL;
    }
    
    // flag标志位设置
    mediaContext->stopCodecThread = true;
    // 如果创建了demuxer线程 则销毁
    if (demuxerThread) {
        // 销毁线程同步控制器
        if (demuxerThreadController) {
            // 释放信号量
            demuxerThreadController->singal();
            delete demuxerThreadController;
            demuxerThreadController = NULL;
        }
        demuxerThread->exit();
        demuxerThread->join();
        delete demuxerThread;
        demuxerThread = NULL;
    }
    
    int nb_streams = mediaContext->nbStreams;
    // 销毁当前播放这个文件的每条stream
    for (int i = 0; i < nb_streams; i++) {
        if (mediaStream[i]) {
            mediaStream[i]->closeDecoder();
            delete mediaStream[i];
            mediaStream[i] = NULL;
        }
    }
    
    // 销毁mediaContext中的avformatContext
    if (mediaContext->formatContext) {
        avformat_close_input(&mediaContext->formatContext);
    }
    delete mediaContext;
    mediaContext = NULL;
    return 1;
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


