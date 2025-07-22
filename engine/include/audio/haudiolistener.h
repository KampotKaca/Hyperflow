#ifndef HAUDIOLISTENER_H
#define HAUDIOLISTENER_H

#include "hshared.h"

namespace hf
{
    struct AudioListener
    {
        explicit AudioListener(const AudioListenerCreationInfo& info);
        ~AudioListener();

        AudioCone cone{};
        vec3 velocity{};
        uint32_t handle{};
        bool isEnabled = false;
    };
}

#endif //HAUDIOLISTENER_H
