//
//  AudioParamInfo.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/05.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef AudioParamInfo_H
#define AudioParamInfo_H
#include <string>
#include "MediaCommon.h"
NS_MEDIA_BEGIN

class AudioParamInfo
{
public:
    AudioParamInfo();
    ~AudioParamInfo();
    
public:
    // 通道数
    int                 channels;
    // 数据的排放方式
    int64_t             channel_layout;
    // 音频格式
    enum AVSampleFormat fmt;
    // 音频一帧的数据量如；ACC 就是1024
    int                 frame_size;
    // 每秒的byte大小。
    int                 bytes_per_sec;
    // 采样率
    int                 sample_rate;
};
// 音频码率计算 = 音频的采样率 * 通道数 * 格式位数；
// 文件大小（M） = 时间 * 码率 / 8 / 1024；
// 每一帧数据 = 音频码率计算 / 8 * 25;

// 假设音频采样率 = 8000，采样通道 = 2，位深度 = 8，采样间隔 = 20ms

// 首先我们计算一秒钟总的数据量，采样间隔采用20ms的话，说明每秒钟需采集50次(1s=1000ms)，那么总的数据量计算为

// 一秒钟总的数据量 =8000 * 2*8/8 = 16000（Byte）

// 所以每帧音频数据大小 = 16000/50 =320（Byte）

// 每个通道样本数 = 320/2 = 160（Byte）

NS_MEDIA_END
#endif // AudioParamInfo_H