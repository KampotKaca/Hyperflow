#ifndef HYPERFLOW_HANIMATIONPLAYER_H
#define HYPERFLOW_HANIMATIONPLAYER_H

#include "hshared.h"

namespace hf
{
    struct AnimationPlayer
    {
        explicit AnimationPlayer(const AnimationPlayerCallbacks& callbacks);
        ~AnimationPlayer();

        AnimationPlayerCallbacks m_Callbacks{};
        Ref<Animation> m_Animation{};
        double_t m_AnimationProgress{};
        bool m_IsPlaying{};
    };
}

#endif //HYPERFLOW_HANIMATIONPLAYER_H