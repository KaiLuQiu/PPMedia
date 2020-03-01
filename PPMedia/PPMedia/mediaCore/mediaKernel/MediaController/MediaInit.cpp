//
//  MediaInit.cpp
//  PPMedia
//  参考ijkplayer的
//
//  Created by 邱开禄 on 2020/02/02.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaInit.h"
#include <pthread.h>
NS_MEDIA_BEGIN
//类静态变量，一定要在外部定义
bool MediaInit::g_ffmpeg_global_inited = false;
AVPacket    *MediaInit::flushPkt = NULL;

//使用多个线程同时播放多个视频源的时候，在调用avcodec_open/close的时候，可能导致失败，这个可以查阅ffmpeg的源码分析其中的原因，失败的主要原因是在调用此2函数时，ffmpeg为了确保该2函数为原子操作，在avcodec_open/close两函数的开头和结尾处使用了一个变量entangled_thread_counter来记录当前函数是否已经有其他线程进入，如果有其他线程正在此2函数内运行，则会调用失败。解决此问题可使用函数

//static int ff_lockmgr_callback(void **mtx, enum AVLockOp op)
//{
//    switch (op) {
//    case AV_LOCK_CREATE:
//        *mtx = SDL_CreateMutex();
//        if (!*mtx) {
//            av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
//            return 1;
//        }
//        return 0;
//    case AV_LOCK_OBTAIN:
//        return !!SDL_LockMutex(*mtx);
//    case AV_LOCK_RELEASE:
//        return !!SDL_UnlockMutex(*mtx);
//    case AV_LOCK_DESTROY:
//        SDL_DestroyMutex(*mtx);
//        return 0;
//    }
//    return 1;
//}

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
//static void ff_log_callback(void*avcl, int level, const char*fmt, va_list vl)
//{
//    char log[1024];
//    vsnprintf(log,sizeof(log),fmt,vl);
//    printf("%s\n",log);
//}

inline static int log_level_av(int av_level)
{
    int pp_level = PP_LOG_VERBOSE;
    if      (av_level <= AV_LOG_PANIC)      pp_level = PP_LOG_FATAL;
    else if (av_level <= AV_LOG_FATAL)      pp_level = PP_LOG_FATAL;
    else if (av_level <= AV_LOG_ERROR)      pp_level = PP_LOG_ERROR;
    else if (av_level <= AV_LOG_WARNING)    pp_level = PP_LOG_WARN;
    else if (av_level <= AV_LOG_INFO)       pp_level = PP_LOG_INFO;
    // AV_LOG_VERBOSE means detailed info
    else if (av_level <= AV_LOG_VERBOSE)    pp_level = PP_LOG_INFO;
    else if (av_level <= AV_LOG_DEBUG)      pp_level = PP_LOG_DEBUG;
    else if (av_level <= AV_LOG_TRACE)      pp_level = PP_LOG_VERBOSE;
    else                                    pp_level = PP_LOG_VERBOSE;
    return pp_level;
}

static void ff_log_callback(void *ptr, int level, const char *fmt, va_list vl)
{
    if (level > av_log_get_level())
        return;

    int ffplv __unused = log_level_av(level);
    VLOG(ffplv, "PPMedia", fmt, vl);
}

MediaInit::MediaInit()
{
}

MediaInit::~MediaInit()
{
    
}

void MediaInit::Init()
{
    if (g_ffmpeg_global_inited)
        return;
    
    /* register all codecs, demux and protocols */
    avcodec_register_all();
//#if CONFIG_AVDEVICE
//    avdevice_register_all();
//#endif
//#if CONFIG_AVFILTER
//    avfilter_register_all();
//#endif
    av_register_all();
    
    avformat_network_init();

    av_lockmgr_register(ff_lockmgr_callback);
    
    av_log_set_callback(ff_log_callback);

    if(NULL == flushPkt) {
        flushPkt = av_packet_alloc();
        flushPkt->data = (uint8_t*)flushPkt;
    }

    g_ffmpeg_global_inited = true;
}

void MediaInit::UnInit()
{
    if(false == g_ffmpeg_global_inited) {
        return;
    }

    if(flushPkt) {
        flushPkt->data = NULL;
        av_packet_free(&flushPkt);
        flushPkt = NULL;
    }

    av_lockmgr_register(NULL);
    avformat_network_deinit();
    g_ffmpeg_global_inited = false;
}

void MediaInit::SetLog()
{
    av_log_set_callback(ff_log_callback);
}

void MediaInit::setLogLevel(int log_level)
{
    int av_level = log_level_av(log_level);
    av_log_set_level(av_level);
}

AVPacket* MediaInit::getFlushPacket()
{
    return flushPkt;
}

void MediaInit::ffp_io_stat_register(void (*cb)(const char *url, int type, int bytes))
{
    
}

void MediaInit::ffp_io_stat_complete_register(void (*cb)(const char *url,
                                               int64_t read_bytes, int64_t total_size,
                                               int64_t elpased_time, int64_t total_duration))
{
    
}

NS_MEDIA_END

