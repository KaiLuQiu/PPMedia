//
//  VideoConver.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/10.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "VideoConver.h"
NS_MEDIA_BEGIN

VideoConver::VideoConver():
swsContex(NULL)
{
    
}

VideoConver::~VideoConver()
{
    if (swsContex) {
        sws_freeContext(swsContex);
        swsContex = NULL;
    }
}

void VideoConver::setVideoInfo(videoParamInfo srcVideoParam, videoParamInfo dstVideoParam)
{
    this->srcVideoParam = srcVideoParam;
    this->dstVideoParam = dstVideoParam;
}

/*
 * 初始化
 */
int VideoConver::init()
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
int VideoConver::release()
{
    if (swsContex) {
        sws_freeContext(swsContex);
        swsContex = NULL;
    }
    return 1;
}

/*
 * 格式转换者
 */
int VideoConver::Conver(AVFrame *inframe, AVFrame *outframe)
{
    if (NULL == swsContex || NULL == inframe || NULL == outframe) {
        printf("VideoTranfer: frame or sws_ctx is NULL\n");
        return -1;
    }
    int height = sws_scale( swsContex,
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

/*
 * 格式转换者
 */
int VideoConver::Conver(uint8_t** outData, int outlineSize[], uint8_t** intData, int inlineSize[])
{
    if (NULL == swsContex || NULL == outData || NULL == intData
        || AV_PIX_FMT_NONE == dstVideoParam.pixelFormat
        || AV_PIX_FMT_NONE == srcVideoParam.pixelFormat) {
        printf("VideoTranfer: frame or sws_ctx is NULL\n");
        return -1;
    }
    
    int height = sws_scale( swsContex,
                            (const uint8_t * const*)intData,
                              inlineSize,
                              0,
                              srcVideoParam.height,
                              outData,
                              outlineSize
                            );
    if(height <= 0) {
        printf("VideoTranfer: sws_scale error\n");
        return -1;
    }
    return 1;
}



void VideoConver::fill_yuv_image(uint8_t *data[4], int linesize[4],
                           int width, int height, int frame_index)
{
    int x, y;

    /* Y */
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            data[0][y * linesize[0] + x] = x + y + frame_index * 3;

    /* Cb and Cr */
    for (y = 0; y < height / 2; y++) {
        for (x = 0; x < width / 2; x++) {
            data[1][y * linesize[1] + x] = 128 + y + frame_index * 2;
            data[2][y * linesize[2] + x] = 64 + x + frame_index * 5;
        }
    }
}

NS_MEDIA_END

