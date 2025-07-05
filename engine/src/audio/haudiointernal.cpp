#include "hshared.h"
#include "hinternal.h"
#include "haudiointernal.h"
#include "audio/haudioplayer.h"

namespace hf::inter
{
    AudioData AUDIO_DATA;

    namespace audio
    {
        void Load_i(bool isEnabled, float volume)
        {
            const ma_engine_config config{};
            ma_result result = ma_engine_init(&config, &AUDIO_DATA.engine);

            if (result != MA_SUCCESS)
                throw GENERIC_EXCEPT("[Hyperflow]", "Unable to load audio engine!");

            AUDIO_DATA.isEnabled = isEnabled;
            AUDIO_DATA.volume = volume;
            ma_engine_set_volume(&AUDIO_DATA.engine, volume);
        }

        void Unload_i()
        {
            ma_engine_uninit(&AUDIO_DATA.engine);
        }
    }
}

namespace hf
{
    void SetVolume(float volume)
    {
        if (inter::AUDIO_DATA.volume != volume)
        {
            inter::AUDIO_DATA.volume = volume;
            ma_engine_set_volume(&inter::AUDIO_DATA.engine, volume);
        }
    }

    void EnableAudio(bool isEnabled)
    {
        if (inter::AUDIO_DATA.isEnabled != isEnabled)
        {
            inter::AUDIO_DATA.isEnabled = isEnabled;

            if (isEnabled)
            {
                for (auto player : inter::HF.audioResources.players | std::views::values)
                    if (player->isPlaying) ma_sound_start((ma_sound*)player->handle);
            }
            else
            {
                for (auto player : inter::HF.audioResources.players | std::views::values)
                    if (player->isPlaying) ma_sound_stop((ma_sound*)player->handle);
            }

        }
    }
}