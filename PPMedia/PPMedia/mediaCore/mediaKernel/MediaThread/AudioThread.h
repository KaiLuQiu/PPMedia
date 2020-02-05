//
//  AudioThread.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/15.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef AudioThread_H
#define AudioThread_H
#include <string>
#include "MediaCommon.h"
NS_MEDIA_BEGIN

/*
 * audio输出线程
 */
void *AudioThread(void *arg);
NS_MEDIA_END
#endif // AudioThread_H
