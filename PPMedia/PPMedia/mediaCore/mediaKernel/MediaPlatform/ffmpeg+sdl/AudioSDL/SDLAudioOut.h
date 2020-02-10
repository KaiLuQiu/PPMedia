//
//  SDLAudioOut.h
//  PPMedia
//
//  Created by 邱开禄 on 2020/02/09.
//  Copyright © 2020 邱开禄. All rights reserved.
//

#ifndef SDLAudioOut_H
#define SDLAudioOut_H

#include "MediaPipelineNode.h"
#include "MediaCommon.h"
#include "SDLCommon.h"

NS_MEDIA_BEGIN

class SDLAudioOut {
public:
    static Aout_Pipenode *pipenode_create_audio_out_by_SDL();
private:
    static int aout_open_audio(Aout_Pipenode *aout, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
    
    static void aout_pause_audio(Aout_Pipenode *aout, int pause_on);
    
    static void aout_flush_audio(Aout_Pipenode *aout);

    static void aout_close_audio(Aout_Pipenode *aout);

    static void aout_set_playback_rate(Aout_Pipenode *aout, float playbackRate);

    static void aout_set_playback_volume(Aout_Pipenode *aout, float volume);

    static double auout_get_latency_seconds(Aout_Pipenode *aout);

    static int aout_get_persecond_callbacks(Aout_Pipenode *aout);

    static void aout_free_l(Aout_Pipenode *aout);
};

NS_MEDIA_END
#endif
