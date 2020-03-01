//
//  MediaInit.h
//  PPMedia
//  此部分代码参考ijkplayer的
//
//  Created by 邱开禄 on 2020/02/02.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaInit_H
#define MediaInit_H
#include <string>
#include "MediaCommon.h"
NS_MEDIA_BEGIN

class MediaInit {
public:
    MediaInit();
    
    ~MediaInit();
    
    /*
     * ffmpeg的初始化注册
     */
    static void Init();
    
    /*
     * ffmpeg 取消注册
     */
    static void UnInit();
    
    /*
     * 设置ffmpeg的log打印回调
     */
    static void SetLog();
    
    /*
     * 设置ffmpeg的log打印级别
     */
    static void setLogLevel(int log_level);
    
    /*
     * 参考ijkplayer,未实现
     */
    static void ffp_io_stat_register(void (*cb)(const char *url, int type, int bytes));
    
    /*
     * 参考ijkplayer,未实现
     */
    static void ffp_io_stat_complete_register(void (*cb)(const char *url,
                                                   int64_t read_bytes, int64_t total_size,
                                                   int64_t elpased_time, int64_t total_duration));

    static bool g_ffmpeg_global_inited;
    
    /*
     * 获取flush pkt
     */
    static AVPacket* getFlushPacket();
private:
    static AVPacket         *flushPkt;
};

NS_MEDIA_END
#endif // FFmpegInit_H
