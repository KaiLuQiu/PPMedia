//
//  ConverBase.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/10.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef ConverBase_H
#define ConverBase_H
#include <string>
#include "MediaCommon.h"

NS_MEDIA_BEGIN
// 音频重采样&视频格式转换的基类
class ConverBase
{
public:
    ConverBase();
    virtual ~ConverBase();
    
    /*
     * 初始化
     */
    virtual int init() {return 0;}
    
    /*
     * 初始化
     */
    virtual int release() {return 0;}

    /*
     * 格式转换者
     */
    virtual int Conver(AVFrame *inframe, AVFrame *outframe)  {return 0;}

private:
};

NS_MEDIA_END
#endif // TranferBase_H
