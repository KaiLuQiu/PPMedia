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
frameType(PP_FRAME_NONE)
{
    videoConver = new (std::nothrow)VideoConver();
    videoConver->init();
    audioConver = NULL;
}
MediaFrame::~MediaFrame()
{
    
}

void MediaFrame::setSrcAudioParam(AudioParamInfo srcParam)
{
    // 如果当前的输入参数和dstAudioParam(表示音频输出的)不同的话，则初始化重采样器
    if (dstAudioParam.channels != srcParam.channels ||
        dstAudioParam.sample_rate != srcParam.sample_rate ||
        dstAudioParam.frame_size != srcParam.frame_size ||
        dstAudioParam.fmt != srcParam.fmt) {
        // 表明输入和输出的音频格式不一样，则进行重采样操作
        if (audioConver) {
            audioConver->release();
            delete audioConver;
        }
        audioConver = new AudioResSample();
        // 对重采样器设置输入输出的音频参数
        audioConver->init(srcParam, dstAudioParam);
        
        
        
    }
//    srcAudioParam = srcParam;

//    dstAudioParam = dstParam;
}

void MediaFrame::setDstAudioParam(AudioParamInfo srcParam)
{
    
}

void MediaFrame::setVideoParam(videoParamInfo srcParam, videoParamInfo dstParam)
{
    srcVideoParam = srcParam;
    dstVideoParam = dstParam;
}

NS_MEDIA_END

