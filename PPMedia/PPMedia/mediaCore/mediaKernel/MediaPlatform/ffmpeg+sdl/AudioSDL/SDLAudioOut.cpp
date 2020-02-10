//
//  SDLAudioOut.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/09.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#include "SDLAudioOut.h"
#include "MediaPipelineNode.h"
NS_MEDIA_BEGIN

static SDL_Class g_pipeline_class = {
    .name = "SDLAudio",
};

// 以下部分是函数方法的映射
int SDLAudioOut::aout_open_audio(Aout_Pipenode *aout, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{
    return SDL_OpenAudio(const_cast<SDL_AudioSpec*>(desired), obtained);
}

void SDLAudioOut::aout_pause_audio(Aout_Pipenode *aout, int pause_on)
{
    printf("aout_pause_audio(%d)\n", pause_on);
    SDL_PauseAudio(pause_on);
}

void SDLAudioOut::aout_flush_audio(Aout_Pipenode *aout)
{
    printf("aout_flush_audio()\n");
}

void SDLAudioOut::aout_close_audio(Aout_Pipenode *aout)
{
    printf("aout_close_audio()\n");
    SDL_CloseAudio();
}

void SDLAudioOut::aout_set_playback_rate(Aout_Pipenode *aout, float playbackRate)
{
    printf("aout_close_audio()\n");
}

void SDLAudioOut::aout_set_playback_volume(Aout_Pipenode *aout, float volume)
{
    printf("aout_set_volume()\n");
}

double SDLAudioOut::auout_get_latency_seconds(Aout_Pipenode *aout)
{
    return 0.0;
}

int SDLAudioOut::aout_get_persecond_callbacks(Aout_Pipenode *aout)
{
    return 0;
}

void SDLAudioOut::aout_free_l(Aout_Pipenode *aout)
{
    if (!aout)
        return;

    aout_close_audio(aout);
    
    MediaPipelineNode::PipeAudioOutNode_free(aout);
}

Aout_Pipenode* SDLAudioOut::pipenode_create_audio_out_by_SDL()
{
    Aout_Pipenode* node = MediaPipelineNode::PipeAudioOutNode_alloc(&g_pipeline_class, 0);
    if (!node)
        return node;

    // Aout_Pipenode_Opaque *opaque = aout->opaque;
    node->free_l = aout_free_l;
    node->open_audio  = aout_open_audio;
    node->pause_audio = aout_pause_audio;
    node->flush_audio = aout_flush_audio;
    node->close_audio = aout_close_audio;

    node->func_set_playback_rate = aout_set_playback_rate;
    node->func_set_playback_volume = aout_set_playback_volume;
    node->func_get_latency_seconds = auout_get_latency_seconds;
    node->func_get_audio_persecond_callbacks = aout_get_persecond_callbacks;
    return node;
}

NS_MEDIA_END
