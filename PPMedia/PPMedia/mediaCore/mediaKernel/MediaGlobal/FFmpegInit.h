//
//  FFmpegInit.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/02.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef FFmpegInit_H
#define FFmpegInit_H
#include <string>
#include "MediaCommon.h"
NS_MEDIA_BEGIN

#define Debug 0
class FFmpegInit {
public:
    FFmpegInit();
    
    ~FFmpegInit();
    
    static void init();
    
    static void release();
    
    static bool Inited();
    static AVPacket         *flushPkt;
private:
    static bool             isInited;

};

NS_MEDIA_END
#endif // FFmpegInit_H
