//
//  VideoTranfer.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/05.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaPipelineNode.h"
NS_MEDIA_BEGIN

Media_Pipenode *MediaPipelineNode::pipenode_alloc(SDL_Class *opaque_class)
{
    Media_Pipenode *node = (Media_Pipenode*) calloc(1, sizeof(Media_Pipenode));
    node->opaque_class = opaque_class;
    if (!node)
        return NULL;
    return node;
}

void MediaPipelineNode::pipenode_free(Media_Pipenode *node)
{
    if (!node)
        return;

    if (node->func_destroy) {
        node->func_destroy();
    }
    
    memset(node, 0, sizeof(Media_Pipenode));
    free(node);
}

NS_MEDIA_END

