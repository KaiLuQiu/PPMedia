//
//  VideoTranfer.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/05.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaPipelineNode.h"
#include <string.h>
NS_MEDIA_BEGIN

Decode_Pipenode *MediaPipelineNode::PipeDecodeNode_alloc(SDL_Class *opaque_class)
{
    Decode_Pipenode *node = (Decode_Pipenode*) calloc(1, sizeof(Decode_Pipenode));
    node->opaque_class = opaque_class;
    if (!node)
        return NULL;
    return node;
}

void MediaPipelineNode::PipeDecodeNode_free(Decode_Pipenode *node)
{
    if (!node)
        return;

    if (node->func_destroy) {
        node->func_destroy();
    }
    
    memset(node, 0, sizeof(Decode_Pipenode));
    free(node);
}

Aout_Pipenode* MediaPipelineNode::PipeAudioOutNode_alloc(SDL_Class *opaque_class, size_t opaque_size)
{
    Aout_Pipenode *node = (Aout_Pipenode*) calloc(1, sizeof(Aout_Pipenode));
    node->opaque_class = opaque_class;
    
    if (opaque_size >= 0) {
        node->opaque = (SDL_Aout_Opaque *)malloc(opaque_size);
        memset(node->opaque, 0, opaque_size);
        if (!node->opaque) {
            free(node);
            return NULL;
        }
    } else {
        node->opaque = NULL;
    }
    
//    node->mutex = SDL_CreateMutex();
//    if (node->mutex == NULL) {
//        free(node->opaque);
//        free(node);
//        return NULL;
//    }
//    
    if (!node)
        return NULL;
    return node;
}

void MediaPipelineNode::PipeAudioOutNode_free(Aout_Pipenode *node)
{
    if (!node)
        return;

    if (node->free_l) {
        node->free_l(node);
    }

//    if (node->mutex) {
//        SDL_DestroyMutex(node->mutex);
//    }

    free(node->opaque);
    memset(node, 0, sizeof(Aout_Pipenode));
    free(node);
}

NS_MEDIA_END

