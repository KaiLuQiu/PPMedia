//
//  MediaClock.h
//  时钟机制参考ffplay
//  PPMedia
//
//  Created by 邱开禄 on 2019/11/14.
//  Copyright © 2019 邱开禄. All rights reserved.
//


#ifndef MediaClock_H
#define MediaClock_H
#include "MediaCommon.h"
#include "MediaContext.h"

NS_MEDIA_BEGIN

class MediaClock
{
public:
    /*
     * 时钟初始化
     */
    static void init_clock(Clock *c, int *queue_serial);

    /*
     * 获取当前显示的pts(这个是一个估算值)
     */
    static double get_clock(Clock *c);

    /*
     * 更新clock
     */
    static void set_clock_at(Clock *c, double pts, int serial, double time);

    /*
     * 获取系统时间，更新clock
     */
    static void set_clock(Clock *c, double pts, int serial);
    
    /*
     * 设置播放速度，更新clock
     */
    static void set_clock_speed(Clock *c, double speed);
    
//    /*
//     * 用于同步外部时钟的
//     */
//    static void sync_clock_to_slave(Clock *c, Clock *slave);

    /*
     * 获取当前的主时钟（目前只支持视频同步音频）
     */
    static double get_master_clock(MediaContext *mediaContext);

private:
};


NS_MEDIA_END

#endif // MediaClock_H.h
