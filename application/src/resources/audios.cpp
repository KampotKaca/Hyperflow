#include "resources/audios.h"

namespace app
{
    AppAudios APP_AUDIOS;

    void AudioLoadAll()
    {
        {
            const hf::AudioClipCreationInfo info
            {
                .filePath = "cartoon_comedy.mp3",
                .useAbsolutePath = false,
                .config = {}
            };
            APP_AUDIOS.cartoon_comedy = hf::Create(info);
        }

        {
            const hf::AudioPlayerCreationInfo info
            {
                .clip = APP_AUDIOS.cartoon_comedy,
                .config =
                {
                    .loopingEnabled = true
                }
            };
            APP_AUDIOS.background_music = hf::Create(info);
            // hf::Play(APP_AUDIOS.background_music);
        }
    }
}