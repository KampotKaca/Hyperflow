#ifndef HAUDIO_H
#define HAUDIO_H

#include "audio/haudioclip.h"
#include "miniaudio.h"

namespace hf::inter
{
    struct AudioData
    {
        ma_engine engine{};
    };

    extern AudioData AUDIO_DATA;
}

#endif //HAUDIO_H
