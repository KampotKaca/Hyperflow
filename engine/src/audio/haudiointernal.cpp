#include "hshared.h"
#include "hinternal.h"
#include "haudiointernal.h"
#include "hyperflow.h"

namespace hf::ir
{
    AudioData AUDIO_DATA;

    namespace audio
    {
        void Load_i()
        {
            if (AUDIO_DATA.isLoaded) return;
            AUDIO_DATA.isLoaded = true;

            const ma_engine_config config
            {
                .listenerCount = HF.internalAudioInfo.usedListenersCount,
            };
            auto result = ma_engine_init(&config, &AUDIO_DATA.engine);

            if (result != MA_SUCCESS) throw GENERIC_EXCEPT("[Hyperflow]", "Unable to load audio engine!");

            ma_engine_set_volume(&AUDIO_DATA.engine, HF.internalAudioInfo.volume);

            AUDIO_DATA.group2D = Create(HF.internalAudioInfo.audio2DInfo);
            AUDIO_DATA.group3D = Create(HF.internalAudioInfo.audio3DInfo);
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
        if (ir::HF.internalAudioInfo.volume != volume)
        {
            ir::HF.internalAudioInfo.volume = volume;
            ma_engine_set_volume(&ir::AUDIO_DATA.engine, volume);
        }
    }

    float_t GetAudioVolume() { return ir::HF.internalAudioInfo.volume; }

    Ref<AudioGroup> Get2DAudioGroup() { return ir::AUDIO_DATA.group2D; }
    Ref<AudioGroup> Get3DAudioGroup() { return ir::AUDIO_DATA.group3D; }
}