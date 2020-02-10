//
//  NodeDefine.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/03.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef NodeDefine_H
#define NodeDefine_H

typedef struct SDL_Class {
    const char *name;
} SDL_Class;

typedef struct Decode_Pipenode Decode_Pipenode;
// 映射到对应解码函数指针
struct Decode_Pipenode {
    SDL_Class                   *opaque_class;
    void (*func_destroy)        (void);
    int  (*func_execute)        (void *arg);
    int  (*func_flush)          (void); // optional
};

// 参考ijkplayer
// 映射到对应平台的音频输出器（iOS:AudioQueue android:AudioTrack）
typedef struct SDL_Aout_Opaque SDL_Aout_Opaque;
typedef struct Aout_Pipenode Aout_Pipenode;
// 不同平台会定义不同平台使用的SDL_mutex & SDL_AudioSpec(这边只做声明)
typedef struct SDL_mutex SDL_mutex;
typedef struct SDL_AudioSpec SDL_AudioSpec;

struct Aout_Pipenode {
//    SDL_mutex                   *mutex;
    SDL_Class                   *opaque_class;
    SDL_Aout_Opaque             *opaque;

    // audio 输出的延迟时间
    double                      minimal_latency_seconds;
//    Aout_Pipenode_Opaque             *opaque;
    void    (*free_l)(Aout_Pipenode *vout);
    int     (*open_audio)(Aout_Pipenode *aout, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
    void    (*pause_audio)(Aout_Pipenode *aout, int pause_on);
    void    (*flush_audio)(Aout_Pipenode *aout);
    void    (*set_volume)(Aout_Pipenode *aout, float left, float right);
    void    (*close_audio)(Aout_Pipenode *aout);

    double  (*func_get_latency_seconds)(Aout_Pipenode *aout);
    void    (*func_set_default_latency_seconds)(Aout_Pipenode *aout, double latency);

    // optional
    void    (*func_set_playback_rate)(Aout_Pipenode *aout, float playbackRate);
    void    (*func_set_playback_volume)(Aout_Pipenode *aout, float playbackVolume);
    int     (*func_get_audio_persecond_callbacks)(Aout_Pipenode *aout);

    // Android only
    int    (*func_get_audio_session_id)(Aout_Pipenode *aout);
};

#endif // MediaPipelineNode_H

