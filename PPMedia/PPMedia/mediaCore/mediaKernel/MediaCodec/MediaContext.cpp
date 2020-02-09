//
//  MediaContext.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/02.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaContext.h"
#include <pthread.h>
NS_MEDIA_BEGIN

MediaContext::MediaContext():
formatContext(NULL),
informat(NULL),
nbStreams(0),
seek_request(0),
seek_flags(0),
seek_pos(0),
seek_rel(0),
keep_last(false),
eof(0),
last_vis_time(0),
frame_timer(0.0),
max_frame_duration(0.0),
frame_drops_late(0),
frame_drops_early(0),
audio_streams(0),
video_streams(0),
audio_hw_buf_size(0),
max_stream_num(MAX_DCODEC_STREAM_NUM),
audioCodecName(NULL),
videoCodecName(NULL),
lowres(0),
fast(false),
audioClock(NULL),
videoClock(NULL),
stopCodecThread(false),
isLoop(false),
demuxerThreadController(NULL),
decodeThreadController(NULL),
needSync(true),
remaining_time(0.0),
abort_request(false)
{
    PacketQueueArray.reserve(8);
    for(int i = 0; i < max_stream_num ;i++) {
        PacketQueueArray.push_back(NULL);
    }
}

MediaContext::~MediaContext()
{
    // 一般在哪个文件上申请，就在哪个文件下释放
    if (audioCodecName) {
        av_free(&audioCodecName);
    }
    if (videoCodecName) {
        av_free(&audioCodecName);
    }
}

bool MediaContext::CreatePacketQueue(int streamIndex)
{
    if(streamIndex > max_stream_num) {
        printf("MediaContext: cur streaIndex > max_stream_num, so fail\n");
        return false;
    }
    // 如果在当前这个PacketQueueArray的成员不为NULL,则先destroy后delete.
    if (NULL != PacketQueueArray[streamIndex]) {
        PacketQueueArray[streamIndex]->packet_queue_destroy();
        SAFE_DELETE(PacketQueueArray[streamIndex]);
    }
    PacketQueueArray[streamIndex] = new (std::nothrow)PacketQueue();
    if (NULL == PacketQueueArray[streamIndex]) {
        printf("MediaContext: new PacketQueue fail\n");
        return false;
    }
    PacketQueueArray[streamIndex]->packet_queue_init();
    PacketQueueArray[streamIndex]->packet_queue_start();
    return true;
}

bool MediaContext::ReleasePacketQueue(int streamIndex)
{
    if(NULL == PacketQueueArray[streamIndex])
        return false;
    PacketQueueArray[streamIndex]->packet_queue_destroy();
    SAFE_DELETE(PacketQueueArray[streamIndex]);
    return true;
}

PacketQueue* MediaContext::GetPacketQueue(int streamIndex)
{
    if(streamIndex > max_stream_num ||
       PacketQueueArray.size() < streamIndex) {
        printf("MediaContext: cur streaIndex > max_stream_num, so fail\n");
        return NULL;
    }
    return PacketQueueArray[streamIndex];
}

NS_MEDIA_END

