//
//  PPThread.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/02.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef PPThread_H
#define PPThread_H
#include <string>
#include "MediaCommon.h"
NS_MEDIA_BEGIN

// 对线程的简单封装使用

typedef void *(*ThreadFunc)(void *);
enum THreadState {
    THREAD_STATE_INIT = 1,    //线程初始化状态
    THREAD_STATE_RUN,         //线程运行状态
    THREAD_STATE_SLEEP,       //线程休眠状态
    THREAD_STATE_WAIT_EXIT,   //线程等待退出状态
    THREAD_STATE_EXIT,        //线程已经退出
};

typedef struct ThreadInfo_T {
    ThreadInfo_T()
    {
        arg = NULL;
        func = NULL;
        Name = "";
        state = THREAD_STATE_INIT;
    }
    void                *arg;               // 线程传入参数
    ThreadFunc          func;               // 线程运行函数
    std::string         Name;               // 线程命名
    pthread_t           pid;                // 线程pid
    THreadState         state;              // 线程状态
}ThreadInfo;

class PPThread {
public:
    PPThread();
    
    ~PPThread();
    
    void setFunc(ThreadFunc func, void *arg, std::string name);
    
    int start();
    
    int join();
    
    int detach();
    
    int exit();
private:
    ThreadInfo          *pThreadInfo;        // 线程信息私有变量
};

NS_MEDIA_END
#endif // PPThread_H
