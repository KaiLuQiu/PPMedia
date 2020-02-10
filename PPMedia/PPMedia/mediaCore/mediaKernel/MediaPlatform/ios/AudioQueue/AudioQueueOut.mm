/*
 * ijksdl_aout_ios_audiounit.m
 *
 * Copyright (c) 2013 Bilibili
 * Copyright (c) 2013 Zhang Rui <bbcallen@gmail.com>
 *
 * This file is part of ijkPlayer.
 *
 * ijkPlayer is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * ijkPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ijkPlayer; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "AudioQueueOut.h"
#include "MediaPipelineNode.h"
#include <stdbool.h>
#include <assert.h>
#include "sdl_audio.h"


#import "IJKSDLAudioUnitController.h"
#import "IJKSDLAudioQueueController.h"

static SDL_Class g_pipeline_class = {
    .name = "AudioQueue",
};

#define SDL_IOS_AUDIO_MAX_CALLBACKS_PER_SEC 15

struct SDL_Aout_Opaque {
    IJKSDLAudioQueueController *aoutController;
};

static int aout_open_audio(Aout_Pipenode *aout, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{
    assert(desired);
    printf("aout_open_audio()\n");
    SDL_Aout_Opaque *opaque = aout->opaque;

    opaque->aoutController = [[IJKSDLAudioQueueController alloc] initWithAudioSpec:desired];
    if (!opaque->aoutController) {
        printf("aout_open_audio_n: failed to new AudioTrcak()\n");
        return -1;
    }

    if (obtained)
        *obtained = opaque->aoutController.spec;

    return 0;
}

static void aout_pause_audio(Aout_Pipenode *aout, int pause_on)
{
    printf("aout_pause_audio(%d)\n", pause_on);
    SDL_Aout_Opaque *opaque = aout->opaque;

    if (pause_on) {
        [opaque->aoutController pause];
    } else {
        [opaque->aoutController play];
    }
}

static void aout_flush_audio(Aout_Pipenode *aout)
{
    printf("aout_flush_audio()\n");
    SDL_Aout_Opaque *opaque = aout->opaque;

    [opaque->aoutController flush];
}

static void aout_close_audio(Aout_Pipenode *aout)
{
    printf("aout_close_audio()\n");
    SDL_Aout_Opaque *opaque = aout->opaque;

    [opaque->aoutController close];
}

static void aout_set_playback_rate(Aout_Pipenode *aout, float playbackRate)
{
    printf("aout_close_audio()\n");
    SDL_Aout_Opaque *opaque = aout->opaque;

    [opaque->aoutController setPlaybackRate:playbackRate];
}

static void aout_set_playback_volume(Aout_Pipenode *aout, float volume)
{
    printf("aout_set_volume()\n");
    SDL_Aout_Opaque *opaque = aout->opaque;

    [opaque->aoutController setPlaybackVolume:volume];
}

static double auout_get_latency_seconds(Aout_Pipenode *aout)
{
    SDL_Aout_Opaque *opaque = aout->opaque;
    return [opaque->aoutController get_latency_seconds];
}

static int aout_get_persecond_callbacks(Aout_Pipenode *aout)
{
    return SDL_IOS_AUDIO_MAX_CALLBACKS_PER_SEC;
}

static void aout_free_l(Aout_Pipenode *aout)
{
    if (!aout)
        return;

    aout_close_audio(aout);

    SDL_Aout_Opaque *opaque = aout->opaque;
    if (opaque) {
        [opaque->aoutController release];
        opaque->aoutController = nil;
    }

    media::MediaPipelineNode::PipeAudioOutNode_free(aout);
}

Aout_Pipenode *SDL_AoutIos_CreateForAudioUnit()
{
    Aout_Pipenode* node = media::MediaPipelineNode::PipeAudioOutNode_alloc(&g_pipeline_class, sizeof(SDL_Aout_Opaque));
    if (!node)
        return node;

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




