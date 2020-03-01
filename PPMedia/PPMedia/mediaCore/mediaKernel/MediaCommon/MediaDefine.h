//
//  MediaDefine.h
//  PPlayer
//
//  Created by 邱开禄 on 2019/11/14.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#ifndef MediaDefine_H
#define MediaDefine_H

#define NS_MEDIA_BEGIN namespace media {
#define NS_MEDIA_END  }

//#define FRAME_QUEUE_SIZE 24
// 这边的QUEUE_SIZE最好为3，否则将会导致decode thread中的丢帧效果失效，原因是queue size过大，里面的数据量多，导致VD和VO两者直接使用的最新一笔数据pts差值变大，从而导致frame_drops_early失效
/********************************************播放器内部参数宏定义********************************************/
#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 18
#define PCM_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))
#define MAX_DCODEC_STREAM_NUM 5
// 设置packetQueue的大小15M
#define MAX_PACKET_QUEUE_SIZE (15 * 512 *1024)

/********************************************对象内存安全释放********************************************/
#ifndef SAFE_AV_FREE
#define SAFE_AV_FREE(p) if(p != NULL) {av_free(&p); p = NULL;}
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

/********************************************定义打印方式********************************************/
#define VLOG(level, TAG, ...)    ((void)vprintf(__VA_ARGS__))
#define ALOG(level, TAG, ...)    ((void)printf(__VA_ARGS__))

/********************************************定义log打印级别********************************************/
#define PP_LOG_UNKNOWN     0
#define PP_LOG_DEFAULT     1
#define PP_LOG_VERBOSE     2
#define PP_LOG_DEBUG       3
#define PP_LOG_INFO        4
#define PP_LOG_WARN        5
#define PP_LOG_ERROR       6
#define PP_LOG_FATAL       7
#define PP_LOG_SILENT      8

#endif

