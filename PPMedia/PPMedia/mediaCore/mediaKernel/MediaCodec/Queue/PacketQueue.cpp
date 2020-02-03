//
//  PacketQueue.cpp
//  PPlayer
//
//  Created by 邱开禄 on 2019/11/14.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#include "PacketQueue.h"
#include "FFmpegInit.h"
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
      SAFE_AV_FREE(mutex);
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
      SAFE_AV_FREE(cond);
      return ret;
    }
        
    this->abort_request = 1;
    return ret;
}

void PacketQueue::packet_queue_start()
{
    pthread_mutex_lock(this->mutex);
    this->abort_request = 0;
    packet_queue_put_private(FFmpegInit::flushPkt);
    pthread_mutex_unlock(this->mutex);
}

int PacketQueue::packet_queue_get(AVPacket *pkt, int block, int *serial)
{
    int ret;
    P_AVPacket  *p_pkt;
    pthread_mutex_lock(this->mutex);

    if(pkt == NULL)        //添加容错处理
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
    if (this->abort_request)//如果已中止，则放入失败
        return -1;
    
    this->AvPacketList;
    P_AVPacket *p_pkt = new P_AVPacket();       //创建一个新的P_AVPacket节点
    if(!p_pkt)
        return -1;
    p_pkt->pkt = *pkt;
    if(pkt == FFmpegInit::flushPkt)                        //
        this->serial++;
    p_pkt->serial = this->serial;              //使用同一个序列号
    
    this->AvPacketList.push_back(p_pkt);       //把当前的这个P_AVPacket丢进队列中
    this->nb_packets++;                        //添加节点数据
    this->size += p_pkt->pkt.size + sizeof(*p_pkt);  // 计算包的size大小以及p_pkt结构大小
    this->duration += p_pkt->pkt.duration;
    
    pthread_cond_signal(this->cond);
    return 0;
}

int PacketQueue::packet_queue_put(AVPacket *pkt)
{
    int ret;
    pthread_mutex_lock(this->mutex);
    ret = packet_queue_put_private(pkt);//主要实现在这里
    pthread_mutex_unlock(this->mutex);
    if (pkt != FFmpegInit::flushPkt && ret < 0)
        av_packet_unref(pkt);//放入失败，释放AVPacket
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
        av_packet_unref(&(*item_e)->pkt);           //先释放avpacket
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
