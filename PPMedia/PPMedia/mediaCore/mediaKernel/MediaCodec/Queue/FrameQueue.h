//
//  FrameQueue.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef FrameQueue_H
#define FrameQueue_H
#include "MediaCommon.h"
#include "PacketQueue.h"

NS_MEDIA_BEGIN

//解码帧信息
typedef struct Frame_T {
    Frame_T()
    {
        frame = NULL;
        memset(&sub, 0, sizeof(AVSubtitle));
        serial = -1;
        pts = -1;
        duration = -1;
        pos = -1;
        width = -1;
        height = -1;
        format = -1;
        memset(&sar, 0, sizeof(AVRational));
        uploaded = -1;
        flip_v = -1;
    }
    ~Frame_T()
    {
        SAFE_DELETE(frame);
        memset(&sub, 0, sizeof(AVSubtitle));
        serial = -1;
        pts = -1;
        duration = -1;
        pos = -1;
        width = -1;
        height = -1;
        format = -1;
        memset(&sar, 0, sizeof(AVRational));
        uploaded = -1;
        flip_v = -1;
    }
    AVFrame *frame;
    AVSubtitle sub;
    int serial;
    double pts;           //解码时间戳
    double duration;      //帧时长信息
    int64_t pos;          //在码流的位置信息
    int width;            //视频的宽
    int height;           //视频的高
    int format;           //图像的格式
    AVRational sar;
    int uploaded;
    int flip_v;           //镜像
} Frame;

class FrameQueue
{
public:
    FrameQueue();
    
    ~FrameQueue();
    
    /*
     * Framequeue的初始化
     */
    int frame_queue_init(PacketQueue *pktq, int max_size, int keep_last);

    /*
     * 获取待显示的第一个帧
     */
    Frame *frame_queue_peek(void);
    
    /*
     * 获取待显示的第二个帧
     */
    Frame *frame_queue_peek_next(void);
    
    /*
     * 获取当前播放器显示的帧
     */
    Frame *frame_queue_peek_last(void);
    
    /*
     * 获取queue中一块Frame大小的可写内存
     */
    Frame *frame_queue_peek_writable(void);
    
    /*
     * 这方法和frame_queue_peek的作用一样， 都是获取待显示的第一帧
     */
    Frame *frame_queue_peek_readable(void);
    
    /*
     * 推入一帧数据， 其实数据已经在调用这个方法前填充进去了， 这个方法的作用是将队列的写索引(也就是队尾)向后移， 还有将这个队列中的Frame的数量加一。
     */
    void frame_queue_push(void);
    
    /*
     * 将读索引(队头)后移一位， 还有将这个队列中的Frame的数量减一
     */
    void frame_queue_next(void);
    
    /*
     * 返回队列中待显示帧的数目
     */
    int frame_queue_nb_remaining(void);
    
    /*
     * 返回正在显示的帧的position
     */
    int64_t frame_queue_last_pos(void);
    
    /*
     * 取消帧引用的所有缓冲区并重置帧字段，释放给定字幕结构中的所有已分配数据。
     */
    void frame_queue_unref_item(Frame *vp);
    
    /*
     * 释放Frame，释放互斥锁和互斥量
     */
    void frame_queue_destory();
    
private:
    // 帧队列
    Frame               *queue;
    // 读索引
    int                 rindex;
    // 写索引
    int                 windex;
    // 总帧数
    int                 size;
    // 最大缓存大小
    int                 max_size;
    // keep_last是一个bool值，表示是否在环形缓冲区的读写过程中保留最后一个读节点不被覆写
    int                 keep_last;
    // 当前显示这一帧的读索引
    int                 rindex_shown;
    // 条件cond
    pthread_cond_t      *cond;
    // 锁
    pthread_mutex_t     *mutex;
    // packet Queue指针
    PacketQueue         *pktq;
};
NS_MEDIA_END


#endif // FrameQueue.h
