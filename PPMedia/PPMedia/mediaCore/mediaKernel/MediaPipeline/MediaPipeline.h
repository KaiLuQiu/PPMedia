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
    void                (*func_destroy)              (Media_Pipeline *pipeline);
    Media_Pipenode      *(*func_open_video_decoder)  (Media_Pipeline *pipeline);
    SDL_Aout            *(*func_open_audio_output)   (Media_Pipeline *pipeline);
    Media_Pipenode      *(*func_init_video_decoder)  (Media_Pipeline *pipeline);
    int                 (*func_config_video_decoder) (Media_Pipeline *pipeline);
};

class MediaPipeline
{
public:
    static Media_Pipeline *MediaPipeline_alloc();
    
    static void MediaPipeline_free(Media_Pipeline *pipeline);
    
    static Media_Pipenode* init_video_decoder(Media_Pipeline *pipeline);

    static Media_Pipenode* open_video_decoder(Media_Pipeline *pipeline);

    static SDL_Aout* open_audio_output(Media_Pipeline *pipeline);
    
    static int config_video_decoder(Media_Pipeline *pipeline);
};

NS_MEDIA_END
#endif // MediaPipeline_H

