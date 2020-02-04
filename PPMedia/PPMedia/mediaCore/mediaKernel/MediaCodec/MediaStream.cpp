//
//  MediaStream.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaStream.h"
NS_MEDIA_BEGIN

MediaStream::MediaStream()
{
    
}
  
MediaStream::~MediaStream()
{
    
}
  
void MediaStream::setStreamInfo(int streamIndex)
{
    // 设置当前的流索引信息
    this->curStreamIndex = streamIndex;
}

int MediaStream::initDecoder(MediaContext MediaCtx)
{
    int ret = 0;
    AVFormatContext *formatCtx = NULL;
    
    return ret;
}
  
int MediaStream::openDecoder()
{
    int ret = 0;
    return ret;
}
  
int MediaStream::closeDecoder()
{
    int ret = 0;
    return ret;
}
  
int MediaStream::pause()
{
    int ret = 0;
    return ret;
}
  
int MediaStream::resume()
{
    int ret = 0;
    return ret;
}
  
MediaFrame* MediaStream::getFrame()
{
    return NULL;
}

NS_MEDIA_END


