#ifndef HAUDIOPLAYER_H
#define HAUDIOPLAYER_H

#include "hshared.h"

namespace hf
{
    struct AudioPlayer
    {
        explicit AudioPlayer();
        ~AudioPlayer();

        AudioPlayerConfig config{};
        Ref<AudioClip> clip{};
    };
}

#endif //HAUDIOPLAYER_H
