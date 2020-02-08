//
//  MediaStream.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaStream.h"
#include "DecodeThread.h"
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
decodeThreadController(NULL)
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
    packetQueue = mediaContext->GetPacketQueue(curStreamIndex);
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
            decodeThread->setFunc(DecoderThread, this, "decodeThread");
            // 启动解码线程
            decodeThread->start();
            // TODO
            
            break;
        case PP_STREAM_VIDEO:
            // 初始化FrameQueue
            frameQueue->frame_queue_init(packetQueue, VIDEO_PICTURE_QUEUE_SIZE, 1);
            // 创建decoder线程
            decodeThread->setFunc(DecoderThread, this, "decodeThread");
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
    // 销毁frameQueue
    if (frameQueue) {
        frameQueue->frame_queue_destory();
        delete frameQueue;
        frameQueue = NULL;
    }
    // 销毁packetQueue
    if(mediaContext->GetPacketQueue(curStreamIndex)) {
        mediaContext->GetPacketQueue(curStreamIndex)->packet_queue_abort();
        // 销毁当前的这个packetQueue
        mediaContext->ReleasePacketQueue(curStreamIndex);
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
  
MediaFrame* MediaStream::getFrame()
{
    // 释放解码器信号量，让解码器开始解码
    if (decodeThreadController) {
        decodeThreadController->singal();
    }
    return NULL;
}

NS_MEDIA_END


