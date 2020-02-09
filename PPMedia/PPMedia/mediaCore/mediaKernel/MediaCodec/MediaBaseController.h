//
//  MediaBaseController.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/04.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaBaseController_H
#define MediaBaseController_H
#include <string>
#include "MediaCommon.h"
#include "MediaContext.h"
#include "MediaFrame.h"
#include "MediaStream.h"

NS_MEDIA_BEGIN

class MediaBaseController
{
public:
    MediaBaseController();
    
    virtual ~MediaBaseController();
    
    /*
     * 初始化操作
     */
    virtual int init() {return 0;}

    /*
     * 设置媒体播放路径
     */
    virtual void setUrl(std::string mediaPath) {
        this->mediaPath = mediaPath;
    }
    
    /*
     * 进入准备状态
     */
    virtual int prepareAsync() {return 0;}

    /*
     * 开始
     */
    virtual int start() {return 0;}

    /*
     * 停止播放
     */
    virtual int stop() {return 0;}

    /*
     * seek
     */
    virtual int seek(float pos) {return 0;}

    /*
     * 暂停
     */
    virtual int pause() {return 0;}
public:
    // 媒体播放上下文
    MediaContext                *mediaContext;
protected:
    // 媒体播放URL
    std::string                 mediaPath;
private:

};

NS_MEDIA_END
#endif // MediaController_H
