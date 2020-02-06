//
//  ThreadController.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/08.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef ThreadController_H
#define ThreadController_H
#include <string>
#include "MediaCommon.h"
#include <pthread.h>
NS_MEDIA_BEGIN
// 多线程同步控制器

class ThreadController {
public:
    ThreadController();
    
    ~ThreadController();
    
    int init();
    
    void release();
    
    void wait();
    
    void singal();
private:
    // 条件变量
    pthread_cond_t*    cond;
    // 互斥量
    pthread_mutex_t*   mutex;
};

NS_MEDIA_END
#endif // ThreadController_H
