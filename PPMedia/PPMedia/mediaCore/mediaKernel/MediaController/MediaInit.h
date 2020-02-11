//
//  MediaInit.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/02.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaInit_H
#define MediaInit_H
#include <string>
#include "MediaCommon.h"
NS_MEDIA_BEGIN

class MediaInit {
public:
    MediaInit();
    
    ~MediaInit();
    
    static void init();
    
    static void release();
    
    static bool Inited();
    
    static AVPacket* getFlushPacket();
private:
    static AVPacket         *flushPkt;

    static bool             isInited;

};

NS_MEDIA_END
#endif // FFmpegInit_H
