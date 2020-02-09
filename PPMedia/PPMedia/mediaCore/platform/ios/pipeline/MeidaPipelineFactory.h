//
//  MeidaPipelineFactory.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/09.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MeidaPipelineFactory_H
#define MeidaPipelineFactory_H

#include "MediaPipeline.h"
#include "MediaCommon.h"

NS_MEDIA_BEGIN

typedef enum
{
    VIDEO_HARDWARE_NODE,
    VIDEO_SOFTWARE_NODE,
    // ...()
}NodeType;

// 外部判断解码方式（MeidaPipeline_ffplay 采用的是软件解）
class MeidaPipelineFactory{
public:
    static Media_Pipeline *create_pipeline(NodeType type);
private:
    static Media_Pipenode *func_get_soft_video_decoder(Media_Pipeline *pipeline);

    static SDL_Aout* func_get_audio_output(Media_Pipeline *pipeline);

};

NS_MEDIA_END
#endif
