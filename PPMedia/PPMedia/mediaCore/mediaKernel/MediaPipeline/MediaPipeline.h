//
//  MediaPipeline.h
//  PPMedia
//  参考ijkplayer
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaPipeline_H
#define MediaPipeline_H
#include <string>
#include "MediaCommon.h"
#include "MediaPipelineNode.h"

NS_MEDIA_BEGIN
typedef struct SDL_Aout {
    
};

typedef struct Media_Pipeline Media_Pipeline;
struct Media_Pipeline {
    void                (*func_destroy)                     (Media_Pipeline *pipeline);
    Media_Pipenode      *(*func_get_video_decoder_node)     (Media_Pipeline *pipeline);
    SDL_Aout            *(*func_get_audio_output_node)      (Media_Pipeline *pipeline);
};

class MediaPipeline
{
public:
    static Media_Pipeline *MediaPipeline_alloc();
    
    static void MediaPipeline_free(Media_Pipeline *pipeline);
    
    static Media_Pipenode* get_video_decoder_node(Media_Pipeline *pipeline);

    static SDL_Aout* get_audio_output_node(Media_Pipeline *pipeline);
};

NS_MEDIA_END
#endif // MediaPipeline_H

