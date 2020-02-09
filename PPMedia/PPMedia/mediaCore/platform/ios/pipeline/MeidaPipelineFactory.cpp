//
//  MeidaPipelineFactory.cpp
//  PPMedia
//  参考ijkplayer
//
//  Created by 邱开禄 on 2020/02/09.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MeidaPipelineFactory.h"
#include "MediaPipeNodeSoftVdec.h"
NS_MEDIA_BEGIN

/*
 * 创建对应的pipeNode（对应的每一个node其实会对应到对应的解码函数上，或者映射到对应的audio播放函数上）
 */
Media_Pipenode* MeidaPipelineFactory::func_get_soft_video_decoder(Media_Pipeline *pipeline)
{
    return MediaPipeNodeSoftVdec::pipenode_create_video_decoder_by_soft();
}

SDL_Aout* MeidaPipelineFactory::func_get_audio_output(Media_Pipeline *pipeline)
{
    return NULL;
}

Media_Pipeline* MeidaPipelineFactory::create_pipeline(NodeType type)
{
    Media_Pipeline *pipeline = NULL;
    switch (type) {
        case VIDEO_HARDWARE_NODE:{

        }
        break;
        case VIDEO_SOFTWARE_NODE:{
            pipeline = MediaPipeline::MediaPipeline_alloc();
            if (!pipeline) {
                break;
            }
            // 映射到对应函数指针上
            pipeline->func_get_video_decoder_node = MeidaPipelineFactory::func_get_soft_video_decoder;
            pipeline->func_get_audio_output_node = MeidaPipelineFactory::func_get_audio_output;
        }
        break;
        default:
            break;
    }

    return pipeline;
}

NS_MEDIA_END


