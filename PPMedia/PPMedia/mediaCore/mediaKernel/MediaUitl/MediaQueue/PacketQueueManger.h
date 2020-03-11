//
//  PacketQueueManger.h
//  PPlayer
//
//  Created by 邱开禄 on 2020/03/11.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#ifndef PacketQueueManger_H
#define PacketQueueManger_H
#include "MediaCommon.h"
#include <pthread.h>
#include "PacketQueue.h"

NS_MEDIA_BEGIN

class PacketQueueManger
{
public:
    PacketQueueManger();
    
    ~PacketQueueManger();
    
    bool CreatePacketQueue(int streamIndex);
    
    bool ReleasePacketQueue(int streamIndex);
    
    PacketQueue *GetPacketQueue(int streamIndex);
public:

    // 不同流对应的流packetQueue
    std::vector<PacketQueue *>      PacketQueueArray;
    // 最大的流数量
    int                             max_stream_num;
};


NS_MEDIA_END
#endif // PacketQueue.h
