//
//  PacketQueue.h
//  PPlayer
//
//  Created by 邱开禄 on 2019/11/14.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#ifndef PacketQueue_H
#define PacketQueue_H
#include "MediaCommon.h"
NS_MEDIA_BEGIN

typedef struct P_AVPacket_T {
    P_AVPacket_T()
    {
        memset(&pkt, 0, sizeof(AVPacket));
        serial = 0;
    }
    ~P_AVPacket_T()
    {
        serial = 0;
    }
    AVPacket pkt;           //demuxer parse出来的包信息
    int serial;             //序号信息
} P_AVPacket;

class PacketQueue
{
public:
    PacketQueue();
    ~PacketQueue();
    
    /*
     * 队列初始化的过程
     */
    int packet_queue_init(void);
    
    /*
     * 启用队列
     */
    void packet_queue_start(void);
    
    /*
     * 从队列中获取一个pkt
     */
    int packet_queue_get(AVPacket *pkt, int block, int *serial);

    /*
     * push一个pkt
     */
    int packet_queue_put_private(AVPacket *pkt);
    
    /*
     * push一个pkt 加锁
     */
    int packet_queue_put(AVPacket *pkt);
    
    /*
     * push一个空pkt
     */
    int packet_queue_put_nullpacket(int stream_index);
 
    /*
     * flush pkt中的数据
     */
    void packet_queue_flush();
    
    /*
     * 销毁
     */
    void packet_queue_destroy();
    
    /*
     * 中止，设置标识为，组织放入packet
     */
    void packet_queue_abort();
public:
    // 中断请求标志位
    int abort_request;
    // 播放序列，所谓播放序列就是一段连续的播放动作，一个seek操作会启动一段新的播放序列
    int serial;
private:
    // packet链表
    std::list<P_AVPacket *> AvPacketList;
    // 队列中packet的数量
    int nb_packets;
    // 队列所占内存空间大小
    int size;
    // 队列中所有packet总的播放时长
    int64_t duration;
    // 条件cond
    pthread_cond_t      *cond;
    // 锁
    pthread_mutex_t     *mutex;
};


NS_MEDIA_END
#endif // PacketQueue.h
