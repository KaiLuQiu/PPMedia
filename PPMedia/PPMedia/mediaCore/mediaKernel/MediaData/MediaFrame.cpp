//
//  MediaFrame.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaFrame.h"
NS_MEDIA_BEGIN

MediaFrame::MediaFrame():
frameType(PP_FRAME_NONE),
isInitVideoParam(false),
isInitAudioParam(false),
srcData(NULL),
dstData(NULL),
srcSize(0),
dstSize(0)
{
    videoConver = new (std::nothrow)VideoConver();
    videoConver->init();
    // 设置默认的参数信息
    videoConver->setVideoInfo(srcVideoParam, dstVideoParam);
    audioConver = new AudioResSample();
    audioConver->init(srcAudioParam, dstAudioParam);
}
MediaFrame::~MediaFrame()
{
    if (srcData) {
        av_freep(&srcData);
    }
    
    if (dstData) {
        av_freep(&dstData);
    }
    
    isInitAudioParam = false;
    isInitVideoParam = false;
}

void MediaFrame::setMediaType(FrameType type)
{
    frameType = type;
}

void MediaFrame::setSrcAudioParam(AudioParamInfo srcParam, AudioParamInfo dstParam)
{
    dstAudioParam = dstParam;
    srcAudioParam = srcParam;
    // 如果当前的输入参数和dstAudioParam(表示音频输出的)不同的话，则初始化重采样器
    if (dstAudioParam.channels != srcParam.channels ||
        dstAudioParam.sample_rate != srcParam.sample_rate ||
        dstAudioParam.frame_size != srcParam.frame_size ||
        dstAudioParam.fmt != srcParam.fmt) {
        // 表明输入和输出的音频格式不一样，则进行重采样操作
        if (audioConver) {
            audioConver->release();
            delete audioConver;
            audioConver = NULL;
        }
        audioConver = new AudioResSample();
        // 对重采样器设置输入输出的音频参数
        audioConver->init(srcAudioParam, dstAudioParam);
    }
    isInitAudioParam = true;
}

void MediaFrame::setVideoParam(videoParamInfo srcParam, videoParamInfo dstParam)
{
    isInitVideoParam = true;
    srcVideoParam = srcParam;
    dstVideoParam = dstParam;
    // 设置视频格式转换的参数信息
    videoConver->setVideoInfo(srcVideoParam, dstVideoParam);
}

int MediaFrame::writeFrameData(uint8_t* data, int64_t dataSize)
{
    if (NULL == data || 0 == dataSize)
        return -1;
    if (dataSize > srcSize) {
        av_fast_malloc(&srcData, (unsigned int *)&srcSize, dataSize);
    }
    memmove(srcData, data, dataSize);
    
    srcSize = dataSize;
    
    return 0;
}

uint8_t*  MediaFrame::readFrameData(int64_t& dataSize)
{
    dataSize = outSize;
    return dstData;
}


bool MediaFrame::Conver()
{
    bool ret = false;
    switch (frameType) {
        case PP_FRAME_VIDEO:
        {
            if (videoConver && isInitVideoParam) {
                videoConver->Conver(<#AVFrame *inframe#>, <#AVFrame *outframe#>);
                ret = true;
            } else {
                printf("MediaFrame: VideoConver is null\n");
            }
        }
        break;
        case PP_FRAME_AUDIO:
        {
            if (audioConver && isInitAudioParam) {
                // 重采样输出参数：输出音频样本数(多加了256个样本)
                int out_count = (int64_t)srcAudioParam.frame_size * dstAudioParam.sample_rate / srcAudioParam.sample_rate + 256;
                // 计算BufferSize
                int out_size = av_samples_get_buffer_size(NULL, dstAudioParam.channels, out_count, dstAudioParam.fmt, 0);
                
                if (dstSize < out_size) {
                    av_fast_malloc(&dstData, (unsigned int *)&dstSize, out_size);
                }
            
                int retSize = audioConver->Conver(dstData, srcData);
                int dstSizePerSample = dstAudioParam.channels * av_get_bytes_per_sample(dstAudioParam.fmt);
                outSize = retSize * dstSizePerSample;
                ret = true;
            } else {
                printf("MediaFrame: audioConver is null\n");
            }
        }
        break;
        default:
            break;
    }
    return ret;
}
NS_MEDIA_END

