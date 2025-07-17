#include "audio/haudioplayer3d.h"
#include "haudiointernal.h"
#include "hinternal.h"

namespace hf
{
    namespace inter
    {
        template bool IsLoaded_i(AudioPlayer3D* player);
        template bool IsPlaying_i(AudioPlayer3D* player);

        template void FreeHandle_i(AudioPlayer3D* player);
        template bool ChangeClip_i(AudioPlayer3D* player, const Ref<AudioClip>& clip, double_t startingDuration);
        template void Destructor_i(AudioPlayer3D* player);
        template double_t GetPlayedInSeconds_i(AudioPlayer3D* player);
        template double_t GetPlayedPercent_i(AudioPlayer3D* player);
        template void Seek_i(AudioPlayer3D* player, double_t positionInSeconds);
        template void SeekPercent_i(AudioPlayer3D* player, double_t positionInSeconds);
        template void Play_i(AudioPlayer3D* player);
        template void Pause_i(AudioPlayer3D* player);
    }

    static void ChangeClip(AudioPlayer3D* player, const Ref<AudioClip>& clip, double_t startingDuration);

    AudioPlayer3D::AudioPlayer3D(const AudioPlayer3DCreationInfo& info) : config(info.config)
    {
        ChangeClip(this, info.clip, -1);
    }

    AudioPlayer3D::~AudioPlayer3D()
    {
        inter::FreeHandle_i(this);
        inter::Destructor_i(this);
    }

    bool IsLoaded(const Ref<AudioPlayer3D>& player) { return inter::IsLoaded_i(player.get()); }
    bool IsPlaying(const Ref<AudioPlayer3D>& player) { return inter::IsPlaying_i(player.get()); }

    Ref<AudioPlayer3D> Create(const AudioPlayer3DCreationInfo& info)
    {
        Ref<AudioPlayer3D> player = MakeRef<AudioPlayer3D>(info);
        inter::HF.audioResources.player3Ds[(uint64_t)player.get()] = player;
        player->stateFlags |= AudioPlayerStateFlags::Loaded;
        return player;
    }

    void Destroy(const Ref<AudioPlayer3D>& player)
    {
        if (!IsLoaded(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");

        inter::FreeHandle_i(player.get());
        inter::HF.audioResources.player3Ds.erase((uint64_t)player.get());
        player->stateFlags &= (AudioPlayerStateFlags)~(uint32_t)AudioPlayerStateFlags::Loaded;
    }

    void Destroy(const Ref<AudioPlayer3D>* pPlayers, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto player = pPlayers[i];
            Destroy(player);
        }
    }

    void ChangeClip(const Ref<AudioPlayer3D>& player, const Ref<AudioClip>& clip, double_t startingDuration)
    {
        if (!IsLoaded(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        ChangeClip(player.get(), clip, startingDuration);

        if (IsPlaying(player) && ma_sound_start((ma_sound*)player->handle) != MA_SUCCESS)
            LOG_ERROR("Unable to play audio player");
    }

    void Play(const Ref<AudioPlayer3D>& player) { inter::Play_i(player.get()); }
    void Pause(const Ref<AudioPlayer3D>& player) { inter::Pause_i(player.get()); }

    void SetMinDistance(const Ref<AudioPlayer3D>& player, float_t minDistance)
    {
        SET_AUDIO_STAT(player, minDistance, ma_sound_set_min_distance)
    }

    void SetMaxDistance(const Ref<AudioPlayer3D>& player, float_t maxDistance)
    {
        SET_AUDIO_STAT(player, maxDistance, ma_sound_set_max_distance)
    }

    void SetAttenuationModel(const Ref<AudioPlayer3D>& player, Audio3DAttenuationModel atten)
    {
        if (!IsLoaded(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        auto& config = player->config;
        if (config.attenuationModel != atten)
        {
            config.attenuationModel = atten;
            ma_sound_set_attenuation_model((ma_sound*)player->handle, (ma_attenuation_model)atten);
        }
    }

#define SET_V3_STAT(pl, d, f)\
    if (!IsLoaded(pl)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");\
    auto& config = pl->config;\
    if (config.d != d)\
    {\
        config.d = d;\
        f((ma_sound*)pl->handle, d.x, d.y, d.z);\
    }

    void SetPosition(const Ref<AudioPlayer3D>& player, vec3 position)
    {
        SET_V3_STAT(player, position, ma_sound_set_position)
    }

    void SetDirection(const Ref<AudioPlayer3D>& player, vec3 direction)
    {
        SET_V3_STAT(player, direction, ma_sound_set_direction)
    }

    void SetVolume(const Ref<AudioPlayer3D>& player, float_t volume)
    {
        SET_AUDIO_STAT(player, volume, ma_sound_set_volume)
    }

    void SetPitch(const Ref<AudioPlayer3D>& player, float_t pitch)
    {
        SET_AUDIO_STAT(player, pitch, ma_sound_set_pitch)
    }

    void SetLoopingMode(const Ref<AudioPlayer3D>& player, bool loopingEnabled)
    {
        SET_AUDIO_STAT(player, loopingEnabled, ma_sound_set_looping)
    }

    float_t GetMinDistance(const Ref<AudioPlayer3D>& player) { return player->config.minDistance; }
    float_t GetMaxDistance(const Ref<AudioPlayer3D>& player) { return player->config.maxDistance; }
    Audio3DAttenuationModel GetAttenuationModel(const Ref<AudioPlayer3D>& player) { return player->config.attenuationModel; }
    vec3 GetPosition(const Ref<AudioPlayer3D>& player) { return player->config.position; }
    vec3 GetDirection(const Ref<AudioPlayer3D>& player) { return player->config.direction; }

    void Seek(const Ref<AudioPlayer3D>& player, double_t positionInSeconds) { inter::Seek_i(player.get(), positionInSeconds); }
    void SeekPercent(const Ref<AudioPlayer3D>& player, double_t position) { inter::SeekPercent_i(player.get(), position); }

    float_t GetPitch(const Ref<AudioPlayer3D>& player) { return player->config.pitch; }
    float_t GetVolume(const Ref<AudioPlayer3D>& player) { return player->config.volume; }
    bool IsLoopingEnabled(const Ref<AudioPlayer3D>& player) { return player->config.loopingEnabled; }

    double_t GetPlayedInSeconds(const Ref<AudioPlayer3D>& player) { return inter::GetPlayedInSeconds_i(player.get()); }
    double_t GetPlayedPercent(const Ref<AudioPlayer3D>& player) { return inter::GetPlayedPercent_i(player.get()); }

    void ChangeClip(AudioPlayer3D* player, const Ref<AudioClip>& clip, double_t startingDuration)
    {
        if(!inter::ChangeClip_i(player, clip, startingDuration)) return;

        auto& config = player->config;
        auto handle = (ma_sound*)player->handle;
        ma_sound_set_min_distance(handle, config.minDistance);
        ma_sound_set_max_distance(handle, config.maxDistance);
        ma_sound_set_attenuation_model(handle, (ma_attenuation_model)config.attenuationModel);
        ma_sound_set_position(handle, config.position.x, config.position.y, config.position.z);
        ma_sound_set_direction(handle, config.direction.x, config.direction.y, config.direction.z);
    }
}