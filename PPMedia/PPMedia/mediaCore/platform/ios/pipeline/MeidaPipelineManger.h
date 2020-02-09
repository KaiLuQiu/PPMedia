//
//  MeidaPipelineManger.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/09.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MeidaPipelineManger_H
#define MeidaPipelineManger_H

#include "MediaPipeline.h"
#include "MediaCommon.h"

NS_MEDIA_BEGIN

typedef enum
{
    HARDWARE,
    SOFTWARE,
}DecodeType;

// 外部判断解码方式（MeidaPipeline_ffplay 采用的是软件解）
class MeidaPipelineManger{
public:
    static Media_Pipeline *create_pipeline(DecodeType type);
    
    static Media_Pipenode *func_open_video_decoder(Media_Pipeline *pipeline);

    static SDL_Aout* func_open_audio_output(Media_Pipeline *pipeline);

};

NS_MEDIA_END
#endif
