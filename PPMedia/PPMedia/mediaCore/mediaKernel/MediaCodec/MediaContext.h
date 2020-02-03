//
//  MediaContext.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaContext_H
#define MediaContext_H
#include <string>
#include "MediaCommon.h"
NS_MEDIA_BEGIN

class MediaContext {
public:
    MediaContext();
    
    ~MediaContext();
    
    
public:
    // FFmpeg上下文
    AVFormatContext     *formatContext;
    // 媒体流的数量
    int                 nbStreams;
    AVInputFormat       *avInformat;
    // 标识一次SEEK请求
    int                 seek_request;
    // SEEK标志，诸如AVSEEK_FLAG_BYTE等
    int                 seek_flags;
    // SEEK的目标位置(当前位置+增量)
    int64_t             seek_pos;
    // 本次SEEK的位置增量
    int64_t             seek_rel;
    // 是否保存最后一帧
    bool                keep_last;
    // 视频宽高信息
    int                 width;
    int                 height;
    // 是否parse到类eof标识位
    int                 eof;
    // 上一次的播放时间
    int                 last_vis_time;
    // 当前frame对应实际时间的累积值
    double              frame_timer;
    // maximum duration of a frame - above this, we consider the jump a  timestamp discontinuity
    double              max_frame_duration;
    // 记录drop video的数量
    int                 frame_drops_late;
    int                 frame_drops_early;
    // 流的总数量
    int                 nb_streams;
    // audio流的数量
    int                 audio_streams;
    // video流的数量
    int                 video_streams;
    // SDL音频缓冲区大小(单位字节)
    int                 audio_hw_buf_size;
private:
};

NS_MEDIA_END
#endif // MediaContext_H
