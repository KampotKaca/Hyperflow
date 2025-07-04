#include "hshared.h"
#include "hinternal.h"
#include "haudiointernal.h"

namespace hf::inter
{
    AudioData AUDIO_DATA;

    namespace audio
    {
        void Load_i()
        {
            const ma_engine_config config{};
            ma_result result = ma_engine_init(&config, &AUDIO_DATA.engine);

            if (result != MA_SUCCESS)
                throw GENERIC_EXCEPT("[Hyperflow]", "Unable to load audio engine!");
        }

        void Unload_i()
        {
            ma_engine_uninit(&AUDIO_DATA.engine);
        }
    }
}