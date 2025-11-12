#include "haudiointernal.h"
#include "hyperflow.h"
#include "audio/haudiolistener.h"
#include "miniaudio.h"

namespace hf
{
    AudioListener::AudioListener(const AudioListenerCreationInfo& info)
    {
    }

    AudioListener::~AudioListener()
    {

    }

    Ref<AudioListener> Create(const AudioListenerCreationInfo& info)
    {
        auto newId = ir::AUDIO_DATA.definedListenersCount;
        ir::AUDIO_DATA.definedListenersCount++;

        ma_engine_listener_set_world_up(&ir::AUDIO_DATA.engine, (ma_uint32)newId, 0.0f, 1.0f, 0.0f);
        auto listener = MakeRef<AudioListener>(info);
        listener->handle = newId;

        Set(listener, info.cone);
        Enable(listener, info.isEnabled);
        return listener;
    }

    void Set(const Ref<AudioListener>& ls, const AudioCone& cone)
    {
        auto oldCone = ls->cone;
        auto handle = (ma_uint32)ls->handle;
        if (oldCone.innerAngle != cone.innerAngle ||
            oldCone.outerAngle != cone.outerAngle ||
            oldCone.outerGain != cone.outerGain)
            ma_engine_listener_set_cone(&ir::AUDIO_DATA.engine, handle, glm::radians(cone.innerAngle), glm::radians(cone.outerAngle), cone.outerGain);

        if (oldCone.position != cone.position)
            ma_engine_listener_set_position(&ir::AUDIO_DATA.engine, handle, cone.position.x, cone.position.y, cone.position.z);

        if (oldCone.euler != cone.euler)
        {
            const auto direction = vec3(glm::eulerAngleXYZ(cone.euler.x, cone.euler.y, cone.euler.z) * vec4(0, 0, 1, 0));
            ma_engine_listener_set_direction(&ir::AUDIO_DATA.engine, handle, direction.x, direction.y, direction.z);
        }

        ls->cone = cone;
    }

    void Enable(const Ref<AudioListener>& ls, bool enable)
    {
        if (ls->isEnabled != enable)
        {
            ls->isEnabled = enable;
            ma_engine_listener_set_enabled(&ir::AUDIO_DATA.engine, (ma_uint32)ls->handle, enable);
        }
    }

    void SetVelocity(const Ref<AudioListener>& ls, vec3 velocity)
    {
        if (ls->velocity != velocity)
        {
            ls->velocity = velocity;
            ma_engine_listener_set_velocity(&ir::AUDIO_DATA.engine, (ma_uint32)ls->handle, velocity.x, velocity.y, velocity.z);
        }
    }

    vec3 GetVelocity(const Ref<AudioListener>& ls) { return ls->velocity; }
    AudioCone GetCone(const Ref<AudioListener>& ls) { return ls->cone; }

    bool IsEnabled(const Ref<AudioListener>& ls) { return ls->isEnabled; }
}
