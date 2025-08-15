#ifndef HAUDIOIMPL_H
#define HAUDIOIMPL_H

#include "haudiogroup.h"
#include "audio/haudioclip.h"
#include "haudiointernal.h"

namespace hf::inter
{
    template<typename T>
    bool IsLoaded_i(T* player) { return (uint32_t)(player->stateFlags & AudioPlayerStateFlags::Loaded); }

    template<typename T>
    bool IsPlaying_i(T* player) { return (uint32_t)(player->stateFlags & AudioPlayerStateFlags::Playing); }

    template<typename T>
    void FreeHandle_i(T* player)
    {
        if (player->handle && player->clip)
        {
            const auto handle = (ma_sound*)player->handle;
            ma_sound_uninit(handle);
            player->clip = nullptr;
        }
    }

    template<typename T>
    bool ChangeClip_i(T* player, const Ref<AudioClip>& clip, uint32_t flags = 0, float_t startingDuration = -1)
    {
        FreeHandle_i(player);
        if (!clip) return false;

        const auto bufferConfig = ma_audio_buffer_config_init((ma_format)clip->format, clip->channels, clip->framesRead, clip->pcmData, nullptr);

        auto buffer = (ma_audio_buffer*)player->buffer;
        if (!buffer) buffer = new ma_audio_buffer();
        if (ma_audio_buffer_init(&bufferConfig, buffer) != MA_SUCCESS)
        {
            LOG_ERROR("Unable create player for the Audio clip -> %s", clip->filePath.c_str());
            return false;
        }

        auto handle = (ma_sound*)player->buffer;
        if (!handle) handle = new ma_sound();
        if (ma_sound_init_from_data_source(&AUDIO_DATA.engine, buffer,
            flags, (ma_sound_group*)player->parent->handle, handle) != MA_SUCCESS)
        {
            LOG_ERROR("Unable create player for the Audio clip -> %s", clip->filePath.c_str());
            return false;
        }

        player->buffer = buffer;
        player->handle = handle;
        player->clip = clip;

        auto& settings = player->settings;
        if (startingDuration >= 0)
        {
            const auto frame = (uint64_t)((double_t)clip->sampleRate * startingDuration);
            ma_sound_seek_to_pcm_frame(handle, frame);
        }

        ma_sound_set_volume(handle, settings.volume);
        ma_sound_set_pitch(handle, settings.pitch);
        ma_sound_set_looping(handle, settings.loopingEnabled);
        return true;
    }

    template<typename T>
    void Destructor_i(T* player)
    {
        if (player->handle)
        {
            const auto h = (ma_sound*)player->handle;
            delete h;
            player->handle = nullptr;
            player->clip = nullptr;
        }

        if (player->buffer)
        {
            const auto b = (ma_audio_buffer*)player->buffer;
            delete b;
            player->buffer = nullptr;
        }
        player->stateFlags = AudioPlayerStateFlags::None;
    }

    template<typename T>
    double_t GetPlayedInSeconds_i(T* player)
    {
        if (!player->clip) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot get the position without clip.");

        uint64_t position = 0;
        if(ma_sound_get_cursor_in_pcm_frames((ma_sound*)player->handle, (ma_uint64*)&position) != MA_SUCCESS)
            LOG_ERROR("Unable to get the audio position %llu", position);
        return (double_t)position / player->clip->sampleRate;
    }

    template<typename T>
    double_t GetPlayedPercent_i(T* player)
    {
        if (!player->clip) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot seek the player without clip.");
        const auto length = (double_t)player->clip->frameCount / (double_t)player->clip->sampleRate;
        return GetPlayedInSeconds_i(player) / length;
    }

    template<typename T>
    void Seek_i(T* player, float_t positionInSeconds)
    {
        if (!IsLoaded_i(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        if (!player->clip) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot seek the player without clip.");

        if (ma_sound_seek_to_second((ma_sound*)player->handle, positionInSeconds) != MA_SUCCESS)
            LOG_ERROR("Unable to seek audio to position %f", positionInSeconds);
    }

    template<typename T>
    void SeekPercent_i(T* player, float_t position)
    {
        if (!IsLoaded_i(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        if (!player->clip) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot seek the player without clip.");
        auto p = (uint64_t)((double_t)player->clip->frameCount * position);
        if (ma_sound_seek_to_pcm_frame((ma_sound*)player->handle, p) != MA_SUCCESS)
            LOG_ERROR("Unable to seek audio to position %f", p);
    }

    template<typename T>
    void Play_i(T* player)
    {
        if (!IsLoaded_i(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        player->stateFlags |= AudioPlayerStateFlags::Playing;
        if (ma_sound_start((ma_sound*)player->handle) != MA_SUCCESS)
            LOG_ERROR("Unable to play audio player");
    }

    template<typename T>
    void Pause_i(T* player)
    {
        if (!IsLoaded_i(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        player->stateFlags &= (AudioPlayerStateFlags)~(uint32_t)AudioPlayerStateFlags::Playing;
        if (ma_sound_stop((ma_sound*)player->handle) != MA_SUCCESS)
            LOG_ERROR("Unable to pause audio player");
    }

    template<typename T>
    void SetVolume_i(T* player, float_t volume)
    {
        if (!IsLoaded_i(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        auto& settings = player->settings;
        if (settings.volume != volume)
        {
            settings.volume = volume;
            ma_sound_set_volume((ma_sound*)player->handle, volume);
        }
    }

    template<typename T>
    void SetPitch_i(T* player, float_t pitch)
    {
        if (!IsLoaded_i(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        auto& settings = player->settings;
        if (settings.pitch != pitch)
        {
            settings.pitch = pitch;
            ma_sound_set_pitch((ma_sound*)player->handle, pitch);
        }
    }

    template<typename T>
    void SetLoopingMode_i(T* player, bool loopingEnabled)
    {
        if (!IsLoaded_i(player)) throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access destroyed audio player");
        auto& settings = player->settings;
        if (settings.loopingEnabled != loopingEnabled)
        {
            settings.loopingEnabled = loopingEnabled;
            ma_sound_set_looping((ma_sound*)player->handle, loopingEnabled);
        }
    }
}

#endif //HAUDIOIMPL_H
