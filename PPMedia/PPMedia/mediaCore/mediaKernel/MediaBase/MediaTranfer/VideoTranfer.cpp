//
//  VideoTranfer.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/05.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "VideoTranfer.h"
NS_MEDIA_BEGIN

VideoTranfer::VideoTranfer():
swsContex(NULL)
{
    
}

VideoTranfer::~VideoTranfer()
{
    if (swsContex) {
        sws_freeContext(swsContex);
        swsContex = NULL;
    }
}

void VideoTranfer::setVideoInfo(videoParam srcVideoParam, videoParam dstVideoParam)
{
    this->srcVideoParam = srcVideoParam;
    this->dstVideoParam = dstVideoParam;
}

/*
 * 初始化
 */
int VideoTranfer::init()
{
    if (swsContex) {
        sws_freeContext(swsContex);
        swsContex = NULL;
    }

    swsContex = sws_getContext(srcVideoParam.width, srcVideoParam.height, srcVideoParam.pixelFormat, dstVideoParam.width, dstVideoParam.height, dstVideoParam.pixelFormat, SWS_BICUBIC, NULL, NULL, NULL);
    
    if (!swsContex) {
        printf("VideoTranfer: sws_getCachedContext error\n");
        return -1;
    }
    return 1;
}

/*
 * 初始化
 */
void VideoTranfer::release()
{
    if (swsContex) {
        sws_freeContext(swsContex);
        swsContex = NULL;
    }
}

/*
 * 格式转换者
 */
int VideoTranfer::tranfer(AVFrame *inframe, AVFrame *outframe)
{
    if (NULL == swsContex || NULL == inframe || NULL == outframe) {
        printf("VideoTranfer: frame or sws_ctx is NULL\n");
        return -1;
    }
    int height = sws_scale(swsContex,
                              (const uint8_t **)inframe->data,
                              inframe->linesize,
                              0,
                              inframe->height,
                              outframe->data,
                              outframe->linesize
                            );
    if(height <= 0) {
        printf("VideoTranfer: sws_scale error\n");
        return -1;
    }
    return 1;
}

NS_MEDIA_END

