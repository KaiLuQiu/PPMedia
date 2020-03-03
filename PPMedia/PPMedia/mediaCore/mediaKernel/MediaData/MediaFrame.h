//
//  MediaFrame.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/29.
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
    
    void setMediaType(FrameType type);
/************audio*************/
    /*
     * 设置输入输出的音频参数信息
     */
    bool setSrcAudioParam(AudioParamInfo srcParam);
    
    bool setDstAudioParam(AudioParamInfo dstParam);

/************video*************/
    /*
     * 设置输入输出的视频参数信息
     */
    bool setSrcVideoParam(videoParamInfo srcParam);

    bool setDstVideoParam(videoParamInfo dstParam);

    int writeFrameData(uint8_t* data, int64_t dataSize);
    
    int writeFrame(AVFrame* frame);
    
    uint8_t* readFrameData(int64_t& dataSize);
    
    bool Conver();

private:
    // 当前媒体的类型
    FrameType                   frameType;
    // 视频格式转换者
    VideoConver*                videoConver;
    // 音频重采样转换者
    AudioResSample*             audioConver;
    /************************Frame**************************/
    // srcFrame
    AVFrame*                    srcFrame;
    // srcFrame
    AVFrame*                    dstFrame;
    /**************************Data**************************/
    // 数据
    uint8_t*                    srcData;
    // 数据
    uint8_t*                    dstData;
    // 数据大小
    int64_t                     srcSize;
    // 数据大小
    int64_t                     dstSize;
    // 数据大小
    int64_t                     outSize;
    // 音视频输入输出参数
    AudioParamInfo              srcAudioParam;
    AudioParamInfo              dstAudioParam;
    videoParamInfo              srcVideoParam;
    videoParamInfo              dstVideoParam;
    
    bool                        isInitSrcVideoParam;
    bool                        isInitSrcAudioParam;
    
    bool                        isInitDstVideoParam;
    bool                        isInitDstAudioParam;
    
    AVAudioFifo*                AudioFIFO;
};

NS_MEDIA_END
#endif // MediaFrame_H
