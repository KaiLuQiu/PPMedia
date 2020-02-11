//
//  MediaInit.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/02.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaInit.h"
#include <pthread.h>
NS_MEDIA_BEGIN
//类静态变量，一定要在外部定义
AVPacket    *MediaInit::flushPkt = NULL;
bool        MediaInit::isInited = false;

//使用多个线程同时播放多个视频源的时候，在调用avcodec_open/close的时候，可能导致失败，这个可以查阅ffmpeg的源码分析其中的原因，失败的主要原因是在调用此2函数时，ffmpeg为了确保该2函数为原子操作，在avcodec_open/close两函数的开头和结尾处使用了一个变量entangled_thread_counter来记录当前函数是否已经有其他线程进入，如果有其他线程正在此2函数内运行，则会调用失败。解决此问题可使用函数

static int ff_lockmgr_callback(void**mutex, enum AVLockOp op)
{
    switch (op) {
        // 创建锁
        case AV_LOCK_CREATE: {
            *mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
            pthread_mutex_init((pthread_mutex_t*)*mutex, NULL);
        }
        break;
        case AV_LOCK_OBTAIN: {
            pthread_mutex_lock((pthread_mutex_t*)*mutex);
        }
        break;
        case AV_LOCK_RELEASE: {
            pthread_mutex_unlock((pthread_mutex_t*)*mutex);
        }
        break;
        case AV_LOCK_DESTROY: {
            if(*mutex) {
                pthread_mutex_destroy((pthread_mutex_t*)*mutex);
                free(*mutex);
                *mutex = NULL;
            }
        }
        break;
        default:
            break;
    }
    return 0;
}

//ffmpeg log打印
static void ff_log_callback(void*avcl, int level, const char*fmt, va_list vl)
{
    char log[1024];
    vsnprintf(log,sizeof(log),fmt,vl);
    printf("%s\n",log);
}

MediaInit::MediaInit()
{
}

MediaInit::~MediaInit()
{
    
}

void MediaInit::init()
{
    if(true == isInited) {
        return;
    }
    if(NULL == flushPkt) {
        flushPkt = av_packet_alloc();
        flushPkt->data = (uint8_t*)flushPkt;
    }
    av_register_all();
    avformat_network_init();
    av_log_set_callback(ff_log_callback);
    av_lockmgr_register(ff_lockmgr_callback);
    isInited = true;
}

void MediaInit::release()
{
    if(false == isInited) {
        return;
    }
    if(flushPkt) {
        flushPkt->data = NULL;
        av_packet_free(&flushPkt);
        flushPkt = NULL;
    }
    av_lockmgr_register(NULL);
    avformat_network_deinit();
    isInited = false;
}

bool MediaInit::Inited()
{
    return isInited;
}

AVPacket* MediaInit::getFlushPacket()
{
    return flushPkt;
}
NS_MEDIA_END

