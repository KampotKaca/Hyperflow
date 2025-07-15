#include "audio/haudioplayer.h"
#include "haudiointernal.h"
#include "hinternal.h"

namespace hf
{
    static void FreeHandle(AudioPlayer* player);
    static void ChangeClip(AudioPlayer* player, const Ref<AudioClip>& clip, double startingDuration = -1);

    AudioPlayer::AudioPlayer(const AudioPlayerCreationInfo& info)
        : config(info.config), clip(info.clip)
    {
        if (clip) ChangeClip(this, clip);
    }

    AudioPlayer::~AudioPlayer()
    {
        FreeHandle(this);
        isLoaded = false;
    }

    Ref<AudioPlayer> Create(const AudioPlayerCreationInfo& info)
    {
        Ref<AudioPlayer> player = MakeRef<AudioPlayer>(info);
        inter::HF.audioResources.players[(uint64_t)player.get()] = player;
        player->isLoaded = true;
        return player;
    }

    void Destroy(const Ref<AudioPlayer>& player)
    {
        if (!player->isLoaded) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");

        FreeHandle(player.get());
        inter::HF.audioResources.players.erase((uint64_t)player.get());
        player->isLoaded = false;
    }

    void Destroy(const Ref<AudioPlayer>* pPlayers, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto player = pPlayers[i];
            Destroy(player);
        }
    }

    bool IsLoaded(const Ref<AudioPlayer>& player) { return player->isLoaded; }

    void ChangeClip(const Ref<AudioPlayer>& player, const Ref<AudioClip>& clip, int64_t startingFrame)
    {
        if (!player->isLoaded) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        ChangeClip(player.get(), clip, startingFrame);

        if (player->isPlaying && ma_sound_start((ma_sound*)player->handle) != MA_SUCCESS)
            LOG_ERROR("Unable to play audio player");
    }
    void Play(const Ref<AudioPlayer>& player)
    {
        if (!player->isLoaded) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        player->isPlaying = true;
        if (inter::AUDIO_DATA.isEnabled && ma_sound_start((ma_sound*)player->handle) != MA_SUCCESS)
            LOG_ERROR("Unable to play audio player");
    }
    void Pause(const Ref<AudioPlayer>& player)
    {
        if (!player->isLoaded) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        player->isPlaying = false;
        if (inter::AUDIO_DATA.isEnabled && ma_sound_stop((ma_sound*)player->handle) != MA_SUCCESS)
            LOG_ERROR("Unable to pause audio player");
    }

    void SetVolume(const Ref<AudioPlayer>& player, float volume)
    {
        if (!player->isLoaded) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        auto& config = player->config;
        if (config.volume != volume)
        {
            config.volume = volume;
            ma_sound_set_volume((ma_sound*)player->handle, volume);
        }
    }

    void SetPitch(const Ref<AudioPlayer>& player, float pitch)
    {
        if (!player->isLoaded) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        auto& config = player->config;
        if (config.pitch != pitch)
        {
            config.pitch = pitch;
            ma_sound_set_pitch((ma_sound*)player->handle, pitch);
        }
    }

    void SetLoopingMode(const Ref<AudioPlayer>& player, bool enableLoop)
    {
        if (!player->isLoaded) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        auto& config = player->config;
        if (config.loopingEnabled != enableLoop)
        {
            config.loopingEnabled = enableLoop;
            ma_sound_set_looping((ma_sound*)player->handle, enableLoop);
        }
    }

    void Seek(const Ref<AudioPlayer>& player, double positionInSeconds)
    {
        if (!player->isLoaded) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        if (!player->clip) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot seek the player without clip.");

        if (ma_sound_seek_to_second((ma_sound*)player->handle, positionInSeconds) != MA_SUCCESS)
            LOG_ERROR("Unable to seek audio to position %f", positionInSeconds);
    }

    void SeekPercent(const Ref<AudioPlayer>& player, double position)
    {
        if (!player->isLoaded) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        if (!player->clip) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot seek the player without clip.");
        double length = player->clip->frameCount / (double)player->clip->sampleRate;
        double positionInSeconds = position * length;
        if (ma_sound_seek_to_second((ma_sound*)player->handle, positionInSeconds) != MA_SUCCESS)
            LOG_ERROR("Unable to seek audio to position %f", positionInSeconds);
    }

    void SetConfig(const Ref<AudioPlayer>& player, const AudioPlayerConfig& config)
    {
        if (!player->isLoaded) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        SetVolume(player, config.volume);
        SetPitch(player, config.pitch);
        SetLoopingMode(player, config.loopingEnabled);
    }

    const AudioPlayerConfig& GetConfig(const Ref<AudioPlayer>& player) { return player->config; }
    float GetPitch(const Ref<AudioPlayer>& player) { return player->config.pitch; }
    float GetVolume(const Ref<AudioPlayer>& player) { return player->config.volume; }
    double GetPositionInSeconds(const Ref<AudioPlayer>& player)
    {
        if (!player->clip) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot get the position without clip.");

        uint64_t position = 0;
        if(ma_sound_get_cursor_in_pcm_frames((ma_sound*)player->handle, &position) != MA_SUCCESS)
            LOG_ERROR("Unable to get the audio position %llu", position);
        return position / player->clip->sampleRate;
    }
    double GetPositionPercent(const Ref<AudioPlayer>& player)
    {
        if (!player->clip) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot seek the player without clip.");
        double length = player->clip->frameCount / (double)player->clip->sampleRate;
        return GetPositionInSeconds(player) / length;
    }
    bool IsLoopingEnabled(const Ref<AudioPlayer>& player) { return player->config.loopingEnabled; }
    bool IsPlaying(const Ref<AudioPlayer>& player) { return player->isPlaying; }
    void FreeHandle(AudioPlayer* player)
    {
        if (player->handle)
        {
            auto handle = (ma_sound*)player->handle;
            ma_sound_uninit(handle);
            delete handle;
            player->handle = nullptr;
            player->clip = nullptr;
        }
    }

    void ChangeClip(AudioPlayer* player, const Ref<AudioClip>& clip, double startingDuration)
    {
        FreeHandle(player);

        player->handle = new ma_sound();
        auto handle = (ma_sound*)player->handle;
        auto result = ma_sound_init_from_data_source(&inter::AUDIO_DATA.engine,
            (ma_data_source*)clip->buffer, 0, nullptr, handle);
        if (result != MA_SUCCESS)
        {
            delete handle;
            player->handle = nullptr;
            LOG_ERROR("Unable create player for the Audio clip -> %s", clip->filePath.c_str());
        }

        auto& config = player->config;
        if (startingDuration >= 0)
        {
            uint64_t frame = (uint64_t)(clip->sampleRate * startingDuration);
            ma_sound_seek_to_pcm_frame(handle, frame);
        }

        ma_sound_set_volume(handle, config.volume);
        ma_sound_set_pitch(handle, config.pitch);
        ma_sound_set_looping(handle, config.loopingEnabled);
    }
}