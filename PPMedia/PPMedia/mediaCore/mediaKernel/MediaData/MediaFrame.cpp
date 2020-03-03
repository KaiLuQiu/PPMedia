//
//  MediaFrame.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/29.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "MediaFrame.h"
NS_MEDIA_BEGIN

MediaFrame::MediaFrame():
frameType(PP_FRAME_NONE),
isInitSrcVideoParam(false),
isInitSrcAudioParam(false),
isInitDstVideoParam(false),
isInitDstAudioParam(false),
srcData(NULL),
dstData(NULL),
srcSize(0),
dstSize(0),
outSize(0),
AudioFIFO(NULL)
{
    videoConver = new (std::nothrow)VideoConver();
    videoConver->init();
    // 设置默认的参数信息
    videoConver->setVideoInfo(srcVideoParam, dstVideoParam);
    audioConver = new AudioResSample();
    audioConver->init(srcAudioParam, dstAudioParam);
    srcFrame = av_frame_alloc();
    dstFrame = av_frame_alloc();
}

MediaFrame::~MediaFrame()
{
    if (AudioFIFO) {
        av_audio_fifo_free(AudioFIFO);
        AudioFIFO = NULL;
    }
    
    if (srcFrame) {
        av_frame_free(&srcFrame);
        srcFrame = NULL;
    }
    
    if (dstFrame) {
        av_frame_free(&dstFrame);
        dstFrame = NULL;
    }
    
    if (srcData) {
        av_freep(&srcData);
    }
    
    if (dstData) {
        av_freep(&dstData);
    }
    
    if (audioConver) {
        delete audioConver;
        audioConver = NULL;
    }
    
    if (videoConver) {
        delete videoConver;
        videoConver = NULL;
    }
    
    isInitSrcVideoParam = false;
    isInitSrcAudioParam = false;
    
    isInitDstVideoParam = false;
    isInitDstAudioParam = false;
}

void MediaFrame::setMediaType(FrameType type)
{
    frameType = type;
}

bool MediaFrame::setSrcAudioParam(AudioParamInfo srcParam)
{
    srcAudioParam = srcParam;
    if (isInitDstAudioParam == false) {
        printf("MediaFrame: dstAudioParam not set\n");
        return false;
    }
    // 如果当前的输入参数和dstAudioParam(表示音频输出的)不同的话，则初始化重采样器
    if (dstAudioParam.channels != srcAudioParam.channels ||
        dstAudioParam.sample_rate != srcAudioParam.sample_rate ||
        dstAudioParam.frame_size != srcAudioParam.frame_size ||
        dstAudioParam.fmt != srcAudioParam.fmt) {
        // 表明输入和输出的音频格式不一样，则进行重采样操作
        if (audioConver) {
            audioConver->release();
            delete audioConver;
            audioConver = NULL;
        }
        audioConver = new AudioResSample();
        // 对重采样器设置输入输出的音频参数
        audioConver->init(srcAudioParam, dstAudioParam);
        // 如果之前创建了audioFIFO则先释放了，重新根据新的参数创建信息的fifo
        if (AudioFIFO) {
            av_audio_fifo_free(AudioFIFO);
            AudioFIFO = NULL;
        }
        AudioFIFO = av_audio_fifo_alloc(srcAudioParam.fmt, srcAudioParam.channels, 1);
        if (NULL == AudioFIFO) {
            printf("MediaFrame: AudioFIFO is null\n");
        }
        
    }
    isInitSrcAudioParam = true;
    return true;
}

bool MediaFrame::setDstAudioParam(AudioParamInfo dstParam)
{
    dstAudioParam = dstParam;
    isInitDstAudioParam = true;
    return true;
}

bool MediaFrame::setSrcVideoParam(videoParamInfo srcParam)
{
    srcVideoParam = srcParam;
    if (isInitDstVideoParam == false) {
        printf("MediaFrame: dstVideoParam not set\n");
        return false;
    }
    // 设置视频格式转换的参数信息
    videoConver->setVideoInfo(srcVideoParam, dstVideoParam);
    isInitSrcVideoParam = true;
    return true;
}

bool MediaFrame::setDstVideoParam(videoParamInfo dstParam)
{
    dstVideoParam = dstParam;
    isInitDstVideoParam = true;
    return true;
}

int MediaFrame::writeFrameData(uint8_t* data, int64_t dataSize)
{
    if (NULL == data || 0 == dataSize)
        return -1;
    if (dataSize > srcSize) {
        av_fast_malloc(&srcData, (unsigned int *)&srcSize, dataSize);
    }
    memmove(srcData, data, dataSize);
    
    srcSize = dataSize;
    
    return 0;
}

uint8_t* MediaFrame::readFrameData(int64_t& dataSize)
{
    dataSize = outSize;
    return dstData;
}

int MediaFrame::writeFrame(AVFrame* frame)
{
    if (NULL == frame || NULL == srcFrame)
        return -1;
    // 解除本frame对本frame中所有缓冲区的引用，并复位frame中各成员。
    av_frame_unref(srcFrame);
    // 复制AVFrame元数据, 元数据指视频的一些非数据字段
    av_frame_copy_props(srcFrame, frame);
    // 将src中所有数据拷贝到dst中，并复位src。
    // 看到这个函数声明，第一感觉是它也会先 reset dst，其实错了!如果你没有先调用 av_frame_unref(dst)，这个函数会导致大量的内存泄漏
    // 为避免内存泄漏，在调用av_frame_move_ref(dst, src)之前应先调用av_frame_unref(dst) 。
    av_frame_move_ref(srcFrame, frame);
    return 0;
}

bool MediaFrame::Conver()
{
    bool ret = false;
    switch (frameType) {
        case PP_FRAME_VIDEO:
        {
            if (videoConver && isInitSrcVideoParam && isInitDstVideoParam) {
                // 设置dst的frame参数信息
                dstFrame->format = dstVideoParam.pixelFormat;
                dstFrame->width = dstVideoParam.width;
                dstFrame->height = dstVideoParam.height;
                // 计算所需的dataSize
                int dst_bytes_num = avpicture_get_size(dstVideoParam.pixelFormat, dstVideoParam.width, dstVideoParam.height);
                if (dstSize < dst_bytes_num) {
                    // dstData内存的申请
                    av_fast_malloc(&dstData, (unsigned int *)&dstSize, dst_bytes_num);
                }
                //前面的av_frame_alloc函数，只是为这个AVFrame结构体分配了内存，
                //而该类型的指针指向的内存还没分配。这里把av_fast_malloc得到的内存和AVFrame关联起来。
                //当然，其还会设置AVFrame的其他成员
                avpicture_fill((AVPicture*)dstFrame, dstData, dstVideoParam.pixelFormat, dstVideoParam.width, dstVideoParam.height);
                // 视频格式转换
                videoConver->Conver(srcFrame, dstFrame);
                if (dstVideoParam.pixelFormat == AV_PIX_FMT_RGBA) {
                    // 如果是rgba则，dstData存放的就是RGBA真正的图像数据
                } else{
                    // 其他格式在做相关处理TODO
                }
                // 更新输出的数据大小
                outSize = dst_bytes_num;

                ret = true;
            } else {
                printf("MediaFrame: VideoConver is null\n");
            }
        }
        break;
        case PP_FRAME_AUDIO:
        {
            if (audioConver && isInitSrcAudioParam && isInitDstAudioParam && AudioFIFO) {
                // 计算源的bufferSize大小
                int srcBufferSize = av_samples_get_buffer_size(srcFrame->linesize, srcAudioParam.channels, srcAudioParam.frame_size, srcAudioParam.fmt, 1);
                
                if (srcSize < srcBufferSize) {
                    av_fast_malloc(&srcData, (unsigned int *)&srcSize, srcBufferSize);
                }
                
                uint8_t* audioBufferPtr[8];
                int lineSize[8];
                
                for (int i = 0; i < 8; i++) {
                    audioBufferPtr[i] = NULL;
                }
                av_samples_fill_arrays(audioBufferPtr, lineSize, srcData, srcAudioParam.channels, srcAudioParam.frame_size, srcAudioParam.fmt, 1);

                int retFifo = av_audio_fifo_write(AudioFIFO, (void **)srcFrame->data, srcFrame->nb_samples);
                retFifo = av_audio_fifo_read(AudioFIFO, (void **)audioBufferPtr, srcFrame->nb_samples);
                
                // 重采样输出参数：输出音频样本数(多加了256个样本) 参考ffplay
                int out_count = (int64_t)srcAudioParam.frame_size * dstAudioParam.sample_rate / srcAudioParam.sample_rate + 256;
                // 计算BufferSize
                int out_size = av_samples_get_buffer_size(NULL, dstAudioParam.channels, out_count, dstAudioParam.fmt, 0);
                
                if (dstSize < out_size) {
                    av_fast_malloc(&dstData, (unsigned int *)&dstSize, out_size);
                }
                // 音频数据重采样
                int retSize = audioConver->Conver(srcData, dstData);
                int dstSizePerSample = dstAudioParam.channels * av_get_bytes_per_sample(dstAudioParam.fmt);
                // 设置输出的数据大小
                outSize = retSize * dstSizePerSample;
                ret = true;
            } else {
                printf("MediaFrame: audioConver is null\n");
            }
        }
        break;
        default:
            break;
    }
    return ret;
}
NS_MEDIA_END

