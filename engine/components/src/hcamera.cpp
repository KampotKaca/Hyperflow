#include "hexternal.h"
#include "hcamera3dcore.h"
#include "hcamera3danchored.h"
#include "hcamera3dfreelook.h"

namespace hf
{
    static CameraUniformInfo GetUniform(const Ref<Renderer>& rn, const Camera3DCore& core,
                                        const vec3 lookDirection, const vec3 position, const mat4& view);

    mat4 Camera3DCore::ToProjectionMat4(const Ref<Renderer>& rn) const
    {
        const auto& size = rn->threadInfo.size;
        const float_t aspect = (float_t)size.x / (float_t)size.y;
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }

    mat4 Camera3DAnchored::ToViewProjectionMat4(const Ref<Renderer>& rn) const
    {
        return core.ToProjectionMat4(rn) * ToViewMat4();
    }

    mat4 Camera3DFreeLook::ToViewProjectionMat4(const Ref<Renderer>& rn) const
    {
        return core.ToProjectionMat4(rn) * ToViewMat4();
    }

    CameraUniformInfo Camera3DAnchored::GetUniformInfo(const Ref<Renderer>& rn) const
    {
        return GetUniform(rn, core, GetDirection(), GetPosition(), ToViewMat4());
    }

    CameraUniformInfo Camera3DFreeLook::GetUniformInfo(const Ref<Renderer>& rn) const
    {
        return GetUniform(rn, core, direction, position, ToViewMat4());
    }

    CameraUniformInfo GetUniform(const Ref<Renderer>& rn, const Camera3DCore& core,
                      const vec3 lookDirection, const vec3 position, const mat4& view)
    {
        CameraUniformInfo info
        {
            .lookDirection = lookDirection,
            .position = position,
            .view = view,
            .invView = glm::inverse(view),
            .proj = core.ToProjectionMat4(rn),
        };

        info.invProj = glm::inverse(info.proj);
        info.viewProj = info.proj * info.view;
        return info;
    }
}