#ifndef HYPERFLOW_HANIMATION_H
#define HYPERFLOW_HANIMATION_H

#include "hshared.h"

namespace hf
{
    struct AnimationClip
    {
        AnimationClip();
        virtual ~AnimationClip() = 0;

        virtual void Play() = 0;
        virtual void Unload() = 0;

        uint32_t frameCount{};
        uint16_t frameRate{};
        bool isLoaded{};
    };
}

#endif //HYPERFLOW_HANIMATION_H