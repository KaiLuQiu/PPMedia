//
//  AudioResSample.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/05.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaFrame_H
#define MediaFrame_H
#include <string>
#include "MediaCommon.h"
#include "TranferBase.h"

NS_MEDIA_BEGIN
typedef struct AudioParamInfo_T
{
    AudioParamInfo_T():
    channels(0),
    channel_layout(0),
    fmt(AV_SAMPLE_FMT_NONE),
    frame_size(0),
    bytes_per_sec(0),
    sample_rate(0) {
    }
    
    ~AudioParamInfo_T()
    {
    
    }
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
}AudioParamInfo;
// 音频码率计算 = 音频的采样率 * 通道数 * 格式位数；
// 文件大小（M） = 时间 * 码率 / 8 / 1024；
// 每一帧数据 = 音频码率计算 / 8 * 25;

// 假设音频采样率 = 8000，采样通道 = 2，位深度 = 8，采样间隔 = 20ms

// 首先我们计算一秒钟总的数据量，采样间隔采用20ms的话，说明每秒钟需采集50次(1s=1000ms)，那么总的数据量计算为

// 一秒钟总的数据量 =8000 * 2*8/8 = 16000（Byte）

// 所以每帧音频数据大小 = 16000/50 =320（Byte）

// 每个通道样本数 = 320/2 = 160（Byte）

class AudioResSample : public TranferBase
{
public:
    AudioResSample();
    
    ~AudioResSample();
    /*
     * 初始化
     */
    virtual int init();
    
    /*
     * 初始化
     */
    virtual int release();

    /*
     * 格式转换者
     */
    virtual int tranfer(AVFrame *inframe, AVFrame *outframe);
    
    /*
     * 格式转换者
     */
    int tranfer(uint8_t **out, int out_samples, AVFrame* frame);

    /*
     * 格式转换者
     */
    int tranfer(uint8_t *out, uint8_t* in);
    
    /*
     * 设置输入输出的格式参数
     */
    void setAudioInfo(AudioParamInfo srcAudioParam, AudioParamInfo dstAudioParam);
    
    /*
     * 更新输入的音频参数
     */
    void updateSrcAudioInfo(AudioParamInfo params);
    
    int flush(uint8_t *out, int dst_nb_samples);
private:
    // 音视频转码上下文
    SwrContext          *swrContex;
    AudioParamInfo      srcParam;
    AudioParamInfo      dstParam;
    
};

NS_MEDIA_END
#endif // MediaFrame_H
