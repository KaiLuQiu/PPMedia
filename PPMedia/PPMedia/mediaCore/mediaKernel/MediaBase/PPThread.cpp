//
//  PPThread.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/02.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "PPThread.h"
#include <pthread.h>
NS_MEDIA_BEGIN

static void *RunFunc(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;
    // 设置线程名字
    pthread_setname_np(info->Name.c_str());
    return info->func(info->arg);
}

PPThread::PPThread()
{
    pThreadInfo = NULL;
}

PPThread::~PPThread()
{
    SAFE_DELETE(pThreadInfo);
}

void PPThread::setFunc(ThreadFunc func, void *arg, std::string name)
{
    SAFE_DELETE(pThreadInfo);
    pThreadInfo = new ThreadInfo();
    pThreadInfo->func = func;
    pThreadInfo->arg = arg;
    pThreadInfo->Name = name;
    pThreadInfo->state = THREAD_STATE_INIT;
}

int PPThread::start()
{
    if (NULL == pThreadInfo) {
        return -1;
    }
    int ret = pthread_create(&pThreadInfo->pid, NULL, RunFunc, pThreadInfo);
    pThreadInfo->state = THREAD_STATE_RUN;
    return ret;
}

int PPThread::join()
{
    if (NULL == pThreadInfo) {
        return -1;
    }
    int ret = pthread_join(pThreadInfo->pid, NULL);
    pThreadInfo->state = THREAD_STATE_WAIT_EXIT;
    return ret;
}

int PPThread::detach()
{
    if (NULL == pThreadInfo) {
        return -1;
    }
    int ret = pthread_detach(pThreadInfo->pid);
    return ret;
}

NS_MEDIA_END

