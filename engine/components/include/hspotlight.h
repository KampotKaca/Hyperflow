#ifndef HSPOTLIGHT_H
#define HSPOTLIGHT_H

#include "hexternal.h"

namespace hf
{
    struct SpotLight
    {
        vec3 position { 0, 5, 0 };
        vec3 direction { 0, -1, 0 };
        vec3 color { 1, 1, 1 };
        float_t innerAngle = 0.5f;
        float_t outerAngle = 3.0f;
        float_t range = 10.0f;
    };
}

#endif //HSPOTLIGHT_H
