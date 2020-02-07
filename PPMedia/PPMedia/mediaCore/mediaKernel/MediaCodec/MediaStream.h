//
//  MediaStream.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaStream_H
#define MediaStream_H
#include <string>
#include "MediaCommon.h"
#include "MediaContext.h"
#include "MediaFrame.h"
#include "PPThread.h"
#include "FrameQueue.h"
#include <pthread.h>

NS_MEDIA_BEGIN
enum StreamType {
    PP_STREAM_NONE,
    PP_STREAM_VIDEO,
    PP_STREAM_AUDIO,
    PP_STREAM_SUBTITLE,
};

// 解码上下文的封装 参考ffplay
typedef struct MediaDecoderContext_T {
    MediaDecoderContext_T()
    {
        avctx = NULL;
        pkt_serial = -1;
        finished = 0;
        packet_pending = 0;
        start_pts = 0;
        next_pts = 0;
        // 创建条件变量
        if(NULL == this->empty_queue_cond) {
            this->empty_queue_cond = (pthread_cond_t *)av_malloc(sizeof(pthread_cond_t));
            if (NULL == this->empty_queue_cond) {
                printf("FrameQueue init malloc cond fail\n");
            }
        }
        pthread_cond_init(this->empty_queue_cond, NULL);
    }
    ~MediaDecoderContext_T()
    {
        if (this->empty_queue_cond) {
            pthread_cond_destroy(this->empty_queue_cond);
            av_free(&this->empty_queue_cond);
        }
    }
    AVCodecContext *avctx;
    AVPacket pkt;
    int pkt_serial;
    int finished;
    int packet_pending;
    int64_t start_pts;
    AVRational start_pts_tb;
    int64_t next_pts;
    AVRational next_pts_tb;
    pthread_cond_t *empty_queue_cond;
} MediaDecoderContext;

class MediaStream
{
public:
    MediaStream();
    
    ~MediaStream();
    
    /*
     * 获取当前流，初始化解码器
     */
    int initDecoder(MediaContext *MediaCtx, int streamIndex, StreamType type);

    /*
     * 获取当前流，启动解码器
     */
    int openDecoder();
    
    /*
     * 获取当前流，关闭解码器
     */
    int closeDecoder();
    
    /*
     * 暂停当前流
     */
    int pause();
    
    /*
     * resume当前流
     */
    int resume();
    
    /*
     * 获取当前帧
     */
    MediaFrame* getFrame();
public:
    // 媒体上下文
    MediaContext*           mediaContext;
    // 当前这个stream是的类型（audio / video）
    StreamType              streamType;
    // 当前流的索引
    int                     curStreamIndex;
    // 解码器上下文
    MediaDecoderContext*    codecContext;
    // 当前的流索引对应的流
    AVStream*               stream;
    // 多线程同步相关（控制decode线程）
    ThreadController*       decodeThreadController;
private:
    // 当前流的解码线程
    PPThread*               decodeThread;
    // 对应解码器
    AVCodec*                codec;
    // 解码后存放frame的队列
    FrameQueue*             frameQueue;
};

NS_MEDIA_END
#endif // MediaStream_H
