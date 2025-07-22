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
                .settings = {}
            };
            APP_AUDIOS.cartoon_comedy = hf::Create(info);
        }

        {
            const hf::AudioPlayerCreationInfo info
            {
                .clip = APP_AUDIOS.cartoon_comedy,
                .settings =
                {
                    .loopingEnabled = true
                }
            };
            APP_AUDIOS.background_music = hf::Create(info);
        }

        {
            const hf::AudioPlayer3DCreationInfo info
            {
                .clip = APP_AUDIOS.cartoon_comedy,
                .settings =
                {

                }
            };
            APP_AUDIOS.background_music3D = hf::Create(info);
        }

        {
            const hf::AudioListenerCreationInfo info{};
            APP_AUDIOS.main_listener = hf::Create(info);
        }
    }
}