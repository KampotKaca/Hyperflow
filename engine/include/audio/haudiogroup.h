#ifndef HYPERFLOW_HAUDIOGROUP_H
#define HYPERFLOW_HAUDIOGROUP_H

#include "hshared.h"

namespace hf
{
    struct AudioGroup
    {
        explicit AudioGroup(const AudioGroupCreationInfo& info);
        ~AudioGroup();

        void* handle{};
        bool enabled = true;
        float_t volume = 1.0f;
        float_t pitch = 1.0f;
    };
}

#endif //HYPERFLOW_HAUDIOGROUP_H
