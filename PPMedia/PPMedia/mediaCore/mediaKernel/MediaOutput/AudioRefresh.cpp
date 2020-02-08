//
//  AudioParamInfo.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/13.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "AudioRefresh.h"
#include "MediaPCMBuffer.h"

NS_MEDIA_BEGIN

void AudioRefresh::audio_callback(void *arg, unsigned char *stream, int len)
{
    AudioRefresh *pART = (AudioRefresh *)arg;
    // 获取当前时间
    int64_t audio_callback_time = av_gettime_relative();
    int audio_write_buf_size = 0;
    // 这步一定要有，否则声音会异常
    SDL_memset(stream, 0, len);
    // TODO
}

AudioRefresh::AudioRefresh():
volume(50)
{
}

AudioRefresh::~AudioRefresh()
{
    
}

/*
 * Audio输出现场初始化过程(参考ffplay)
 */
int AudioRefresh::init(MediaContext* mediaContext)
{
    // 获取目标参数，也就是输出的音频参数
    audioParam = mediaContext->dstAudioParam;
    int64_t wanted_channel_layout = audioParam.channel_layout;
    int wanted_nb_channels = audioParam.channels;
    int wanted_sample_rate = audioParam.sample_rate;
    // 这边一定要设置为主线程运行，否则接下来SDL_Init就会fail
    // 设置期望的音频channel layout、nb_samples、sample_rate等参数
    SDL_SetMainReady();
    SDL_AudioSpec wanted_spec, spec;
    
    const int next_nb_channels[] = {0, 0, 1, 6, 2, 6, 4, 6};
    const int next_sample_rates[] = {0, 44100, 48000, 96000, 192000};
    int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;
    
    // 这边是判断layout 返回的通道个数是否和传入的相等 wanted_channel_layout各个通道存储顺序,av_get_channel_layout_nb_channels根据通道的layout返回通道的个数
     if (wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)
         || !wanted_channel_layout) {
         wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
         wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
     }
    wanted_nb_channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
    // 设置通道个数
    wanted_spec.channels = wanted_nb_channels;
    // 设置采样率
    wanted_spec.freq = wanted_sample_rate;
    // 如果通道个数or采样率为0，则有问题
    if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
        av_log(NULL, AV_LOG_ERROR, "Invalid sample rate or channel count!\n");
        return -1;
    }
    // 从采样率数组中找到第一个不大于传入参数wanted_sample_rate的值
     // 音频采样格式有两大类型：planar和packed，假设一个双声道音频文件，一个左声道采样点记作L，一个右声道采样点记作R，则：
     // planar存储格式：(plane1)LLLLLLLL...LLLL (plane2)RRRRRRRR...RRRR
     // packed存储格式：(plane1)LRLRLRLR...........................LRLR
     // 在这两种采样类型下，又细分多种采样格式，如AV_SAMPLE_FMT_S16、AV_SAMPLE_FMT_S16P等，注意SDL2.0目前不支持planar格式
     // channel_layout是int64_t类型，表示音频声道布局，每bit代表一个特定的声道，参考channel_layout.h中的定义，一目了然
     while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq) {
         next_sample_rate_idx--;
     }
    // 采样格式：S表带符号，16是采样深度(位深)，SYS表采用系统字节序，这个宏在SDL中定义
    wanted_spec.format = AUDIO_S16SYS;
    // 设置静音
    wanted_spec.silence = 0;
    // SDL声音缓冲区尺寸，单位是单声道采样点尺寸x声道数
    // 一帧frame的大小
    wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
    wanted_spec.callback = audio_callback;
    // 提供给回调函数的参数
    // 打开音频设备并创建音频处理线程。期望的参数是wanted_spec，实际得到的硬件参数是spec
    // 1) SDL提供两种使音频设备取得音频数据方法：
    //    a. push，SDL以特定的频率调用回调函数，在回调函数中取得音频数据
    //    b. pull，用户程序以特定的频率调用SDL_QueueAudio()，向音频设备提供数据。此种情况wanted_spec.callback=NULL
    // 2) 音频设备打开后播放静音，不启动回调，调用SDL_PauseAudio(0)后启动回调，开始正常播放音频
    wanted_spec.userdata =  (void *)this;
    
    // 打开audio 设备
    while (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
        av_log(NULL, AV_LOG_WARNING, "SDL_OpenAudio (%d channels, %d Hz): %s\n",
               wanted_spec.channels, wanted_spec.freq, SDL_GetError());
        // 如果打开音频设备失败，则尝试用不同的声道数或采样率再试打开音频设备
        wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
        if (!wanted_spec.channels) {
            wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
            wanted_spec.channels = wanted_nb_channels;
            if (!wanted_spec.freq) {
                av_log(NULL, AV_LOG_ERROR,
                       "No more combinations to try, audio open failed\n");
                return -1;
            }
        }
        wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
    }
    // 检查打开音频设备的实际参数：采样格式
    if (spec.format != AUDIO_S16SYS) {
        av_log(NULL, AV_LOG_ERROR,
               "SDL advised audio format %d is not supported!\n", spec.format);
        return -1;
    }
    
    // 检查打开音频设备的实际参数：声道数
    if (spec.channels != wanted_spec.channels) {
        wanted_channel_layout = av_get_default_channel_layout(spec.channels);
        if (!wanted_channel_layout) {
            av_log(NULL, AV_LOG_ERROR,
                   "SDL advised channel count %d is not supported!\n", spec.channels);
            return -1;
        }
    }
    
    // wanted_spec是期望的参数，spec是实际的参数，wanted_spec和spec都是SDL中的结构。
    mediaContext->dstAudioParam.fmt = AV_SAMPLE_FMT_S16;
    mediaContext->dstAudioParam.sample_rate = spec.freq;
    mediaContext->dstAudioParam.channel_layout = wanted_channel_layout;
    mediaContext->dstAudioParam.channels =  spec.channels;
    mediaContext->dstAudioParam.frame_size = av_samples_get_buffer_size(NULL, mediaContext->dstAudioParam.channels, 1, mediaContext->dstAudioParam.fmt, 1);
    mediaContext->dstAudioParam.bytes_per_sec = av_samples_get_buffer_size(NULL, mediaContext->dstAudioParam.channels, mediaContext->dstAudioParam.sample_rate, mediaContext->dstAudioParam.fmt, 1);
    if (mediaContext->dstAudioParam.bytes_per_sec <= 0 || mediaContext->dstAudioParam.frame_size <= 0) {
        av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
        return -1;
    }
    // 输出设备的缓冲区大小，一般来说audio输出设备中有2两块buffer，进行交替渲染，所以说，在更新audio clock的时候，需要将这两块的buffer信息减去才是当当时最新播放的pts信息
    mediaContext->audio_hw_buf_size = spec.size;
    // 用此函数来暂停播放，或播放。根据参数来决定，如果参数是非0值就暂停，如果是0值就播放。
    SDL_PauseAudio(0);
    return spec.size;
}

/*
 * 设置音量
 */
void AudioRefresh::setVolume(int volume)
{
    this->volume = volume;
}

/*
 * 暂停,如果参数是非0值就暂停，如果是0值就播放。
 */
void AudioRefresh::pause(int pause_on)
{
    SDL_PauseAudio(pause_on);
    // TODO
    // audio时钟设置暂停
}

/*
 * 设置音量
 */
void AudioRefresh::stop()
{
    SDL_CloseAudio();
}

NS_MEDIA_END

