#include "audio/haudioclip.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "haudiointernal.h"

namespace hf
{
#define CHECK(x, s)\
    {\
        auto r = (x);\
        if (r != MA_SUCCESS)\
        {\
            LOG_ERROR("[Hyperflow] Unable to load the audio, Error Code: %i\n[File] %s", r, s);\
            return false;\
        }\
    }

    AudioClip::AudioClip(const AudioClipCreationInfo& info)
    : filePath(info.filePath), useAbsolutePath(info.useAbsolutePath), settings(info.settings)
    {
        if(!inter::audio::CreateClip_i(this)) inter::audio::DestroyClip_i(this);
    }

    AudioClip::~AudioClip()
    {
        inter::audio::DestroyClip_i(this);
    }

    bool IsLoaded(const Ref<AudioClip>& clip) { return clip->pcmData; }

    Ref<AudioClip> Create(const AudioClipCreationInfo& info)
    {
        auto audio = MakeRef<AudioClip>(info);
        inter::HF.audioResources.clips[info.filePath] = audio;
        return audio;
    }

    void Destroy(const Ref<AudioClip>& clip)
    {
        if (inter::audio::DestroyClip_i(clip.get()))
            inter::HF.audioResources.clips.erase(clip->filePath);
    }

    void Destroy(const Ref<AudioClip>* pClips, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto clip = pClips[i];
            if (inter::audio::DestroyClip_i(clip.get()))
                inter::HF.audioResources.clips.erase(clip->filePath);
        }
    }

    uint64_t GetFrameCount(const Ref<AudioClip>& clip) { return clip->frameCount; }
    uint32_t GetChannels(const Ref<AudioClip>& clip) { return clip->channels; }
    uint32_t GetSampleRate(const Ref<AudioClip>& clip) { return clip->sampleRate; }

    namespace inter::audio
    {
        bool CreateClip_i(AudioClip* clip)
        {
            if (clip->pcmData) return false;

            std::string audioLoc{};

            if (clip->useAbsolutePath) audioLoc = clip->filePath;
            else audioLoc = TO_RES_PATH(std::string("audio/") + clip->filePath);

            if (!utils::FileExists(audioLoc.c_str()))
            {
                LOG_ERROR("[Hyperflow] Unable to find Audio clip: %s", clip->filePath.c_str());
                return false;
            }

            auto settings = clip->settings;
            const ma_decoder_config decoderConfig
            {
                .format = (ma_format)settings.format,
                .sampleRate = settings.sampleRate,
                .channelMixMode = (ma_channel_mix_mode)settings.channelMixMode,
                .ditherMode = (ma_dither_mode)settings.ditherMode,
                .encodingFormat = (ma_encoding_format)settings.encodingFormat,
            };

            ma_decoder decoder;
            CHECK(ma_decoder_init_file(audioLoc.c_str(), &decoderConfig, &decoder), clip->filePath.c_str());

            CHECK(ma_decoder_get_length_in_pcm_frames(&decoder, (ma_uint64*)&clip->frameCount), clip->filePath.c_str())

            clip->channels = decoder.outputChannels;
            clip->sampleRate = decoder.outputSampleRate;
            clip->format = decoder.outputFormat;

            clip->pcmData = utils::Allocate(clip->frameCount * decoder.outputChannels * sizeof(float));

            ma_uint64 framesRead;
            CHECK(ma_decoder_read_pcm_frames(&decoder, clip->pcmData, clip->frameCount, &framesRead), clip->filePath.c_str())
            ma_decoder_uninit(&decoder);

            clip->framesRead = framesRead;
            return true;
        }

        bool DestroyClip_i(AudioClip* clip)
        {
            if (clip->pcmData)
            {
                utils::Deallocate(clip->pcmData);
                clip->pcmData = nullptr;
                return true;
            }
            return false;
        }
    }
}