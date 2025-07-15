#ifndef HAUDIOCLIP_H
#define HAUDIOCLIP_H

#include "hshared.h"

namespace hf
{
    struct AudioClip
    {
        explicit AudioClip(const AudioClipCreationInfo& info);
        ~AudioClip();

        std::string filePath{};
        bool useAbsolutePath{};
        AudioClipConfig config{};

        void* pcmData{};

        uint64_t frameCount{};
        uint64_t framesRead{};
        uint32_t channels{};
        uint32_t format{};
        uint32_t sampleRate{};
    };
}

#endif //HAUDIOCLIP_H
