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
#include "PacketQueue.h"
NS_MEDIA_BEGIN

class MediaContext {
public:
    MediaContext();
    
    ~MediaContext();
    
    bool CreatePacketQueue(int streamIndex);
    
    bool ReleasePacketQueue(int streamIndex);
    
    PacketQueue *GetPacketQueue(int streamIndex);

public:
    // FFmpeg上下文
    AVFormatContext*    formatContext;

    AVInputFormat*      Informat;
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
    // 媒体流的数量
    int                 nbStreams;
    // audio流的数量
    int                 audio_streams;
    // video流的数量
    int                 video_streams;
    // SDL音频缓冲区大小(单位字节)
    int                 audio_hw_buf_size;
    // 最大的流数量
    int                 max_stream_num;
    // 不同流对应的流packetQueue
    std::vector<PacketQueue *> PacketQueueArray;
    // 参考https://www.jianshu.com/p/8de0fc796ef9
    // 指定音频解码器
    const char*         audioCodecName;
    // 指定视频解码器
    const char*         videoCodecName;
    // 以下设置播放的一些参数
    // 分辨率参数
    int                 lowres;
    // Allow non spec compliant speedup tricks.
    bool                fast;
    
    
private:
};

NS_MEDIA_END
#endif // MediaContext_H
