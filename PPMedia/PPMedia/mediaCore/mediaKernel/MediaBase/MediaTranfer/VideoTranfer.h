//
//  VideoTranfer.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef VideoTranfer_H
#define VideoTranfer_H
#include <string>
#include "MediaCommon.h"
#include "TranferBase.h"

NS_MEDIA_BEGIN

typedef struct videoParam_T {
    videoParam_T()
    {
        pixelFormat = AV_PIX_FMT_NONE;
        width = -1;
        height = -1;
    }
    AVPixelFormat   pixelFormat;
    int             width;
    int             height;
} videoParam;

class VideoTranfer : public TranferBase
{
public:
    VideoTranfer();
    ~VideoTranfer();
    /*
     * 初始化
     */
    virtual int init();
    
    /*
     * 初始化
     */
    virtual int release();

    /*
     * 格式转换者
     */
    virtual int tranfer(AVFrame *inframe, AVFrame *outframe);
    
    /*
     * 设置输入输出的格式参数
     */
    void setVideoInfo(videoParam srcVideoParam, videoParam dstVideoParam);
private:
    // 视频图片转换上下文
    SwsContext          *swsContex;
    videoParam          srcVideoParam;
    videoParam          dstVideoParam;
};

NS_MEDIA_END
#endif // VideoTranfer_H
