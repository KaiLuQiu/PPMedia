//
//  MediaPCMBuffer.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/23.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaPCMBuffer.h"
#include <stdlib.h>
NS_MEDIA_BEGIN
MediaPCMBuffer::MediaPCMBuffer():
rindex(0),
windex(0),
maxSize(MAX_PCMBUFFER_SIZE),
size(0)
{
    for (int i = 0; i < MAX_PCMBUFFER_SIZE; i++) {
        pcmSize[i] = 0;
        ptsArr[i] = 0.0;
        pcmData[i] = NULL;
    }
}

MediaPCMBuffer::~MediaPCMBuffer()
{
    for (int i = 0; i < MAX_PCMBUFFER_SIZE; i++) {
        pcmSize[i] = 0;
        ptsArr[i] = 0.0;
        if (pcmData[i]) {
            free(pcmData[i]);
            pcmData[i] = NULL;
        }
    }
    if (this->cond) {
        pthread_cond_destroy(this->cond);
        av_free(&this->cond);
    }
    if(this->mutex) {
        pthread_mutex_unlock(this->mutex);
        pthread_mutex_destroy(this->mutex);
        av_free(&this->mutex);
    }
}

/*
 * 初始化PCMBuffer
 */
int MediaPCMBuffer::init()
{
    int ret;
    // 创建互斥量
    if(NULL == this->mutex) {
        this->mutex = (pthread_mutex_t *)av_malloc(sizeof(pthread_mutex_t));
        if (NULL == this->mutex) {
            printf("FrameQueue init malloc mutex fail\n");
            return -1;
        }
    }
    // 初始化mutex
    ret = pthread_mutex_init(this->mutex, NULL);
    if (ret < 0) {
        av_free(&this->mutex);
        this->mutex = NULL;
        return ret;
    }
    // 创建条件变量
    if(NULL == this->cond) {
        this->cond = (pthread_cond_t *)av_malloc(sizeof(pthread_cond_t));
        if (NULL == this->cond) {
            printf("FrameQueue init malloc cond fail\n");
            return -1;
        }
    }
    ret = pthread_cond_init(this->cond, NULL);
    if (ret < 0) {
        av_free(&this->cond);
        this->cond = NULL;
        return ret;
    }
    return 0;
}

/*
 * 重置PCMBuffer
 */
void MediaPCMBuffer::release()
{
    for (int i = 0; i < MAX_PCMBUFFER_SIZE; i++) {
            pcmSize[i] = 0;
            ptsArr[i] = 0.0;
            if (pcmData[i]) {
                free(pcmData[i]);
                pcmData[i] = NULL;
            }
        }
    if (this->cond) {
        pthread_cond_destroy(this->cond);
        av_free(&this->cond);
    }
    if(this->mutex) {
        pthread_mutex_unlock(this->mutex);
        pthread_mutex_destroy(this->mutex);
        av_free(&this->mutex);
    }
}

/*
 * 写入一个data数据到当前的pcmBuffer中（如下的写法可能会影响性能，因为在临界区操作比较多）
 */
int MediaPCMBuffer::write(uint8_t* data, int64_t dataSize)
{
    pthread_mutex_lock(this->mutex);
    // 如果size大于良maxSize的话，那么丢列中已经写满了
    while (this->size >= this->maxSize) {
        pthread_cond_wait(this->cond, this->mutex);
    }
    // 获取循环队列的写索引位置
    int writeIndex = this->windex;
    int pcmBufferSize = pcmSize[writeIndex];
    // 如果当前的这个pcmBufferSize小于输入的dataSize大小则，从新分配空间malloc和realloc将会比较耗时，不应该放在锁内
    if (pcmBufferSize < dataSize) {
        if (0 == pcmBufferSize) {
            pcmData[writeIndex] = (uint8_t *)malloc(dataSize * sizeof(uint8_t));
            if (NULL == pcmData[writeIndex]) {
                printf("MediaPCMBuffer: malloc fail\n");
            }
        } else {
            pcmData[writeIndex] = (uint8_t *)realloc(pcmData[writeIndex], dataSize * sizeof(uint8_t));
            if (NULL == pcmData[writeIndex]) {
                printf("MediaPCMBuffer: malloc fail\n");
            }
        }
        // 更新pcmSize的大小
        pcmSize[writeIndex] = dataSize;
    }
    
    // cp数据到pcmBuffer队列中
    memcpy(pcmData[writeIndex], data, dataSize);
    // 队列中可读数据++
    this->size++;
    // 写索引+++,如果大于maxSize则为0
    if (++this->windex == this->maxSize) {
        this->windex = 0;
    }
    pthread_cond_signal(this->cond);
    pthread_mutex_unlock(this->mutex);
    return 0;
}

/*
 * 从PCMBuffer中读取数据
 */
int64_t MediaPCMBuffer::read(uint8_t* data, int64_t dataSize)
{
    int64_t readSize = 0;
    pthread_mutex_lock(this->mutex);
    // 如果size小于0的话,那么就没有可读取数据了，则等待
    while (this->size < 0) {
      pthread_cond_wait(this->cond, this->mutex);
    }
    // 获取循环队列的读索引位置
    int readIndex = this->rindex;
    if (NULL == pcmData[readIndex]) {
        pthread_mutex_unlock(this->mutex);
        return -1;
    }
    // 获取当前这笔数据的size
    readSize = pcmSize[readIndex];
    if (dataSize >= readSize) {
        memcpy(data, pcmData[readIndex], readSize);
    } else {
        // 如果dataSize < readSize,则只读取dataSize的数据量
        memcpy(data, pcmData[readIndex], dataSize);
        readSize = dataSize;
    }
    this->size--;
    //    (rindex + 1) % this->maxSize
    if (++this->rindex == this->maxSize) {
        this->rindex= 0;
    }
    pthread_cond_signal(this->cond);
    pthread_mutex_unlock(this->mutex);
    return readSize;
}

bool MediaPCMBuffer::isEmpty()
{
    if (this->size <= 0) {
        return true;
    }
    return false;
}

/*
 * 清空缓冲区
 */
int MediaPCMBuffer::flush()
{
    pthread_mutex_lock(this->mutex);
    for (int i = 0; i < MAX_PCMBUFFER_SIZE; i++) {
        pcmSize[i] = 0;
        ptsArr[i] = 0.0;
        pcmData[i] = NULL;
        memset(pcmData[i], 0, pcmSize[i]);
    }
    this->size = 0;
    this->rindex = 0;
    this->windex = 0;
    pthread_mutex_unlock(this->mutex);
    return 1;
}
NS_MEDIA_END

