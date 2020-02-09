//
//  MediaPipeline.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/05.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaPipeline.h"
NS_MEDIA_BEGIN

Media_Pipeline *MediaPipeline::MediaPipeline_alloc()
{
    Media_Pipeline *pipeline = (Media_Pipeline*) calloc(1, sizeof(Media_Pipeline));
    return pipeline;
}

void MediaPipeline::MediaPipeline_free(Media_Pipeline *pipeline)
{
    if (!pipeline)
        return;

    if (pipeline->func_destroy) {
        pipeline->func_destroy(pipeline);
    }

    memset(pipeline, 0, sizeof(Media_Pipeline));
    free(pipeline);
}

Media_Pipenode* MediaPipeline::get_video_decoder_node(Media_Pipeline *pipeline)
{
    return pipeline->func_get_video_decoder_node(pipeline);
}

SDL_Aout* MediaPipeline::get_audio_output_node(Media_Pipeline *pipeline)
{
    return pipeline->func_get_audio_output_node(pipeline);
}

NS_MEDIA_END

