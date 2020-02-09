//
//  PacketQueue.cpp
//  PPlayer
//
//  Created by 邱开禄 on 2019/11/14.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#include "PacketQueue.h"
#include "MediaInit.h"
#include <pthread.h>

NS_MEDIA_BEGIN

PacketQueue::PacketQueue():
nb_packets(0),
size(0),
duration(0),
abort_request(0),
serial(0),
cond(NULL),
mutex(NULL)
{
    AvPacketList.clear();
}

PacketQueue::~PacketQueue()
{
    
}

int PacketQueue::packet_queue_init()
{
    int ret;
    if(NULL == mutex) {
          mutex = (pthread_mutex_t *)av_malloc(sizeof(pthread_mutex_t));
          if (NULL == mutex) {
              printf("FrameQueue init malloc mutex fail\n");
              return -1;
          }
      }
    // 初始化mutex
    ret = pthread_mutex_init(mutex, NULL);
    if (ret < 0) {
        av_free(&this->mutex);
        this->mutex = NULL;
        return ret;
    }
    // 创建条件变量
    if(NULL == cond) {
      cond = (pthread_cond_t *)av_malloc(sizeof(pthread_cond_t));
      if (NULL == cond) {
          printf("FrameQueue init malloc cond fail\n");
          return -1;
      }
    }
    ret = pthread_cond_init(cond, NULL);
    if (ret < 0) {
        av_free(&this->cond);
        this->cond = NULL;
        return ret;
    }
        
    this->abort_request = 1;
    return ret;
}

void PacketQueue::packet_queue_start()
{
    pthread_mutex_lock(this->mutex);
    this->abort_request = 0;
    packet_queue_put_private(MediaInit::getFlushPacket());
    pthread_mutex_unlock(this->mutex);
}

int PacketQueue::packet_queue_get(AVPacket *pkt, int block, int *serial)
{
    int ret;
    P_AVPacket  *p_pkt;
    pthread_mutex_lock(this->mutex);

    // 添加容错处理
    if(pkt == NULL)
    {
        pthread_mutex_unlock(this->mutex);
        return -1;
    }

    for(;;)
    {
        if (this->abort_request) {
            ret = -1;
            break;
        }
        
        p_pkt = this->AvPacketList.front();
        this->AvPacketList.pop_front();
        if(p_pkt){
            this->nb_packets--;
            this->size -= p_pkt->pkt.size + sizeof(*p_pkt);
            this->duration -= p_pkt->pkt.duration;
            *pkt = p_pkt->pkt;
            if(serial)
                *serial = p_pkt->serial;
            
            /*******************/
            av_free(p_pkt);
            /*******************/

            ret = 1;
            break;
        } else if(!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(this->cond, this->mutex);
        }
    }

    pthread_mutex_unlock(this->mutex);
    return ret;
}

int PacketQueue::packet_queue_put_private(AVPacket *pkt)
{
    // 如果已中止，则放入失败
    if (this->abort_request)
        return -1;
    
    this->AvPacketList;
    // 创建一个新的P_AVPacket节点
    P_AVPacket *p_pkt = new P_AVPacket();
    if(!p_pkt)
        return -1;
    p_pkt->pkt = *pkt;
    // 如果这个包是flushPkt，则让那个serial++,表示不连续
    if(pkt == MediaInit::getFlushPacket())
        this->serial++;
    // 使用同一个序列号
    p_pkt->serial = this->serial;
    // 把当前的这个P_AVPacket丢进队列中
    this->AvPacketList.push_back(p_pkt);
    // 添加节点数据
    this->nb_packets++;
    // 计算包的size大小以及p_pkt结构大小
    this->size += p_pkt->pkt.size + sizeof(*p_pkt);
    this->duration += p_pkt->pkt.duration;
    pthread_cond_signal(this->cond);
    return 0;
}

int PacketQueue::packet_queue_put(AVPacket *pkt)
{
    int ret;
    pthread_mutex_lock(this->mutex);
    // 主要实现在这里
    ret = packet_queue_put_private(pkt);
    pthread_mutex_unlock(this->mutex);
    if (pkt != MediaInit::getFlushPacket() && ret < 0) {
        // 放入失败，释放AVPacket
        av_packet_unref(pkt);
    }
    return ret;
    
}

int PacketQueue::packet_queue_put_nullpacket(int stream_index)
{
    AVPacket pkt1, *pkt = &pkt1;
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;
    pkt->stream_index = stream_index;
    return packet_queue_put(pkt);
}

void PacketQueue::packet_queue_flush()
{
    pthread_mutex_lock(this->mutex);

    std::list<P_AVPacket *>::iterator item = this->AvPacketList.begin();
    for(; item != this->AvPacketList.end(); )
    {
        std::list<P_AVPacket *>::iterator item_e = item++;
        // 先释放avpacket
        av_packet_unref(&(*item_e)->pkt);
        av_freep(&(*item_e));
        this->AvPacketList.erase(item_e);
    }
    this->AvPacketList.clear();
    this->nb_packets = 0;
    this->size = 0;
    this->duration = 0;
    pthread_mutex_unlock(this->mutex);
}

void PacketQueue::packet_queue_destroy()
{
    packet_queue_flush();
}

void PacketQueue::packet_queue_abort()
{
    pthread_mutex_lock(this->mutex);
    this->abort_request = 1;
    pthread_mutex_unlock(this->mutex);
    pthread_cond_signal(this->cond);
}

NS_MEDIA_END
