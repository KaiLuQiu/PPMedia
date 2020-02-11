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
#include "MediaParamInfo.h"
#include "ThreadController.h"
#include "MediaPipeline.h"
#include "MediaPipelineNode.h"

NS_MEDIA_BEGIN

class MediaContext {
public:
    MediaContext();
    
    ~MediaContext();
    
    bool CreatePacketQueue(int streamIndex);
    
    bool ReleasePacketQueue(int streamIndex);
    
    PacketQueue *GetPacketQueue(int streamIndex);

public:
    /*********************************************************音视频同步相关*********************************************************/
    // 是否进行avsync
    bool                            needSync;
    // 视频超前音频，需要等待的剩余时间
    double                          remaining_time;
    // 时钟信息设置
    Clock*                          audioClock;
    Clock*                          videoClock;
    // 当前frame对应实际时间的累积值
    double                          frame_timer;
    // 上一次的播放时间
    int                             last_vis_time;
    // maximum duration of a frame - above this, we consider the jump a  timestamp discontinuity
    double                          max_frame_duration;
    // 记录drop video的数量
    int                             frame_drops_late;
    int                             frame_drops_early;
    
    /*********************************************************软解硬件解相关*********************************************************/
    // 对应的解码器node可以映射到软件解码或者对应平台的硬件解码
    Decode_Pipenode*                 decodec_node;
    Aout_Pipenode*                   aout_node;
    
    /********************************************************媒体流控制相关*********************************************/
    // FFmpeg上下文
    AVFormatContext*                formatContext;

    AVInputFormat*                  informat;
    // 是否parse到类eof标识位
    int                             eof;
    // 参考https://www.jianshu.com/p/8de0fc796ef9
    // 指定音频解码器
    const char*                     audioCodecName;
    // 指定视频解码器
    const char*                     videoCodecName;
    // 是否需要循环播放这个媒体文件
    bool                            isLoop;
    // 视频播放终端标志位
    bool                            abort_request;
    
    /********************************************************seek操作相关*************************************************/
    // 标识一次SEEK请求
    int                             seek_request;
    // SEEK标志，诸如AVSEEK_FLAG_BYTE等
    int                             seek_flags;
    // SEEK的目标位置(当前位置+增量)
    int64_t                         seek_pos;
    // 本次SEEK的位置增量
    int64_t                         seek_rel;
    // 是否保存最后一帧
    bool                            keep_last;

    /********************************************************媒体流信息相关*************************************************/
    // 媒体流的数量
    int                             nbStreams;
    // audio流的数量
    int                             audio_streams;
    // video流的数量
    int                             video_streams;
    // 最大的流数量
    int                             max_stream_num;
    // 不同流对应的流packetQueue
    std::vector<PacketQueue *>      PacketQueueArray;

    /********************************************************音视频参数信息相关*************************************************/
    // 音频参数信息
    AudioParamInfo                  srcAudioParam;
    AudioParamInfo                  dstAudioParam;
    // 视频参数信息
    videoParamInfo                  srcVideoParam;
    videoParamInfo                  dstVideoParam;
    // SDL音频缓冲区大小(单位字节)
    int                             audio_hw_buf_size;
    
    /********************************************************播放的一些参数相关**************************************************/
    // 分辨率参数
    int                             lowres;
    // Allow non spec compliant speedup tricks.
    bool                            fast;
    
    /*********************************************************线程控制相关*********************************************************/
    // 线程控制标志位
    bool                            stopCodecThread;
    // 多线程同步相关（控制demuxer线程）
    ThreadController*               demuxerThreadController;
    // 多线程同步相关（控制decode线程）
    ThreadController*               decodeThreadController;

private:
};

NS_MEDIA_END
#endif // MediaContext_H
