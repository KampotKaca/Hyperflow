#include "hshared.h"
#include "hinternal.h"
#include "haudiointernal.h"
#include "audio/haudioplayer.h"

namespace hf::inter
{
    AudioData AUDIO_DATA;

    namespace audio
    {
        void Load_i(float_t volume)
        {
            constexpr ma_engine_config config{};
            auto result = ma_engine_init(&config, &AUDIO_DATA.engine);

            if (result != MA_SUCCESS) throw GENERIC_EXCEPT("[Hyperflow]", "Unable to load audio engine!");

            ma_engine_set_volume(&AUDIO_DATA.engine, volume);
        }

        void Unload_i()
        {
            ma_engine_uninit(&AUDIO_DATA.engine);
        }
    }
}