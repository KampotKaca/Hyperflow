#include "hanimationplayer.h"
#include "hyperflow.h"

namespace hf
{
    AnimationPlayer::AnimationPlayer(const AnimationPlayerCallbacks& callbacks) : callbacks(callbacks)
    {

    }

    AnimationPlayer::~AnimationPlayer()
    {

    }

    void Play(const Ref<AnimationPlayer>& player)
    {
        if (IsPlaying(player)) return;
        player->isPlaying = true;
    }

    void Pause(const Ref<AnimationPlayer>& player)
    {
        if (!IsPlaying(player)) return;
        player->isPlaying = false;
    }

    void Seek(const Ref<AnimationPlayer>& player, float_t positionInSeconds)
    {

    }

    void SeekPercent(const Ref<AnimationPlayer>& player, float_t position)
    {

    }

    double_t GetPlayedInSeconds(const Ref<AnimationPlayer>& player)
    {
        if (player->animation) return player->animationProgress;
        log_warn_s("[Hyperflow] %s", "Cannot compute played in seconds if there is no animation set!");
        return 0.0;
    }
    double_t GetPlayedPercent(const Ref<AnimationPlayer>& player) { return player->animationProgress; }
    bool IsPlaying(const Ref<AnimationPlayer>& player) { return player->isPlaying; }
    Ref<AnimationClip> GetAnim(const Ref<AnimationPlayer>& player) { return player->animation; }

    void ChangeAnim(const Ref<AnimationPlayer>& player, const Ref<AnimationClip>& anim, float_t startingDuration)
    {

    }
}
