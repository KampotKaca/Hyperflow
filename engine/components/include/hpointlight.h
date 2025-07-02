#ifndef HPOINTLIGHT_H
#define HPOINTLIGHT_H

#include "hexternal.h"

namespace hf
{
    struct PointLight
    {
        vec3 position { 0, 5, 0 };
        vec3 color { 1, 1, 1 };
        float range = 10;
    };
}

#endif //HPOINTLIGHT_H
