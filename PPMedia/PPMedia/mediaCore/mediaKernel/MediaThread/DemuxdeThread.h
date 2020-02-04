//
//  DemuxdeThread.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/15.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef DemuxdeThread_H
#define DemuxdeThread_H
#include <string>
#include "MediaCommon.h"
NS_MEDIA_BEGIN
/*
 * demuxer解封装线程
 */
void *DemuxerThread(void *arg);
NS_MEDIA_END
#endif // DemuxdeThread_H
