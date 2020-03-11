//
//  PacketQueueManger.cpp
//  PPlayer
//
//  Created by 邱开禄 on 2020/03/11.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#include "PacketQueueManger.h"
#include "MediaInit.h"
#include <pthread.h>

NS_MEDIA_BEGIN

PacketQueueManger::PacketQueueManger():
max_stream_num(MAX_DCODEC_STREAM_NUM)
{
    PacketQueueArray.reserve(8);
    for(int i = 0; i < max_stream_num ;i++) {
        PacketQueueArray.push_back(NULL);
    }
}

PacketQueueManger::~PacketQueueManger()
{
    
}

bool PacketQueueManger::CreatePacketQueue(int streamIndex)
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

bool PacketQueueManger::ReleasePacketQueue(int streamIndex)
{
    if(NULL == PacketQueueArray[streamIndex])
        return false;
    PacketQueueArray[streamIndex]->packet_queue_destroy();
    SAFE_DELETE(PacketQueueArray[streamIndex]);
    return true;
}

PacketQueue* PacketQueueManger::GetPacketQueue(int streamIndex)
{
    if(streamIndex > max_stream_num ||
       PacketQueueArray.size() < streamIndex) {
        printf("MediaContext: cur streaIndex > max_stream_num, so fail\n");
        return NULL;
    }
    return PacketQueueArray[streamIndex];
}

NS_MEDIA_END
