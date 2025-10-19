#ifndef HYPERFLOW_HANIMATIONPLAYER_H
#define HYPERFLOW_HANIMATIONPLAYER_H

#include "hshared.h"

namespace hf
{
    struct AnimationPlayer
    {
        explicit AnimationPlayer(const AnimationPlayerCallbacks& callbacks);
        ~AnimationPlayer();

        AnimationPlayerCallbacks callbacks{};
        Ref<AnimationClip> animation{};
        double_t animationProgress{};
        bool isPlaying{};
    };
}

#endif //HYPERFLOW_HANIMATIONPLAYER_H