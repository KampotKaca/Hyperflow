#include "haudiointernal.h"
#include "hyperflow.h"
#include "../../external/miniaudio/miniaudio.h"

namespace hf
{
    AudioListener Define(const AudioListenerDefinitionInfo& info)
    {
        try
        {
            auto newListener = inter::AUDIO_DATA.definedListenersCount + 1;
            inter::AUDIO_DATA.definedListenersCount++;

            ma_engine_listener_set_world_up(&inter::AUDIO_DATA.engine, (ma_uint32)newListener, 0.0f, 1.0f, 0.0f);
            Set(newListener, info.cone);
            Enable(newListener, info.isEnabled);
            return newListener;
        }
        catch (const HyperException& e)
        {
            LOG_ERROR("Unable to define audio listener\n%s", e.what());
            return 0;
        }
        catch (...)
        {
            LOG_ERROR("Unable to define audio listener!");
            return 0;
        }
    }

    void Set(AudioListener listener, const AudioCone& cone)
    {
        ma_engine_listener_set_cone(&inter::AUDIO_DATA.engine, (ma_uint32)listener, cone.innerAngle, cone.outerAngle, cone.outerGain);
        ma_engine_listener_set_position(&inter::AUDIO_DATA.engine, (ma_uint32)listener, cone.position.x, cone.position.y, cone.position.z);

        vec3 direction = vec3(glm::eulerAngleXYZ(cone.euler.x, cone.euler.y, cone.euler.z) * vec4(0, 0, 1, 0));
        ma_engine_listener_set_direction(&inter::AUDIO_DATA.engine, (ma_uint32)listener, direction.x, direction.y, direction.z);
    }

    void Enable(AudioListener listener, bool enable)
    {
        ma_engine_listener_set_enabled(&inter::AUDIO_DATA.engine, (ma_uint32)listener, enable);
    }

    void SetVelocity(AudioListener listener, vec3 velocity)
    {
        ma_engine_listener_set_velocity(&inter::AUDIO_DATA.engine, (ma_uint32)listener, velocity.x, velocity.y, velocity.z);
    }

    vec3 GetVelocity(AudioListener listener)
    {
        auto vel = ma_engine_listener_get_velocity(&inter::AUDIO_DATA.engine, (ma_uint32)listener);
        return { vel.x, vel.y, vel.z };
    }

    AudioCone GetCone(AudioListener listener)
    {
        AudioCone cone{};
        auto pos = ma_engine_listener_get_position(&inter::AUDIO_DATA.engine, (ma_uint32)listener);
        auto dir = ma_engine_listener_get_direction(&inter::AUDIO_DATA.engine, (ma_uint32)listener);
        auto up = ma_engine_listener_get_world_up(&inter::AUDIO_DATA.engine, (ma_uint32)listener);

        ma_engine_listener_get_cone(&inter::AUDIO_DATA.engine, (ma_uint32)listener, &cone.innerAngle, &cone.outerAngle, &cone.outerGain);
        cone.innerAngle = glm::degrees(cone.innerAngle);
        cone.outerAngle = glm::degrees(cone.outerAngle);
        cone.position = { pos.x, pos.y, pos.z };

        return cone;
    }
}
