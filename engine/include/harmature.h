#ifndef HYPERFLOW_HARMATURE_H
#define HYPERFLOW_HARMATURE_H

#include "hshared.h"

namespace hf
{
    struct Armature
    {
        struct Bone
        {
            mat4 transform{};
            std::string name{};
            int32_t parentIndex = -1;
        };

        List<Bone> bones{};
    };
}

#endif //HYPERFLOW_HARMATURE_H