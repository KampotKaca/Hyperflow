#include "audio/haudioplayer.h"
#include "hinternal.h"
#include "audio/haudioimpl.h"
#include "hyperflow.h"

namespace hf
{
    namespace ir
    {
        template bool IsLoaded_i(AudioPlayer* player);
        template bool IsPlaying_i(AudioPlayer* player);

        template void FreeHandle_i(AudioPlayer* player);
        template bool ChangeClip_i(AudioPlayer* player, const Ref<AudioClip>& clip, uint32_t flags, float_t startingDuration);
        template void Destructor_i(AudioPlayer* player);
        template double_t GetPlayedInSeconds_i(AudioPlayer* player);
        template double_t GetPlayedPercent_i(AudioPlayer* player);
        template void Seek_i(AudioPlayer* player, float_t positionInSeconds);
        template void SeekPercent_i(AudioPlayer* player, float_t positionInSeconds);
        template void Play_i(AudioPlayer* player);
        template void Pause_i(AudioPlayer* player);

        template void SetVolume_i(AudioPlayer* player, float_t volume);
        template void SetPitch_i(AudioPlayer* player, float_t pitch);
        template void SetLoopingMode_i(AudioPlayer* player, bool loopingEnabled);
    }

    AudioPlayer::AudioPlayer(const AudioPlayerCreationInfo& info) :
    settings(info.settings), parent(info.parent)
    {
        if (!parent) parent = ir::AUDIO_DATA.group2D;
        ir::ChangeClip_i(this, info.clip, MA_SOUND_FLAG_NO_SPATIALIZATION);
    }

    AudioPlayer::~AudioPlayer()
    {
        ir::FreeHandle_i(this);
        ir::Destructor_i(this);
    }

    Ref<AudioPlayer> Create(const AudioPlayerCreationInfo& info)
    {
        Ref<AudioPlayer> player = MakeRef<AudioPlayer>(info);
        ir::HF.audioResources.players[(uint64_t)player.get()] = player;
        player->stateFlags |= AudioPlayerStateFlags::Loaded;
        return player;
    }

    void Destroy(const Ref<AudioPlayer>& player)
    {
        if (!IsLoaded(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");

        ir::FreeHandle_i(player.get());
        ir::HF.audioResources.players.erase((uint64_t)player.get());
        player->stateFlags &= (AudioPlayerStateFlags)~(uint32_t)AudioPlayerStateFlags::Loaded;
    }

    void Destroy(const Ref<AudioPlayer>* pPlayers, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto player = pPlayers[i];
            Destroy(player);
        }
    }

    bool IsLoaded(const Ref<AudioPlayer>& player) { return ir::IsLoaded_i(player.get()); }
    bool IsPlaying(const Ref<AudioPlayer>& player) { return ir::IsPlaying_i(player.get()); }

    void ChangeClip(const Ref<AudioPlayer>& player, const Ref<AudioClip>& clip, float_t startingDuration)
    {
        if (!IsLoaded(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        ir::ChangeClip_i(player.get(), clip, MA_SOUND_FLAG_NO_SPATIALIZATION, startingDuration);

        if (IsPlaying(player) && ma_sound_start((ma_sound*)player->handle) != MA_SUCCESS)
            LOG_ERROR("Unable to play audio player");
    }
    void Play(const Ref<AudioPlayer>& player) { ir::Play_i(player.get()); }
    void Pause(const Ref<AudioPlayer>& player) { ir::Pause_i(player.get()); }

    void SetVolume(const Ref<AudioPlayer>& player, float_t volume) { ir::SetVolume_i(player.get(), volume); }
    void SetPitch(const Ref<AudioPlayer>& player, float_t pitch) { ir::SetPitch_i(player.get(), pitch); }
    void SetLoopingMode(const Ref<AudioPlayer>& player, bool loopingEnabled) { ir::SetLoopingMode_i(player.get(), loopingEnabled); }

    void Seek(const Ref<AudioPlayer>& player, float_t positionInSeconds) { ir::Seek_i(player.get(), positionInSeconds); }
    void SeekPercent(const Ref<AudioPlayer>& player, float_t position) { ir::SeekPercent_i(player.get(), position); }

    float GetPitch(const Ref<AudioPlayer>& player) { return player->settings.pitch; }
    float GetVolume(const Ref<AudioPlayer>& player) { return player->settings.volume; }
    double GetPlayedInSeconds(const Ref<AudioPlayer>& player) { return ir::GetPlayedInSeconds_i(player.get()); }
    double GetPlayedPercent(const Ref<AudioPlayer>& player) { return ir::GetPlayedPercent_i(player.get()); }
    bool IsLoopingEnabled(const Ref<AudioPlayer>& player) { return player->settings.loopingEnabled; }
}