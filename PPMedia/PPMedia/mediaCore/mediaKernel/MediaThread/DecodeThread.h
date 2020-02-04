//
//  DecodeThread.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/02.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef DecodeThread_H
#define DecodeThread_H
#include <string>
#include "MediaCommon.h"
NS_MEDIA_BEGIN

/*
 * demuxer解封装线程
 */
void *DecoderThread(void *arg);
NS_MEDIA_END
#endif // DecodeThread_H
