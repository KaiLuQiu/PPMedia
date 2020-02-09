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
#include "ThreadController.h"
#include "MediaPipeline.h"
#include "MediaPipelineNode.h"
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

    // 指针数组
    MediaStream*                mediaStream[MAX_DCODEC_STREAM_NUM];
    // 管道，上层创建不同平台的pipleline，通过它可以获取不同平台的解码器 音频输出器等node
    Media_Pipeline*             pipleLine;
private:
    /*
     * 打开流
     */
    int streamOpen();
    // 流索引数组
    int                         stream_index[AVMEDIA_TYPE_NB];
    // PPThread是对线程创建的封装
    PPThread*                   demuxerThread;
    // 多线程同步相关（控制demuxer线程）
    ThreadController*           demuxerThreadController;
    /*********************************************************软解硬件解相关*********************************************************/
    // 对应的解码器node可以映射到软件解码或者对应平台的硬件解码
    Media_Pipenode*             decodec_node;
    Media_Pipenode*             aout_node;
};

NS_MEDIA_END
#endif // MediaPlayerController_H
