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

NS_MEDIA_BEGIN
enum StreamType {
    PP_STREAM_NONE,
    PP_STREAM_VIDEO,
    PP_STREAM_AUDIO,
    PP_STREAM_SUBTITLE,
};

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
    
private:
    // 当前流的索引
    int             curStreamIndex;
    // 当前流的解码线程
    PPThread*       decodeThread;
    // 当前的流索引对应的流
    AVStream*       stream;
    // 对应解码器
    AVCodec*        codec;
    // 媒体上下文
    MediaContext*   mediaContext;
    // 解码器上下文
    AVCodecContext* CodecContext;
    // 解码后存放frame的队列
    FrameQueue*     frameQueue;
    // 当前这个stream是的类型（audio / video）
    StreamType      streamType;
};

NS_MEDIA_END
#endif // MediaStream_H
