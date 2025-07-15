#ifndef AUDIOS_H
#define AUDIOS_H

#include <hyperflow.h>

namespace app
{
    struct AppAudios
    {
        hf::Ref<hf::AudioClip> cartoon_comedy;
        hf::Ref<hf::AudioPlayer> background_music;
        hf::Ref<hf::AudioPlayer> background_music2;
    };

    extern AppAudios APP_AUDIOS;

    void AudioLoadAll();
}

#endif //AUDIOS_H
