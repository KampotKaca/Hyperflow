#include "audio/haudioplayer3d.h"
#include "hinternal.h"
#include "audio/haudioimpl.h"
#include "hyperflow.h"

namespace hf
{
    namespace ir
    {
        template bool IsLoaded_i(AudioPlayer3D* player);
        template bool IsPlaying_i(AudioPlayer3D* player);

        template void FreeHandle_i(AudioPlayer3D* player);
        template bool ChangeClip_i(AudioPlayer3D* player, const Ref<AudioClip>& clip, uint32_t flags, float_t startingDuration);
        template void Destructor_i(AudioPlayer3D* player);
        template double_t GetPlayedInSeconds_i(AudioPlayer3D* player);
        template double_t GetPlayedPercent_i(AudioPlayer3D* player);
        template void Seek_i(AudioPlayer3D* player, float_t positionInSeconds);
        template void SeekPercent_i(AudioPlayer3D* player, float_t positionInSeconds);
        template void Play_i(AudioPlayer3D* player);
        template void Pause_i(AudioPlayer3D* player);

        template void SetVolume_i(AudioPlayer3D* player, float_t volume);
        template void SetPitch_i(AudioPlayer3D* player, float_t pitch);
        template void SetLoopingMode_i(AudioPlayer3D* player, bool loopingEnabled);
    }

    static void ChangeClip(AudioPlayer3D* player, const Ref<AudioClip>& clip, float_t startingDuration);

    AudioPlayer3D::AudioPlayer3D(const AudioPlayer3DCreationInfo& info)
    : settings(info.settings), settings3d(info.settings3d), cone(info.cone), parent(info.parent)
    {
        if (!parent) parent = ir::AUDIO_DATA.group3D;
        ChangeClip(this, info.clip, -1);
    }

    AudioPlayer3D::~AudioPlayer3D()
    {
        ir::FreeHandle_i(this);
        ir::Destructor_i(this);
    }

    bool IsLoaded(const Ref<AudioPlayer3D>& player) { return ir::IsLoaded_i(player.get()); }
    bool IsPlaying(const Ref<AudioPlayer3D>& player) { return ir::IsPlaying_i(player.get()); }

    Ref<AudioPlayer3D> Create(const AudioPlayer3DCreationInfo& info)
    {
        Ref<AudioPlayer3D> player = MakeRef<AudioPlayer3D>(info);
        ir::HF.audioResources.player3Ds[(uint64_t)player.get()] = player;
        player->stateFlags |= AudioPlayerStateFlags::Loaded;
        return player;
    }

    void Destroy(const Ref<AudioPlayer3D>& player)
    {
        hassert(IsLoaded(player), "[Hyperflow] Trying to access destroyed audio player");
        ir::FreeHandle_i(player.get());
        ir::HF.audioResources.player3Ds.erase((uint64_t)player.get());
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

    void ChangeClip(const Ref<AudioPlayer3D>& player, const Ref<AudioClip>& clip, float_t startingDuration)
    {
        hassert(IsLoaded(player), "[Hyperflow] Trying to access destroyed audio player");
        ChangeClip(player.get(), clip, startingDuration);

        if (IsPlaying(player) && ma_sound_start((ma_sound*)player->handle) != MA_SUCCESS)
            log_error("Unable to play audio player");
    }

    void Play(const Ref<AudioPlayer3D>& player) { ir::Play_i(player.get()); }
    void Pause(const Ref<AudioPlayer3D>& player) { ir::Pause_i(player.get()); }

    void SetRange(const Ref<AudioPlayer3D>& player, float_t maxRange, float_t falloff)
    {
        hassert(IsLoaded(player), "[Hyperflow] Trying to access destroyed audio player");
        auto& settings = player->settings3d;
        falloff = glm::min(maxRange, falloff);
        if (settings.maxRange != maxRange)
        {
            settings.maxRange = maxRange;
            ma_sound_set_max_distance((ma_sound*)player->handle, maxRange);
        }

        if (settings.falloff != falloff)
        {
            settings.falloff = falloff;
            float_t minDistance = glm::max(0.001f, maxRange - falloff);
            ma_sound_set_min_distance((ma_sound*)player->handle, minDistance);
        }
    }

    void SetAttenuationModel(const Ref<AudioPlayer3D>& player, Audio3DAttenuationModel atten)
    {
        hassert(IsLoaded(player), "[Hyperflow] Trying to access destroyed audio player");
        auto& config = player->settings3d;
        if (config.attenuationModel != atten)
        {
            config.attenuationModel = atten;
            ma_sound_set_attenuation_model((ma_sound*)player->handle, (ma_attenuation_model)atten);
        }
    }

    void SetVolume(const Ref<AudioPlayer3D>& player, float_t volume) { ir::SetVolume_i(player.get(), volume); }
    void SetPitch(const Ref<AudioPlayer3D>& player, float_t pitch) { ir::SetPitch_i(player.get(), pitch); }
    void SetLoopingMode(const Ref<AudioPlayer3D>& player, bool loopingEnabled) { ir::SetLoopingMode_i(player.get(), loopingEnabled); }

    vec2 GetRange(const Ref<AudioPlayer3D>& player) { return {player->settings3d.maxRange, player->settings3d.falloff}; }
    Audio3DAttenuationModel GetAttenuationModel(const Ref<AudioPlayer3D>& player) { return player->settings3d.attenuationModel; }

    void Seek(const Ref<AudioPlayer3D>& player, float_t positionInSeconds) { ir::Seek_i(player.get(), positionInSeconds); }
    void SeekPercent(const Ref<AudioPlayer3D>& player, float_t position) { ir::SeekPercent_i(player.get(), position); }

    float_t GetPitch(const Ref<AudioPlayer3D>& player) { return player->settings.pitch; }
    float_t GetVolume(const Ref<AudioPlayer3D>& player) { return player->settings.volume; }
    bool IsLoopingEnabled(const Ref<AudioPlayer3D>& player) { return player->settings.loopingEnabled; }

    double_t GetPlayedInSeconds(const Ref<AudioPlayer3D>& player) { return ir::GetPlayedInSeconds_i(player.get()); }
    double_t GetPlayedPercent(const Ref<AudioPlayer3D>& player) { return ir::GetPlayedPercent_i(player.get()); }

    void ChangeClip(AudioPlayer3D* player, const Ref<AudioClip>& clip, float_t startingDuration)
    {
        if(!ir::ChangeClip_i(player, clip, MA_SOUND_FLAG_DECODE, startingDuration)) return;

        auto& settings3d = player->settings3d;
        auto handle = (ma_sound*)player->handle;
        ma_sound_set_max_distance(handle, settings3d.maxRange);
        ma_sound_set_min_distance(handle, settings3d.maxRange - settings3d.falloff);
        ma_sound_set_attenuation_model(handle, (ma_attenuation_model)settings3d.attenuationModel);
        ma_sound_set_spatialization_enabled(handle, true);
    }

    void Set(const Ref<AudioPlayer3D>& pl, const AudioCone& cone)
    {
        hassert(IsLoaded(pl), "[Hyperflow] Trying to access destroyed audio player");

        auto handle = (ma_sound*)pl->handle;
        auto coneRef = pl->cone;
        if (cone.innerAngle != coneRef.innerAngle || cone.outerAngle != coneRef.outerAngle || cone.outerGain != coneRef.outerGain)
            ma_sound_set_cone(handle, cone.innerAngle, cone.outerAngle, cone.outerGain);

        if (cone.position != coneRef.position) ma_sound_set_position(handle, cone.position.x, cone.position.y, cone.position.z);

        if (cone.euler != coneRef.euler)
        {
            vec3 direction = vec3(glm::eulerAngleXYZ(cone.euler.x, cone.euler.y, cone.euler.z) * vec4(0, 0, 1, 0));
            ma_sound_set_direction(handle, direction.x, direction.y, direction.z);
        }

        pl->cone = cone;
    }

    void SetVelocity(const Ref<AudioPlayer3D>& pl, vec3 velocity)
    {
        hassert(IsLoaded(pl), "[Hyperflow] Trying to access destroyed audio player");

        if (velocity != pl->velocity)
        {
            pl->velocity = velocity;
            ma_sound_set_velocity((ma_sound*)pl->handle, velocity.x, velocity.y, velocity.z);
        }
    }

    vec3 GetVelocity(const Ref<AudioPlayer3D>& pl) { return pl->velocity; }
    AudioCone GetCone(const Ref<AudioPlayer3D>& pl) { return pl->cone; }
}