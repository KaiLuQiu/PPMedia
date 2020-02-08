//
//  DemuxdeThread.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/15.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "DemuxdeThread.h"
#include <pthread.h>
#include "MediaCommon.h"
#include "MediaContext.h"
#include "PacketQueue.h"
#include "FFmpegInit.h"
#include "ThreadController.h"

NS_MEDIA_BEGIN

void *DemuxerThread(void *arg)
{
    MediaContext *mediaContext = (MediaContext *)arg;
    AVFormatContext* formatContext = mediaContext->formatContext;
    // 获取当前这个媒体文件的流数量
    int nb_streams = mediaContext->nbStreams;
    PacketQueue* packetQueue = NULL;
    int64_t allPacketQueueSize = 0;
    AVPacket pkt;
    memset(&pkt, 0, sizeof(AVPacket));
    ThreadController* demuxerThreadController = mediaContext->demuxerThreadController;
    if (NULL == demuxerThreadController) {
        printf("demuxerThreadController is NULL fail\n");
        return NULL;
    }
    while(false == mediaContext->stopCodecThread) {
        // 如果有seek请求则执行以下操作
//        if (seek) {
//            ret = pMediaCore->Seek(pSeekPos, AVSEEK_FLAG_BACKWARD);
//             if (ret >= 0) {
//                    for (int i = 0; i < nb_streams; i++) {
//                        packetQueue = mediaContext->GetPacketQueue(i);
//                        if(NULL == packetQueue)
//                            continue;
//                        // 清空这个flush
//                        packetQueue->packet_queue_flush();
//                        // 输入一个flush_Pkt，通知decoder去flush avcodec
//                        packetQueue->packet_queue_put(FFmpegInit::flushPkt);
//                    }
//                 // 清空audio packet队列
//                 audioPackeQueueFunc->packet_queue_flush(audioRingBuffer);
//                 audioPackeQueueFunc->packet_queue_put(audioRingBuffer, pPlayerContext->audio_flush_pkt);
//                 // 清空video packet队列
//                 videoPackeQueueFunc->packet_queue_flush(videoRingBuffer);
//                 videoPackeQueueFunc->packet_queue_put(videoRingBuffer, pPlayerContext->video_flush_pkt);
//                 // 发送seek完成的消息
//                 if (NULL != pHandler)
//                     pHandler->sendOnSeekCompletion();
//             } else {
//                 // 发送seek 失败的消息
//                 if (NULL != pHandler)
//                     pHandler->sendOnSeekFail();
//             }
//        }
        // 遍历每一条流计算总的packet包大小,如果大于一个阈值需要暂停demuxer，让这个线程挂起
        allPacketQueueSize = 0;
        for (int i = 0; i < nb_streams; i++) {
            packetQueue = mediaContext->GetPacketQueue(i);
            if(NULL == packetQueue)
                continue;
            allPacketQueueSize += packetQueue->size;
        }
        if (allPacketQueueSize >= MAX_PACKET_QUEUE_SIZE) {
            // TODO
            // 挂起这个线程
        }
        // 调用ffmpeg读包操作，内部会调用到对应的容器进行read,parse包的
        int ret = av_read_frame(formatContext, &pkt);
        // 如果读包失败
        if (ret < 0) {
            if ((ret == AVERROR_EOF || avio_feof(formatContext->pb)) && !mediaContext->eof) {
                // 说明已经读取到结尾位置了
                for (int i = 0; i < nb_streams; i++) {
                    packetQueue = mediaContext->GetPacketQueue(i);
                    if(NULL == packetQueue)
                        continue;
                    // 这边推2笔空buff是因为，第一笔是为了获取eof，第二笔只是暂时处理decodeThread的bug
                    packetQueue->packet_queue_put_nullpacket(i);
                    packetQueue->packet_queue_put_nullpacket(i);
                }
            
                mediaContext->eof = 1;
                if (mediaContext->isLoop) {
                    //TODO
                    // seek到开头
                } else {
                    //TODO
                    //挂起当前线程
                    demuxerThreadController->wait();
                }
            }
            continue;
        }
        int streamIndex = pkt.stream_index;
        packetQueue = mediaContext->GetPacketQueue(streamIndex);
        if (packetQueue) {
            packetQueue->packet_queue_put(&pkt);
        } else {
            // 如果这个队列不存在，则释放这个pkt
            av_packet_unref(&pkt);
            continue;
        }
    }
    return NULL;
}

NS_MEDIA_END

