//
//  MediaCommon.h
//  PPlayer
//
//  Created by 邱开禄 on 2019/11/14.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#ifndef MediaCommon_H
#define MediaCommon_H

#include <list>
#include <vector>
#include <string.h>
#include <stdlib.h>

extern "C"{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/rational.h>
#include <libavutil/time.h>
#include <libavutil/samplefmt.h>

}

#include "SDL.h"
#include <SDL_mutex.h>
#include <SDL_thread.h>
#include <SDL_timer.h>
#include <SDL_mutex.h>
#include <SDL_audio.h>
#include <SDL_main.h>
#include <SDL_pixels.h>

#define NS_MEDIA_BEGIN namespace media {
#define NS_MEDIA_END  }

//#define FRAME_QUEUE_SIZE 24
// 这边的QUEUE_SIZE最好为3，否则将会导致decode thread中的丢帧效果失效，原因是queue size过大，里面的数据量多，导致VD和VO两者直接使用的最新一笔数据pts差值变大，从而导致frame_drops_early失效
#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 18
#define PCM_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

#ifndef SAFE_AV_FREE
#define SAFE_AV_FREE(p) if(p != NULL) {av_free(p); p = NULL;}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { if (x) delete (x); (x) = NULL; }    //定义安全释放函数
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) { if (x) delete [] (x); (x) = NULL; }    //定义安全释放函数
#endif
#ifndef SAFE_FREE
#define SAFE_FREE(p) if(p != NULL) {free(p); p = NULL;}
#endif

#endif

