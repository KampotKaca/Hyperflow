#include "hyaml.h"
#include "audio/haudioclip.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "haudiointernal.h"
#include "hstrconversion.h"

namespace hf
{
    AudioClip::AudioClip(const AudioClipCreationInfo& info) : filePath(info.filePath), settings(info.settings)
    {
        std::string audioLoc{};

        if (filePath.isAbsolute) audioLoc = filePath.path;
        else audioLoc = TO_RES_PATH(std::string("audio/") + filePath.path);

        if (!utils::FileExists(audioLoc.c_str()))
        {
            log_error("[Hyperflow] Unable to find Audio clip: %s", filePath.path.c_str());
            return;
        }

        const ma_decoder_config decoderConfig
        {
            .format = (ma_format)settings.format,
            .sampleRate = settings.sampleRate,
            .channelMixMode = (ma_channel_mix_mode)settings.channelMixMode,
            .ditherMode = (ma_dither_mode)settings.ditherMode,
        };

        ma_decoder decoder;
        if (ma_decoder_init_file(audioLoc.c_str(), &decoderConfig, &decoder) != MA_SUCCESS)
        {
            log_error("[Hyperflow] Unable to load audio clip: %s", filePath.path.c_str());
            return;
        }
        if (ma_decoder_get_length_in_pcm_frames(&decoder, (ma_uint64*)&frameCount) != MA_SUCCESS)
        {
            log_error("[Hyperflow] Unable to get length of the audio clip: %s", filePath.path.c_str());
            return;
        }

        channels = decoder.outputChannels;
        sampleRate = decoder.outputSampleRate;
        format = decoder.outputFormat;

        pcmData = utils::Alloc(frameCount * decoder.outputChannels * sizeof(float_t));

        ma_uint64 fRead;
        if (ma_decoder_read_pcm_frames(&decoder, pcmData, frameCount, &fRead) != MA_SUCCESS)
        {
            log_error("[Hyperflow] Unable to audio clip frames: %s", filePath.path.c_str());
            return;
        }
        ma_decoder_uninit(&decoder);
        framesRead = fRead;
    }

    AudioClip::~AudioClip()
    {
        ir::audio::DestroyAudioClip_i(this);
    }

    void Destroy(const Ref<AudioClip>& clip)
    {
        ir::audio::DestroyAudioClip_i(clip.get());
    }

    void Destroy(const Ref<AudioClip>* pClips, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++) ir::audio::DestroyAudioClip_i(pClips[i].get());
    }

    bool IsLoaded(const Ref<AudioClip>& clip) { return clip->pcmData; }

    uint64_t GetFrameCount(const Ref<AudioClip>& clip) { return clip->frameCount; }
    uint32_t GetChannels(const Ref<AudioClip>& clip) { return clip->channels; }
    uint32_t GetSampleRate(const Ref<AudioClip>& clip) { return clip->sampleRate; }

    namespace ir::audio
    {
        Ref<AudioClip> CreateAudioClipAsset_i(const char* assetPath)
        {
            const auto assetLoc = TO_RES_PATH(std::string("audio/") + assetPath) + ".meta";
            List<char> metadata{};
            START_READING(assetLoc.c_str(), metadata);

            AudioClipCreationInfo info{};
            info.filePath = FilePath{ .path = assetPath };

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            ReadAudioClipSettings_i(root, info.settings);
            return MakeRef<AudioClip>(info);
        }

        bool DestroyAudioClip_i(AudioClip* clip)
        {
            if (clip->pcmData)
            {
                utils::Free(clip->pcmData);
                clip->pcmData = nullptr;
                return true;
            }
            return false;
        }
    }
}