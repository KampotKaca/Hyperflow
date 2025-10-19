#include "hanimation.h"

namespace hf
{
    Animation::Animation()
    {

    }

    Animation::~Animation()
    {

    }

    void Destroy(const Ref<Animation>& anim)
    {

    }

    void Destroy(const Ref<Animation>* pAnimations, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {

        }
    }

    bool IsLoaded(const Ref<Animation>& anim) { return anim->isLoaded; }
    float_t GetDuration(const Ref<Animation>& anim) { return anim->duration; }
}