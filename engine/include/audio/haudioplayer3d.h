#ifndef HAUDIOPLAYER3D_H
#define HAUDIOPLAYER3D_H

#include "hshared.h"

namespace hf
{
    struct AudioPlayer3D
    {
        explicit AudioPlayer3D(const AudioPlayer3DCreationInfo& info);
        ~AudioPlayer3D();

        AudioPlayerSettings settings{};
        AudioPlayer3DSettings settings3d{};
        AudioCone cone{};
        vec3 velocity{};

        void* handle{};
        void* buffer{};
        Ref<AudioClip> clip{};
        Ref<AudioGroup> parent{};
        AudioPlayerStateFlags stateFlags{};
    };
}

#endif //HAUDIOPLAYER3D_H
