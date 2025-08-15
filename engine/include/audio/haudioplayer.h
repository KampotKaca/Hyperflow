#ifndef HAUDIOPLAYER_H
#define HAUDIOPLAYER_H

#include "hshared.h"

namespace hf
{
    struct AudioPlayer
    {
        explicit AudioPlayer(const AudioPlayerCreationInfo& info);
        ~AudioPlayer();

        AudioPlayerSettings settings{};
        Ref<AudioClip> clip{};
        Ref<AudioGroup> parent{};
        void* handle{};
        void* buffer{};
        AudioPlayerStateFlags stateFlags{};
    };
}

#endif //HAUDIOPLAYER_H
