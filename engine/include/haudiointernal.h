#ifndef HAUDIO_H
#define HAUDIO_H

#include "miniaudio.h"

namespace hf::inter
{
    struct AudioData
    {
        ma_engine engine{};
        uint32_t definedListenersCount = 0;
    };

    extern AudioData AUDIO_DATA;
}

#endif //HAUDIO_H
