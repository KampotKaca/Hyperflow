#include "hanimationplayer.h"
#include "hyperflow.h"

namespace hf
{
    AnimationPlayer::AnimationPlayer(const AnimationPlayerCallbacks& callbacks) : m_Callbacks(callbacks)
    {

    }

    AnimationPlayer::~AnimationPlayer()
    {

    }

    void Play(const Ref<AnimationPlayer>& player)
    {
        if (IsPlaying(player)) return;
        player->m_IsPlaying = true;
    }

    void Pause(const Ref<AnimationPlayer>& player)
    {
        if (!IsPlaying(player)) return;
        player->m_IsPlaying = false;
    }

    void Seek(const Ref<AnimationPlayer>& player, float_t positionInSeconds)
    {

    }

    void SeekPercent(const Ref<AnimationPlayer>& player, float_t position)
    {

    }

    double_t GetPlayedInSeconds(const Ref<AnimationPlayer>& player)
    {
        if (player->m_Animation) return player->m_AnimationProgress;
        LOG_WARN("[Hyperflow] %s", "Cannot compute played in seconds if there is no animation set!");
        return 0.0;
    }
    double_t GetPlayedPercent(const Ref<AnimationPlayer>& player) { return player->m_AnimationProgress; }
    bool IsPlaying(const Ref<AnimationPlayer>& player) { return player->m_IsPlaying; }
    Ref<Animation> GetAnim(const Ref<AnimationPlayer>& player) { return player->m_Animation; }

    void ChangeAnim(const Ref<AnimationPlayer>& player, const Ref<Animation>& anim, float_t startingDuration)
    {

    }
}
