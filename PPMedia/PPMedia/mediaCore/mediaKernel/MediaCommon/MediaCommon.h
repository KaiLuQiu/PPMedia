//
//  MediaCommon.h
//  PPlayer
//
//  Created by 邱开禄 on 2019/11/14.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#ifndef MediaCommon_H
#define MediaCommon_H

#include <list>
#include <vector>
#include <string.h>
#include <stdlib.h>

extern "C"{
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/rational.h>
    #include <libavutil/time.h>
    #include <libavutil/samplefmt.h>
}
#include "MediaDefine.h"


// 时钟结构体参考ffplay
typedef struct Clock_T {
    Clock_T()
    {
        pts = 0.0;
        pts_drift = 0.0;
        last_updated = 0.0;
        speed = 1.0;
        serial = 0;
        paused = 0;
        queue_serial = NULL;
    }
    ~Clock_T()
    {
        pts = 0.0;
        pts_drift = 0.0;
        last_updated = 0.0;
        speed = 1.0;
        serial = 0;
        paused = 0;
        SAFE_DELETE(queue_serial);
    }
    // 显示时间戳，播放后，当前帧变成上一帧
    double pts;
    // 当前帧显示时间戳与当前系统时钟时间的差值
    double pts_drift;
    // 当前时钟(如视频时钟)最后一次更新时间，也可称当前时钟时间
    double last_updated;
    // 时钟速度控制，用于控制播放速度
    double speed;
    // 播放序列，所谓播放序列就是一段连续的播放动作，一个seek操作会启动一段新的播放序列
    int serial;
    // 暂停标志
    int paused;
    // 指向packet_serial
    int *queue_serial;
} Clock;


#endif

