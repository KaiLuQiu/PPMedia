//
//  MediaPipelineNode.h
//  PPMedia
//  参考ijkplayer
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaPipelineNode_H
#define MediaPipelineNode_H
#include <stdlib.h>
#include "MediaDefine.h"
#include "NodeDefine.h"

NS_MEDIA_BEGIN
class MediaPipelineNode
{
public:
    static Decode_Pipenode *PipeDecodeNode_alloc(SDL_Class *opaque_class);
    static void PipeDecodeNode_free(Decode_Pipenode *node);

    static Aout_Pipenode *PipeAudioOutNode_alloc(SDL_Class *opaque_class, size_t opaque_size);
    static void PipeAudioOutNode_free(Aout_Pipenode *node);

};

NS_MEDIA_END
#endif // MediaPipelineNode_H
