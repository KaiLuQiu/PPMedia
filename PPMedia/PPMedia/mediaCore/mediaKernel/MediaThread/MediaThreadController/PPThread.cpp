//
//  PPThread.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/02.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "PPThread.h"
#include <stdint.h>
#include <pthread.h>
NS_MEDIA_BEGIN

static void *RunThread(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;
    // 设置线程名字
    pthread_setname_np(info->Name.c_str());
    info->func(info->arg);
    return NULL;
}

PPThread::PPThread()
{
    pThreadInfo = NULL;
}

PPThread::~PPThread()
{
    SAFE_DELETE(pThreadInfo);
}

void PPThread::createThreadEx(ThreadFunc func, void *arg, std::string name)
{
    SAFE_DELETE(pThreadInfo);
    pThreadInfo = new ThreadInfo();
    pThreadInfo->func = func;
    pThreadInfo->arg = arg;
    pThreadInfo->Name = name;
    pThreadInfo->state = THREAD_STATE_INIT;
}

// 参考ijkplayer
int PPThread::setThreadPriority(ThreadPriority priority)
{
    struct sched_param sched;
    int policy;
    pthread_t thread = pthread_self();

    if (pthread_getschedparam(thread, &policy, &sched) < 0) {
        printf("pthread_getschedparam() failed");
        return -1;
    }
    if (priority == SDL_THREAD_PRIORITY_LOW) {
        sched.sched_priority = sched_get_priority_min(policy);
    } else if (priority == SDL_THREAD_PRIORITY_HIGH) {
        sched.sched_priority = sched_get_priority_max(policy);
    } else {
        int min_priority = sched_get_priority_min(policy);
        int max_priority = sched_get_priority_max(policy);
        sched.sched_priority = (min_priority + (max_priority - min_priority) / 2);
    }
    if (pthread_setschedparam(thread, policy, &sched) < 0) {
        printf("pthread_setschedparam() failed");
        return -1;
    }
    return 0;
}

int PPThread::start()
{
    assert(pThreadInfo);
    if (NULL == pThreadInfo) {
        return -1;
    }
    int ret = pthread_create(&pThreadInfo->pid, NULL, RunThread, pThreadInfo);
    pThreadInfo->state = THREAD_STATE_RUN;
    return ret;
}

int PPThread::join()
{
    assert(pThreadInfo);
    if (NULL == pThreadInfo) {
        return -1;
    }
    int ret = pthread_join(pThreadInfo->pid, NULL);
    pThreadInfo->state = THREAD_STATE_WAIT_EXIT;
    return ret;
}

int PPThread::detach()
{
    assert(pThreadInfo);
    if (NULL == pThreadInfo) {
        return -1;
    }
    int ret = pthread_detach(pThreadInfo->pid);
    return ret;
}

int PPThread::exit()
{
    assert(pThreadInfo);
    if (NULL == pThreadInfo) {
        return -1;
    }
    pThreadInfo->state = THREAD_STATE_EXIT;
    return 1;
}

NS_MEDIA_END


