//
//  MediaPipeNodeSoftVdec.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/09.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaPipeNodeSoftVdec.h"
#include "MediaPipelineNode.h"
#include "DecodeThread.h"
NS_MEDIA_BEGIN

static SDL_Class g_pipeline_class = {
    .name = "MediaPipelineManger",
};

// 以下部分是函数方法的映射
Media_Pipenode* MediaPipeNodeSoftVdec::pipenode_create_video_decoder_by_soft()
{
    Media_Pipenode *node = MediaPipelineNode::pipenode_alloc(&g_pipeline_class);
    if (!node)
        return node;

    node->func_destroy  = NULL;
    // 映射对应的软件解码函数
    node->func_video_decode = DecoderThreadByffplay;
    node->func_audio_out = NULL;
    return node;
}
NS_MEDIA_END
