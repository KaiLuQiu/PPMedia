//
//  MediaFactory.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/15.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaFactory.h"
NS_MEDIA_BEGIN

MediaBaseController* MediaFactory::CreateMediaController(PPMediaType type)
{
    MediaBaseController *mediaController = NULL;
    switch (type) {
        // 播放器
        case PP_MEDIA_PLAYER:
            mediaController = new (std::nothrow)MediaPlayerController();
            if(NULL == mediaController) {
                printf("MediaFactory: new MediaPlayerController fail\n");
            }
            break;
        // 编码器
        case PP_MEDIA_ECODER:
            mediaController = new MediaEncoderController();
            if(NULL == mediaController) {
                printf("MediaFactory: new MediaEncoderController fail\n");
            }
            break;
        default:
            printf("MediaFactory: no this type\n");
            break;
    }
    return mediaController;
}

NS_MEDIA_END

