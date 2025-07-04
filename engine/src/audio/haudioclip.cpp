#include "audio/haudioclip.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "haudiointernal.h"
#include "miniaudio.h"

namespace hf
{
#define CHECK(x, s) if ((x) != MA_SUCCESS) { LOG_ERROR("[Hyperflow] Unable to load the audio\n[File] %s", s); return false; }

    AudioClip::AudioClip(const AudioClipCreationInfo& info)
    : filePath(info.filePath), useAbsolutePath(info.useAbsolutePath), config(info.config)
    {
        inter::audio::CreateClip_i(this);
    }

    AudioClip::~AudioClip()
    {
        inter::audio::DestroyClip_i(this);
    }

    bool IsLoaded(const Ref<AudioClip>& clip) { return clip->buffer; }

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

    namespace inter::audio
    {
        bool CreateClip_i(AudioClip* clip)
        {
            if (clip->buffer) return false;

            std::string audioLoc{};

            if (clip->useAbsolutePath) audioLoc = clip->filePath;
            else audioLoc = TO_RES_PATH(std::string("audio/clips/") + clip->filePath);

            if (!utils::FileExists(audioLoc.c_str()))
            {
                LOG_ERROR("[Hyperflow] Unable to find Audio clip: %s", clip->filePath.c_str());
                return false;
            }

            auto config = clip->config;
            const ma_decoder_config decoderConfig
            {
                .format = (ma_format)config.format,
                .sampleRate = config.sampleRate,
                .channelMixMode = (ma_channel_mix_mode)config.channelMixMode,
                .ditherMode = (ma_dither_mode)config.ditherMode,
                .encodingFormat = (ma_encoding_format)config.encodingFormat,
            };

            ma_decoder decoder;
            CHECK(ma_decoder_init_file(audioLoc.c_str(), &decoderConfig, &decoder), clip->filePath.c_str());

            CHECK(ma_decoder_get_length_in_pcm_frames(&decoder, &clip->frameCount), clip->filePath.c_str())

            clip->channels = decoder.outputChannels;
            clip->sampleRate = decoder.outputSampleRate;

            clip->buffer = utils::Allocate(clip->frameCount * decoder.outputChannels * sizeof(float));

            ma_uint64 framesRead;
            CHECK(ma_decoder_read_pcm_frames(&decoder, clip->buffer, clip->frameCount, &framesRead), clip->filePath.c_str())
            ma_decoder_uninit(&decoder);

            auto bufferConfig = ma_audio_buffer_config_init(ma_format_f32, clip->channels, framesRead, clip->buffer, nullptr);
            CHECK(ma_audio_buffer_init(&bufferConfig, (ma_audio_buffer*)clip->sharedBuffer), clip->filePath.c_str())

            return true;
        }

        bool DestroyClip_i(AudioClip* clip)
        {
            if (clip->buffer)
            {
                ma_audio_buffer_uninit((ma_audio_buffer*)clip->sharedBuffer);
                utils::Deallocate(clip->buffer);
                clip->buffer = nullptr;
                return true;
            }
            return false;
        }
    }
}