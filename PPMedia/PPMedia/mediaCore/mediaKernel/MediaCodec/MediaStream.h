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

NS_MEDIA_BEGIN

class MediaStream
{
public:
    MediaStream();
    
    ~MediaStream();
    
    void setStreamInfo(int streamIndex);
    /*
     * 获取当前流，初始化解码器
     */
    int initDecoder(MediaContext MediaCtx);
    
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
    int         curStreamIndex;
};

NS_MEDIA_END
#endif // MediaStream_H
