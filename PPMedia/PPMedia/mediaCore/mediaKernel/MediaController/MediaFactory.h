//
//  MediaFactory.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/15.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaFactory_H
#define MediaFactory_H
#include <string>
#include "MediaCommon.h"
#include "MediaPlayerController.h"
#include "MediaEncoderController.h"

NS_MEDIA_BEGIN

enum PPMediaType{
    // 播放器
    PP_MEDIA_PLAYER,
    // 编码器
    PP_MEDIA_ECODER,
};

class MediaFactory {
public:
    MediaBaseController* CreateMediaController(PPMediaType type);
private:
};

NS_MEDIA_END
#endif // MediaFactory_H
