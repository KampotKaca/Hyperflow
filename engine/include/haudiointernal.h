#ifndef HAUDIO_H
#define HAUDIO_H

#include "miniaudio.h"

namespace hf::inter
{
    struct AudioData
    {
        ma_engine engine{};
        uint32_t definedListenersCount = 0;
        bool isLoaded = false;

        Ref<AudioGroup> group2D{};
        Ref<AudioGroup> group3D{};
    };

    extern AudioData AUDIO_DATA;
}

#endif //HAUDIO_H
