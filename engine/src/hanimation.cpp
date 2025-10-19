#include "hanimation.h"
#include "hinternal.h"

namespace hf
{
    AnimationClip::AnimationClip()
    {
        isLoaded = true;
    }

    AnimationClip::~AnimationClip()
    {
        inter::general::DestroyAnimationClip_i(this);
    }

    void Destroy(const Ref<AnimationClip>& anim)
    {
        inter::general::DestroyAnimationClip_i(anim.get());
        inter::HF.generalResources.animations.erase((uint64_t)anim.get());
    }

    void Destroy(const Ref<AnimationClip>* pAnimations, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto anim = pAnimations[i];
            inter::general::DestroyAnimationClip_i(anim.get());
            inter::HF.generalResources.animations.erase((uint64_t)anim.get());
        }
    }

    bool IsLoaded(const Ref<AnimationClip>& anim) { return anim->isLoaded; }
    float_t GetDuration(const Ref<AnimationClip>& anim) { return (float_t)anim->frameCount / anim->frameRate; }
    uint16_t GetFrameRate(const Ref<AnimationClip>& anim) { return anim->frameRate; }
    uint32_t GetFrameCount(const Ref<AnimationClip>& anim) { return anim->frameCount; }

    namespace inter::general
    {
        void DestroyAnimationClip_i(AnimationClip* anim)
        {
            if (anim->isLoaded)
            {
                anim->isLoaded = false;
                anim->Unload();
            }
        }
    }
}