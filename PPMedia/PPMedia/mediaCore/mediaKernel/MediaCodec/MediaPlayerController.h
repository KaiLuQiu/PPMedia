//
//  MediaPlayerController.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/04.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaPlayerController_H
#define MediaPlayerController_H
#include <string>
#include "MediaCommon.h"
#include "MediaContext.h"
#include "MediaFrame.h"
#include "MediaStream.h"
#include "PPThread.h"
#include "MediaBaseController.h"
NS_MEDIA_BEGIN

#define Debug 0
class MediaPlayerController : public MediaBaseController
{
public:
    MediaPlayerController();
    
    virtual ~MediaPlayerController();
    
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
    
    /*
     * 获取媒体Frame
     */
    MediaFrame *getFrame();
    
    /*
     * 依据时间获取获取媒体Frame
     */
    MediaFrame *getFrame(int64_t syncTime);
private:
    /*
     * 打开流
     */
    int streamOpen();
    // 指针数组
    MediaStream*    mediaStream[MAX_DCODEC_STREAM_NUM];
    // 流索引数组
    int             stream_index[AVMEDIA_TYPE_NB];
    // PPThread是对线程创建的封装
    PPThread*       demuxerThread;

};

NS_MEDIA_END
#endif // MediaPlayerController_H
