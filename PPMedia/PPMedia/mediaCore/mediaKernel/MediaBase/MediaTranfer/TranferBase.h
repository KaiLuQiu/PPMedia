//
//  TranferBase.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/05.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef TranferBase_H
#define TranferBase_H
#include <string>
#include "MediaCommon.h"

NS_MEDIA_BEGIN
// 音频重采样&视频格式转换的基类
class TranferBase
{
public:
    TranferBase();
    virtual ~TranferBase();
    
    /*
     * 初始化
     */
    virtual int init() {return 0;}
    
    /*
     * 初始化
     */
    virtual void release() {return 0;}

    /*
     * 格式转换者
     */
    virtual int tranfer(AVFrame *inframe, AVFrame *outframe)  {return 0;}

private:
};

NS_MEDIA_END
#endif // TranferBase_H
