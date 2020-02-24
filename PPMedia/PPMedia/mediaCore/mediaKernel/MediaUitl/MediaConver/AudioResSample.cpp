//
//  AudioResSample.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/05.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "AudioResSample.h"
NS_MEDIA_BEGIN

AudioResSample::AudioResSample():
swrContex(NULL)
{
    
}

AudioResSample::~AudioResSample()
{
    if(swrContex) {
        swr_free(&swrContex);
        swrContex = NULL;
    }
}

/*
 * 初始化
 */
int AudioResSample::init(AudioParamInfo srcAudioParam, AudioParamInfo dstAudioParam)
{
    this->srcParam = srcAudioParam;
    this->dstParam = dstAudioParam;
    
    if(swrContex == NULL) {
        swrContex = swr_alloc();
    }
    
    // 对音频转上下文设置参数信息
    // 使用pFrame(源)和p_PlayerContext->audioInfoTarget(目标)中的音频参数来设置p_SwrContex
    swrContex = swr_alloc_set_opts(swrContex, dstParam.channel_layout, dstParam.fmt,
    dstParam.sample_rate, srcParam.channel_layout, srcParam.fmt, srcParam.sample_rate, 0, NULL);

    
    if (NULL == swrContex || swr_init(swrContex) < 0) {
        printf( "Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
               srcParam.sample_rate, av_get_sample_fmt_name((AVSampleFormat)srcParam.fmt), srcParam.channels,
               dstParam.sample_rate, av_get_sample_fmt_name((AVSampleFormat)dstParam.fmt), dstParam.channels);
        swr_free(&swrContex);
        return -1;
    }
    return 1;
}

/*
 * 初始化
 */
int AudioResSample::release()
{
    if(swrContex) {
        swr_free(&swrContex);
        swrContex = NULL;
    }
    return 1;
}

void AudioResSample::updateSrcAudioInfo(AudioParamInfo params)
{
    this->srcParam = params;
}

int AudioResSample::Conver(uint8_t **out, int out_samples, AVFrame* frame)
{
    int resampled_data_size;
    
    if (!frame || !out) {
        return 0;
    }
    
    const uint8_t **in = (const uint8_t **)frame->extended_data;

    // 音频重采样：返回值是重采样后得到的音频数据中单个声道的样本数
    int len = swr_convert(swrContex, out, out_samples, in, frame->nb_samples);
    if (len < 0) {
        return 0;
    }
    
    // 重采样返回的一帧音频数据大小(以字节为单位)
    resampled_data_size = len * dstParam.channels * av_get_bytes_per_sample(dstParam.fmt);
    return resampled_data_size;
}

/*
 * 格式转换者(这个用于编码)
 */
int AudioResSample::Conver(AVFrame *inframe, AVFrame *outframe)
{
    int ret;
    if (NULL == swrContex || NULL == outframe || NULL == inframe) {
        printf("AudioResSample: swrContex or outframe or inframe is NULL fail!!!\n");
    }
    ret = swr_convert_frame(swrContex, outframe, inframe);
    if(ret < 0) {
        printf("AudioResSample: swr_convert_frame fail!!!\n");
        return ret;
    }
    return ret;
}

/*
* 格式转换者
*/
int AudioResSample::Conver(uint8_t *out, uint8_t* in)
{
    int ret;
    if (NULL == swrContex || NULL == out || NULL == in) {
        printf("AudioResSample: swrContex or out or in is NULL fail\n");
        return -1;
    }
    // 其中srcData是一个数组指针，内部有8个指针元素，相当于最多指向8个通道的数据，因为AVFrame数据格式就是这样的，8个行的数据
    uint8_t* srcData[8] = {NULL};
    uint8_t* dstData[8] = {NULL};
    // 因此也需要8行的lineszie，记录每一行data的长度大小
    int srcLineSize[8] = {0};
    int dstLineSize[8] = {0};
    // 目标帧数
    int dst_nb_samples = 0;
    int src_nb_samples = srcParam.frame_size;
    // 填充srcData
    ret = av_samples_fill_arrays(srcData, srcLineSize, in, srcParam.channels, src_nb_samples, srcParam.fmt, 1);
    if (ret < 0) {
        printf("AudioResSample: av_samples_fill_arrays fail\n");
        return -1;
    }
    /* compute destination number of samples */
    dst_nb_samples = av_rescale_rnd(swr_get_delay(swrContex, srcParam.sample_rate) +
                                        src_nb_samples, dstParam.sample_rate, srcParam.sample_rate, AV_ROUND_UP);
    // 补偿处理
    if (dstParam.frame_size > 0 && srcParam.frame_size != dstParam.frame_size) {
        ret = swr_set_compensation(swrContex, (dstParam.frame_size - srcParam.frame_size) * dstParam.sample_rate / srcParam.sample_rate, dstParam.frame_size * dstParam.sample_rate / srcParam.sample_rate);
        if (ret < 0) {
            printf("AudioResSample: swr_set_compensation fail\n");
            return -1;
        }
        dst_nb_samples = dstParam.frame_size;
    }
    
    // 填充dstData
    ret = av_samples_fill_arrays(dstData, dstLineSize, out, dstParam.channels, dst_nb_samples, dstParam.fmt, 1);
    if (ret < 0) {
        printf("AudioResSample: av_samples_fill_arrays fail\n");
        return -1;
    }
    //int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
    //                                const uint8_t **in , int in_count);
    ret = swr_convert(swrContex, &out, dst_nb_samples, (const uint8_t **)srcData, src_nb_samples);
    if (ret < 0) {
        printf("AudioResSample: swr_convert fail\n");
        return -1;
    }
    return 0;
}

int AudioResSample::flush(uint8_t *out, int dst_nb_samples)
{
    int ret;
    uint8_t* dstData[8] = {NULL};
    int dstLineSize[8] = {0};
    // 填充dstData
    ret = av_samples_fill_arrays(dstData, dstLineSize, out, dstParam.channels, dst_nb_samples, dstParam.fmt, 1);
    if (ret < 0) {
        printf("AudioResSample: av_samples_fill_arrays fail\n");
        return -1;
    }
    ret = swr_convert(swrContex, dstData, dst_nb_samples, NULL, 0);
    if (ret < 0) {
        printf("AudioResSample: swr_convert fail\n");
        return ret;
    }
    return ret;
}
NS_MEDIA_END

