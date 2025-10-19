#ifndef HYPERFLOW_HANIMATION_H
#define HYPERFLOW_HANIMATION_H

#include "hshared.h"

namespace hf
{
    struct Animation
    {
        explicit Animation();
        ~Animation();

        float_t duration{};
        bool isLoaded{};
    };
}

#endif //HYPERFLOW_HANIMATION_H