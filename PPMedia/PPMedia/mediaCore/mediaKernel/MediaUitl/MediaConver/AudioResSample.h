//
//  AudioResSample.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/05.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef AudioResSample_H
#define AudioResSample_H
#include <string>
#include "MediaDefine.h"
#include "ConverBase.h"
#include "MediaParamInfo.h"

NS_MEDIA_BEGIN

class AudioResSample : public ConverBase
{
public:
    AudioResSample();
    
    ~AudioResSample();
    /*
     * 初始化
     */
    virtual int init(AudioParamInfo srcAudioParam, AudioParamInfo dstAudioParam);
    
    /*
     * 初始化
     */
    virtual int release();

    /*
     * 格式转换者
     */
    virtual int Conver(AVFrame *inframe, AVFrame *outframe);
    
    /*
     * 格式转换者
     */
    int Conver(uint8_t **out, int out_samples, AVFrame* frame);

    /*
     * 格式转换者
     */
    int Conver(uint8_t *out, uint8_t* in);
    
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
#endif // AudioResSample_H
