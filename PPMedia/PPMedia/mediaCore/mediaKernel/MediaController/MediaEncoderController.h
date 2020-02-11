//
//  MediaEncoderController.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/04.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaEncoderController_H
#define MediaEncoderController_H
#include <string>
#include "MediaCommon.h"
#include "MediaContext.h"
#include "MediaFrame.h"
#include "MediaStream.h"
#include "MediaBaseController.h"

NS_MEDIA_BEGIN

class MediaEncoderController : public MediaBaseController
{
public:
    MediaEncoderController();
    
    virtual ~MediaEncoderController();
    
     /*
      * 设置媒体播放路径
      */
    void setSaveUrl(std::string savePath);
    
    /*
     * 初始化操作
     */
    virtual int init();
    
    /*
     * 进入准备状态
     */
    virtual int prepareAsync();

    /*
     * 开始
     */
    virtual int start();

    /*
     * 停止播放
     */
    virtual int stop();

    /*
     * seek
     */
    virtual int seek(float pos);

    /*
     * 暂停
     */
    virtual int pause();
private:
    // 媒体播放URL
    std::string                 savePath;
};

NS_MEDIA_END
#endif // MediaController_H
