//
//  VideoConver.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef VideoConver_H
#define VideoConver_H
#include <string>
#include "MediaCommon.h"
#include "ConverBase.h"
#include "MediaParamInfo.h"

NS_MEDIA_BEGIN

class VideoConver : public ConverBase
{
public:
    VideoConver();
    ~VideoConver();
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
    virtual int Conver(AVFrame *inframe, AVFrame *outframe);
    
    /*
     * 设置输入输出的格式参数
     */
    void setVideoInfo(videoParamInfo srcVideoParam, videoParamInfo dstVideoParam);
private:
    // 视频图片转换上下文
    SwsContext              *swsContex;
    videoParamInfo          srcVideoParam;
    videoParamInfo          dstVideoParam;
};

NS_MEDIA_END
#endif // VideoTranfer_H
