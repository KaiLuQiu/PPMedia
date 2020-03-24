//
//  PPlayerElement.h
//  PPlayerElement
//
//  Created by 邱开禄 on 2020/03/24.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#ifndef PPlayerElement_H
#define PPlayerElement_H
#include <string>
#include "MediaCommon.h"
#include "EventHandler.h"

NS_MEDIA_BEGIN

/* PPlayerElement 为播放器的最小单元。每一个PPlayerElement都能直接播放上层传递下来的Url
 * 可以封装为：输入url，启动播放后，输出相应的数据
 *  状态：待实现
 */
class PPlayerElement
{
public:
    /*
     * 设置url
     */
    void setDataSource(std::string url);
    
    /*
     * 执行播放器的prepareAsync状态，此过程会开启demuxer, audiodecode videodecode等线程。做好初始化工作。
     */
    bool prepareAsync();

    void prepare();
    
    /*
     * 当收到prepared之后，上层播放器可以设置start状态开始播放视频
     */
    bool start();
    
    /*
     * 进入暂停状态
     */
    bool pause(bool state);
    
    /*
     * 进行seek的过程
     */
    int seek(float pos);
    
    /*
     * 进行resume重新同步过程
     */
    bool resume();

    /*
     * stop播放器
     */
    bool stop();
    
    /*
     * flush当前的播放器，也就是吧demuxer audio video decode等数据都清空掉
     */
    void flush();
    
    /*
     * 设置是否循环播放视频
     */
    bool setLoop(bool loop);
    
    /*
     * 获取当前播放的位置信息
     */
    int64_t getCurPos();
    
    /*
     * 设置播放速度
     */
    bool setSpeed();

    /*
     * 获取当前的播放速度
     */
    float getSpeed();

    /*
     * 获取总时长
     */
    long getDuration();
    
    /*
     * 设置音量
     */
    void setVolume(float value);
    
    PPlayerElement();
    
    void pp_get_msg(Message& msg);
    
    /*
     * 获析构函数一定不能私有话，否则可能导致内存泄漏
     */
    virtual ~PPlayerElement();
private:
    std::string pUrl;
};

NS_MEDIA_END
#endif // PPlayerElement_H
