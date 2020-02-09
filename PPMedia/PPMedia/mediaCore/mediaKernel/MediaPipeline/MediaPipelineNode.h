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
#include <string>
#include "MediaCommon.h"

NS_MEDIA_BEGIN


typedef struct Media_Pipenode Media_Pipenode;
// 映射到对应解码函数指针
struct Media_Pipenode {
    SDL_Class                   *opaque_class;
    void (*func_destroy)        (void);
    int  (*func_execute)        (void *arg);
    int  (*func_flush)          (void); // optional
};

class MediaPipelineNode
{
public:
    static Media_Pipenode *pipenode_alloc(SDL_Class *opaque_class);
    
    static void pipenode_free(Media_Pipenode *node);
};

NS_MEDIA_END
#endif // MediaPipelineNode_H
