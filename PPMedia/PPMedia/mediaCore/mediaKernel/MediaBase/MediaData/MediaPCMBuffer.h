//
//  MediaPCMBuffer.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/23.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaPCMBuffer_H
#define MediaPCMBuffer_H
#include <string>
#include "MediaCommon.h"
#include <pthread.h>

NS_MEDIA_BEGIN

// 初步定义循环队列的大小为18帧
#define MAX_PCMBUFFER_SIZE 18

// 对解码后的pcm数据进行封装
class MediaPCMBuffer
{
public:
    MediaPCMBuffer();
    
    ~MediaPCMBuffer();
    
    /*
     * 初始化PCMBuffer
     */
    int init();
    
    /*
     * 重置PCMBuffer
     */
    void release();
    
    /*
     * 写入一个data数据到当前的pcmBuffer中
     */
    int write(uint8_t* data, int64_t dataSize);

    /*
     * 从PCMBuffer中读取数据
     */
    int64_t read(uint8_t* data, int64_t size);
    
    /*
     * 判断队列可读数据是否为空
     */
    bool isEmpty();
    
    /*
     * 清空缓冲区
     */
    int flush();
private:
    // pcmData数组指针
    uint8_t*            pcmData[MAX_PCMBUFFER_SIZE];
    // 当前这个帧pcmBuffer的size
    int64_t             pcmSize[MAX_PCMBUFFER_SIZE];
    // 当前这个帧pts
    double              ptsArr[MAX_PCMBUFFER_SIZE];
    // 读索引
    int                 rindex;
    // 写索引
    int                 windex;
    // 最大的pcmBufferSize
    int                 maxSize;
    // 当前的含有pcmbuffer的size
    int                 size;
    // 条件cond
    pthread_cond_t      *cond;
    // 锁
    pthread_mutex_t     *mutex;
};

NS_MEDIA_END
#endif // MediaPCMBuffer_H
