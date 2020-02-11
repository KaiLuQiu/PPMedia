//
//  MediaStream.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/09.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaSync.h"
#include "MediaClock.h"

NS_MEDIA_BEGIN
MediaSync::MediaSync()
{
    
}

MediaSync::~MediaSync()
{
    
}

double MediaSync::vp_duration(Frame *vp, Frame *nextvp, MediaContext *mediaContext)
{
    if (vp->serial == nextvp->serial) {
        double duration = nextvp->pts - vp->pts;
        if (isnan(duration) || duration <= 0 || duration > mediaContext->max_frame_duration)
            return vp->duration;
        else
            return duration;
    } else {
        return 0.0;
    }
}

double MediaSync::compute_target_delay(double delay, MediaContext *mediaContext)
{
    double sync_threshold, diff = 0;

    // 计算video的时钟和audio时钟的差值
    diff = MediaClock::get_clock(mediaContext->videoClock) - MediaClock::get_master_clock(mediaContext);

    /* skip or repeat frame. We take into account the
       delay to compute the threshold. I still don't know
       if it is the best guess */
    sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
    if (!isnan(diff) && fabs(diff) < mediaContext->max_frame_duration) {
        // 如果当前落后audio 则应该丢帧
        if (diff <= -sync_threshold)
            delay = FFMAX(0, delay + diff);
        // 如果超前应该等待更久的时间
        else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
            delay = delay + diff;
        else if (diff >= sync_threshold)
            delay = 2 * delay;
    }

    printf("avsync: video refresh thread delay=%0.3f A-V=%f\n",
            delay, -diff);

    return delay;
}

void MediaSync::update_video_clock(double pts, int serial, MediaContext *mediaContext)
{
    MediaClock::set_clock(mediaContext->videoClock, pts, serial);
}

int MediaSync::get_master_sync_type(MediaContext *mediaContext) {
    // 目前默认采用视频sync音频
    return SYNC_AUDIO_CLOCK;
}


NS_MEDIA_END


