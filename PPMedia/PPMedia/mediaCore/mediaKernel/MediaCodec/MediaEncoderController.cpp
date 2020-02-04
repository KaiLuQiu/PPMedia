//
//  MediaEncoderController.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/04.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaEncoderController.h"
NS_MEDIA_BEGIN

MediaEncoderController::MediaEncoderController()
{
    
}

MediaEncoderController::~MediaEncoderController()
{
    
}

void MediaEncoderController::setSaveUrl(std::string savePath)
{
    this->savePath = savePath;
}

int MediaEncoderController::prepareAsync()
{
    return 0;
}

int MediaEncoderController::start()
{
    return 0;
}

int MediaEncoderController::stop()
{
    return 0;
}

int MediaEncoderController::seek(float pos)
{
    return 0;
}

int MediaEncoderController::pause()
{
    return 0;
}
NS_MEDIA_END


