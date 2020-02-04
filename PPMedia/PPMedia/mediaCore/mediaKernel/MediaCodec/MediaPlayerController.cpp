//
//  MediaPlayerController.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/04.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaPlayerController.h"
NS_MEDIA_BEGIN

MediaPlayerController::MediaPlayerController()
{
    
}

MediaPlayerController::~MediaPlayerController()
{
    
}

/*
 * 进入准备状态
 */
int MediaPlayerController::prepareAsync()
{
    return 0;
}

/*
 * 开始
 */
int MediaPlayerController::start()
{
    return 0;
}

/*
 * 停止播放
 */
int MediaPlayerController::stop()
{
    return 0;
}

/*
 * seek
 */
int MediaPlayerController::seek(float pos)
{
    return 0;
}

/*
 * 暂停
 */
int MediaPlayerController::pause()
{
    return 0;
}

/*
 * 获取媒体Frame
 */
MediaFrame *getFrame()
{
    return NULL;
}

/*
 * 依据时间获取获取媒体Frame
 */
MediaFrame *getFrame(int64_t syncTime)
{
    return NULL;
}

NS_MEDIA_END


