//
//  MediaPipeNodeSoftVdec.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/09.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaPipeNodeSoftVdec_H
#define MediaPipeNodeSoftVdec_H

#include "MediaPipelineNode.h"
#include "MediaCommon.h"
NS_MEDIA_BEGIN

class MediaPipeNodeSoftVdec {
public:
    static Decode_Pipenode *pipenode_create_video_decoder_by_soft();
private:
};

NS_MEDIA_END
#endif
