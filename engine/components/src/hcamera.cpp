#include "hshared.h"
#include "hrenderer.h"
#include "hinternal.h"
#include "hcamera3dcore.h"
#include "hcamera3danchored.h"
#include "hcamera3dfreelook.h"

namespace hf
{
    static CameraUniformInfo GetUniform(const Camera3DCore& core,
                                        vec3 lookDirection,  vec3 position, const mat4& view);

    mat4 Camera3DCore::ToProjectionMat4() const
    {
        const auto& size = ir::HF.renderer->threadInfo.size;
        const float_t aspect = (float_t)size.x / (float_t)size.y;
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }

    mat4 Camera3DAnchored::ToViewProjectionMat4() const
    {
        return core.ToProjectionMat4() * ToViewMat4();
    }

    mat4 Camera3DFreeLook::ToViewProjectionMat4() const
    {
        return core.ToProjectionMat4() * ToViewMat4();
    }

    CameraUniformInfo Camera3DAnchored::GetUniformInfo() const
    {
        return GetUniform(core, GetDirection(), GetPosition(), ToViewMat4());
    }

    CameraUniformInfo Camera3DFreeLook::GetUniformInfo() const
    {
        return GetUniform(core, direction, position, ToViewMat4());
    }

    CameraUniformInfo GetUniform(const Camera3DCore& core,
                      const vec3 lookDirection, const vec3 position, const mat4& view)
    {
        CameraUniformInfo info{};
        info.lookDirection = lookDirection;
        info.position      = position;
        info.view          = view;
        info.invView       = glm::inverse(view);
        info.proj          = core.ToProjectionMat4();

        info.invProj  = glm::inverse(info.proj);
        info.viewProj = info.proj * info.view;
        return info;
    }
}