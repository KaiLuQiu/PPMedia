//
//  MediaFrame.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/11.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef MediaFrame_H
#define MediaFrame_H
#include <string>
#include "MediaDefine.h"
#include "MediaParamInfo.h"
#include "AudioResSample.h"
#include "VideoConver.h"

NS_MEDIA_BEGIN

typedef enum {
    PP_FRAME_NONE,
    PP_FRAME_VIDEO,
    PP_FRAME_AUDIO,
}FrameType;

// 对解码后的音视频数据进行封装
class MediaFrame
{
public:
    MediaFrame();
    ~MediaFrame();
/************audio*************/
    /*
     * 设置输入输出的音频参数信息
     */
    void setSrcAudioParam(AudioParamInfo srcParam);
    
    void setDstAudioParam(AudioParamInfo srcParam);

/************video*************/
    /*
     * 设置输入输出的视频参数信息
     */
    void setVideoParam(videoParamInfo srcParam, videoParamInfo dstParam);


private:
    // 当前媒体的类型
    FrameType                   frameType;
    // 视频格式转换者
    VideoConver*                videoConver;
    // 音频重采样转换者
    AudioResSample*             audioConver;
    // 数据
    uint8_t*                    data;
    // 数据大小
    int64_t                     size;
    // 音视频输入输出参数
    AudioParamInfo              srcAudioParam;
    AudioParamInfo              dstAudioParam;
    videoParamInfo              srcVideoParam;
    videoParamInfo              dstVideoParam;
    //
};

NS_MEDIA_END
#endif // MediaFrame_H
