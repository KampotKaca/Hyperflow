#include "hexternal.h"
#include "hcamera3dcore.h"
#include "hcamera3danchored.h"
#include "hcamera3dfreelook.h"

namespace hf
{
    mat4 Camera3DCore::ToProjectionMat4(const Ref<Renderer>& rn) const
    {
        const auto& size = rn->threadInfo.size;
        float aspect = (float)size.x / (float)size.y;
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
}