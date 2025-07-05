#ifndef HAUDIOPLAYER_H
#define HAUDIOPLAYER_H

#include "hshared.h"

namespace hf
{
    struct AudioPlayer
    {
        explicit AudioPlayer(const AudioPlayerCreationInfo& info);
        ~AudioPlayer();

        AudioPlayerConfig config{};
        Ref<AudioClip> clip{};
        void* handle{};
        bool isPlaying = false;
        bool isLoaded = false;
    };
}

#endif //HAUDIOPLAYER_H
