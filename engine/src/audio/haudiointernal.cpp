#include "hshared.h"
#include "hinternal.h"
#include "haudiointernal.h"
#include "hyperflow.h"

namespace hf::inter
{
    AudioData AUDIO_DATA;

    namespace audio
    {
        void Load_i()
        {
            const ma_engine_config config
            {
                .listenerCount = HF.internalAudioInfo.usedListenersCount,
            };
            auto result = ma_engine_init(&config, &AUDIO_DATA.engine);

            if (result != MA_SUCCESS) throw GENERIC_EXCEPT("[Hyperflow]", "Unable to load audio engine!");

            ma_engine_set_volume(&AUDIO_DATA.engine, HF.internalAudioInfo.volume);
        }

        void Unload_i()
        {
            ma_engine_uninit(&AUDIO_DATA.engine);
        }
    }
}

namespace hf
{
    void SetAudioVolume(float_t volume)
    {
        if (inter::HF.internalAudioInfo.volume != volume)
        {
            inter::HF.internalAudioInfo.volume = volume;
            ma_engine_set_volume(&inter::AUDIO_DATA.engine, volume);
        }
    }

    float_t GetAudioVolume() { return inter::HF.internalAudioInfo.volume; }
}