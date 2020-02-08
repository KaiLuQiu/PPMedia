//
//  MediaSync.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/09.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaSync_H
#define MediaSync_H
#include "MediaCommon.h"
#include "FrameQueue.h"
#include "MediaContext.h"

NS_MEDIA_BEGIN

#define AV_SYNC_THRESHOLD_MIN 0.04
#define AV_SYNC_THRESHOLD_MAX 0.1
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
// 定义avsync不同步的阈值
#define AV_NOSYNC_THRESHOLD 10.0


// 音视频同步模式
enum AV_SYNC_MODE{
    // 同步音频
    SYNC_AUDIO_CLOCK,
    // 同步系统时钟
    SYNC_SYSTEM_CLOCK,
    // 同步视频
    SYNC_VIDEO_CLOCK,
};

class MediaSync
{
public:
    MediaSync();
    ~MediaSync();
    
    /*
     * 获取时钟同步方式
     */
    static int get_master_sync_type(MediaContext *mediaContext);
    
    /*
     * 计算当前现实这笔的pts和下一笔的pts的差值，如果duration 满足如下几种情况，则使用包的duration， 否则使用差值duration
     */
    static double vp_duration(Frame *vp, Frame *nextvp, MediaContext *mediaContext);
    
    /*
     * 根据当前的视频和主时钟（audio时钟）计算差值diff,根据不同情况调整delay值
     */
    static double compute_target_delay(double delay, MediaContext *mediaContext);
    
    /*
     * 更新video的pts
     */
    static void update_video_clock(double pts, int serial, MediaContext *mediaContext);
private:

};

NS_MEDIA_END
#endif // MediaSync_H
