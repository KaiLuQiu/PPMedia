//
//  ThreadController.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/08.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "ThreadController.h"
#include <pthread.h>
NS_MEDIA_BEGIN

ThreadController::ThreadController():
mutex(NULL),
cond(NULL)
{
  
    
}
    
ThreadController::~ThreadController()
{
    if (this->cond) {
        pthread_cond_destroy(this->cond);
        av_free(&this->cond);
    }
    if(this->mutex) {
        pthread_mutex_unlock(this->mutex);
        pthread_mutex_destroy(this->mutex);
        av_free(&this->mutex);
    }
}
    
int ThreadController::init()
{
    int ret;
    // 创建互斥量
      if(NULL == this->mutex) {
          this->mutex = (pthread_mutex_t *)av_malloc(sizeof(pthread_mutex_t));
          if (NULL == this->mutex) {
              printf("FrameQueue init malloc mutex fail\n");
              return -1;
          }
      }
      // 初始化mutex
      ret = pthread_mutex_init(this->mutex, NULL);
      if (ret < 0) {
          av_free(&this->mutex);
          this->mutex = NULL;
          return ret;
      }
      // 创建条件变量
      if(NULL == this->cond) {
          this->cond = (pthread_cond_t *)av_malloc(sizeof(pthread_cond_t));
          if (NULL == this->cond) {
              printf("FrameQueue init malloc cond fail\n");
              return -1;
          }
      }
      ret = pthread_cond_init(this->cond, NULL);
      if (ret < 0) {
          av_free(&this->cond);
          this->cond = NULL;
          return ret;
      }
    return ret;
}

void ThreadController::release()
{
    if (this->cond) {
        pthread_cond_destroy(this->cond);
        av_free(&this->cond);
    }
    if(this->mutex) {
        pthread_mutex_unlock(this->mutex);
        pthread_mutex_destroy(this->mutex);
        av_free(&this->mutex);
    }
}

void ThreadController::wait()
{
    if (NULL == this->mutex || NULL == this->cond) {
        printf("mutex or cond is NULL\n");
        return ;
    }
    pthread_mutex_lock(this->mutex);
    // 释放这个信号量
    pthread_cond_wait(this->cond, this->mutex);
    pthread_mutex_unlock(this->mutex);
}
    
void ThreadController::singal()
{
    if (NULL == this->mutex || NULL == this->cond) {
        printf("mutex or cond is NULL\n");
        return ;
    }
    pthread_mutex_lock(this->mutex);
    // 释放这个信号量
    pthread_cond_signal(this->cond);
    pthread_mutex_unlock(this->mutex);
}



NS_MEDIA_END

