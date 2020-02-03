//
//  FrameQueue.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "FrameQueue.h"
#include <pthread.h>

NS_MEDIA_BEGIN

//取消帧引用的所有缓冲区并重置帧字段，释放给定字幕结构中的所有已分配数据。
//frame_queue_unref_item释放的内存都是关联的内存，而非结构体自身内存。
//AVFrame内部有许多的AVBufferRef类型字段，而AVBufferRef只是AVBuffer的引用，AVBuffer通过引用计数自动管理内存（简易垃圾回收机制）。因此AVFrame在不需要的时候，需要通过av_frame_unref减少引用计数。

FrameQueue::FrameQueue()
{
    // 帧队列
    queue = NULL;
    // 读索引
    rindex = 0;
    // 写索引
    windex = 0;
    // 总帧数
    size = 0;
    // 最大缓存大小
    max_size = 0;
    // keep_last是一个bool值，表示是否在环形缓冲区的读写过程中保留最后一个读节点不被覆写
    keep_last = 0;
    // 当前显示这一帧的读索引
    rindex_shown = 0;
    // 条件cond
    cond = NULL;
    // 锁
    mutex = NULL;
    // packet Queue指针
    pktq = NULL;
}

FrameQueue::~FrameQueue()
{
    
}

void FrameQueue::frame_queue_unref_item(Frame *vp)
{
    av_frame_unref(vp->frame);      // 释放帧引用
    avsubtitle_free(&vp->sub);      // 释放sub的结构
}

// Framequeue的初始化
int FrameQueue::frame_queue_init(PacketQueue *pktq, int max_size, int keep_last)
{
    int ret;
    // 创建互斥量
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
    this->pktq = pktq;
    this->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
    this->keep_last = !!keep_last;
    
    this->queue = (Frame *)av_malloc(this->max_size * sizeof(Frame));
    if (NULL == this->queue) {
        printf("FrameQueue queue malloc fail\n");
        SAFE_AV_FREE(mutex);
        SAFE_AV_FREE(cond);
        return -1;
    }
    for (int i = 0; i < this->max_size; i++)
        if (!(this->queue[i].frame = av_frame_alloc()))
            return AVERROR(ENOMEM);
    return 0;
}

// 释放Frame，释放互斥锁和互斥量
void FrameQueue::frame_queue_destory()
{
    int i;
    for (i = 0; i < this->max_size; i++) {
        Frame *vp = &this->queue[i];
        frame_queue_unref_item(vp);
        av_frame_free(&vp->frame);      // 释放AVFrame.
    }
    SDL_DestroyCond(this->cond);
    SDL_DestroyMutex(this->mutex);
}

// 获取待显示的第一个帧 ,rindex 表示当前的显示的帧，rindex_shown表示当前带显示的是哪一帧
Frame *FrameQueue::frame_queue_peek()
{
    return &this->queue[(this->rindex + this->rindex_shown) % this->max_size];
}

// 获取待显示的第二个帧
Frame *FrameQueue::frame_queue_peek_next()
{
    return &this->queue[(this->rindex + this->rindex_shown + 1) % this->max_size];
}

// 获取当前播放器显示的帧
Frame *FrameQueue::frame_queue_peek_last()
{
    return &this->queue[this->rindex];
}

// 获取queue中一块Frame大小的可写内存
// 加锁情况下，等待直到队列有空余空间可写（f->size < f->max_size）
// 如果有退出请求（f->pktq->abort_request），则返回NULL
// 返回windex位置的元素（windex指向当前应写位置）
Frame *FrameQueue::frame_queue_peek_writable()
{
    pthread_mutex_lock(this->mutex);
    while (this->size >= this->max_size &&
           !this->pktq->abort_request) {
        pthread_cond_wait(this->cond, this->mutex);
    }
    pthread_mutex_unlock(this->mutex);
    if (this->pktq->abort_request)
        return NULL;
    
    return &this->queue[this->windex];
}

// 这方法和frame_queue_peek的作用一样， 都是获取待显示的第一帧
// 加锁情况下，判断是否有可读节点（f->size - f->rindex_shown > 0)
// 如果有退出请求，则返回NULL
// 读取当前可读节点(f->rindex + f->rindex_shown) % f->max_size
// rindex_shown的意思是rindex指向的节点是否被读过
Frame *FrameQueue::frame_queue_peek_readable()
{
    pthread_mutex_lock(this->mutex);
    while (this->size - this->rindex_shown <= 0 &&
           !this->pktq->abort_request) {
        pthread_cond_wait(this->cond, this->mutex);
    }
    pthread_mutex_unlock(this->mutex);

    if (this->pktq->abort_request)
        return NULL;
    
    return &this->queue[(this->rindex + this->rindex_shown) % this->max_size];
}

// 推入一帧数据， 其实数据已经在调用这个方法前填充进去了， 这个方法的作用是将队列的写索引(也就是队尾)向后移， 还有将这个队列中的Frame的数量加一。
// windex加1，如果超过max_size，则回环为0
// 加锁情况下大小加1.
void FrameQueue::frame_queue_push()
{
    if (++this->windex == this->max_size)  // 这边只要保证windex之前都都是被写过的
        this->windex = 0;
    pthread_mutex_lock(this->mutex);
    this->size++;
    pthread_cond_singal(this->cond);
    pthread_mutex_unlock(this->mutex);
}

// 将读索引(队头)后移一位， 还有将这个队列中的Frame的数量减一
void FrameQueue::frame_queue_next()
{
    if (this->keep_last && !this->rindex_shown) {
        this->rindex_shown = 1;
        return;
    }
    frame_queue_unref_item(&this->queue[f->rindex]);
    if (++this->rindex == this->max_size)
        this->rindex = 0;
    pthread_mutex_lock(this->mutex);
    this->size--;
    pthread_cond_singal(this->cond);
    pthread_mutex_unlock(this->mutex);
}

// 返回队列中待显示帧的数目
int FrameQueue::frame_queue_nb_remaining()
{
    return this->size - this->rindex_shown;
}

// 返回正在显示的帧的position
int64_t FrameQueue::frame_queue_last_pos()
{
    Frame *fp = &this->queue[this->rindex];
    if (this->rindex_shown && fp->serial == this->pktq->serial)
        return fp->pos;
    else
        return -1;
}

NS_MEDIA_END
