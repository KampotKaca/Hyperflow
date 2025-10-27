#include "audio/haudiogroup.h"
#include "haudiointernal.h"
#include "hinternal.h"

namespace hf
{
    inline void Destroy_i(AudioGroup* group)
    {
        if (group->handle)
        {
            ma_sound_group_uninit((ma_sound_group*)group->handle);
            delete (ma_sound_group*)group->handle;
            group->handle = nullptr;
        }
    }

    AudioGroup::AudioGroup(const AudioGroupCreationInfo& info)
        : enabled(info.enabled), volume(info.volume), pitch(info.pitch)
    {
        ma_sound_group* parent{};
        if (info.parent) parent = (ma_sound_group*)info.parent->handle;
        auto group = new ma_sound_group();
        handle = group;
        ma_sound_group_init(&ir::AUDIO_DATA.engine, 0, parent, group);
        if (enabled) ma_sound_group_start(group);
        else ma_sound_group_stop(group);
        ma_sound_group_set_volume(group, volume);
        ma_sound_group_set_pitch(group, pitch);
    }

    AudioGroup::~AudioGroup()
    {
        Destroy_i(this);
    }

    Ref<AudioGroup> Create(const AudioGroupCreationInfo& info)
    {
        Ref<AudioGroup> group = MakeRef<AudioGroup>(info);
        ir::HF.audioResources.groups[(uint64_t)group.get()] = group;
        return group;
    }

    void Enable(const Ref<AudioGroup>& group, bool enable)
    {
        if (group->enabled != enable)
        {
            if (enable) ma_sound_group_start((ma_sound_group*)group->handle);
            else ma_sound_group_stop((ma_sound_group*)group->handle);
            group->enabled = enable;
        }
    }

    void SetVolume(const Ref<AudioGroup>& group, float_t volume)
    {
        if (group->volume != volume)
        {
            ma_sound_group_set_volume((ma_sound_group*)group->handle, volume);
            group->volume = volume;
        }
    }

    void SetPitch(const Ref<AudioGroup>& group, float_t pitch)
    {
        if (group->pitch != pitch)
        {
            ma_sound_group_set_pitch((ma_sound_group*)group->handle, pitch);
            group->pitch = pitch;
        }
    }

    bool IsLoaded(const Ref<AudioGroup>& group)     { return group->handle; }
    bool IsEnabled(const Ref<AudioGroup>& group)    { return group->enabled; }
    float_t GetPitch(const Ref<AudioGroup>& group)  { return group->pitch; }
    float_t GetVolume(const Ref<AudioGroup>& group) { return group->volume; }
}