//
//  AudioRefresh.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/13.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef AudioRefresh_H
#define AudioRefresh_H
#include <string>
#include "MediaCommon.h"
#include "MediaContext.h"
#include "AudioParamInfo.h"
NS_MEDIA_BEGIN

#define SDL_AUDIO_MIN_BUFFER_SIZE 512
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

class AudioRefresh
{
public:
    AudioRefresh();
    
    ~AudioRefresh();
    
    /*
     * Audio输出现场初始化过程
     */
    int init(MediaContext* mediaContext);
    
    /*
     * 设置音量
     */
    void setVolume(int volume);
    
    /*
     * 暂停
     */
    void pause(int pause_on);
    
    /*
     * 停止
     */
    void stop();
    
private:
    /*
     * SDL audio的callback
     */
    static void audio_callback(void *arg, unsigned char *stream, int len);
    
private:
    AudioParamInfo          audioParam;
    int                     volume;
};

NS_MEDIA_END
#endif // AudioRefresh_H
