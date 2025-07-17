#include "hfreemovecamera3d.h"
#include "hexternal.h"

namespace hf
{
    void FreeMoveCamera3D::Update(const Ref<Window>& window, const float_t deltaTime)
    {
        auto motion = vec3(0.0f);
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Forward) > 0  && IsKeyDownContinues(window, Key::W))         motion.z += 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Backward) > 0 && IsKeyDownContinues(window, Key::S))         motion.z -= 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Right) > 0    && IsKeyDownContinues(window, Key::D))         motion.x += 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Left) > 0     && IsKeyDownContinues(window, Key::A))         motion.x -= 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Up) > 0       && IsKeyDownContinues(window, Key::Space))     motion.y += 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Down) > 0     && IsKeyDownContinues(window, Key::LeftShift)) motion.y -= 1.0f;

        auto lookAmount = vec2((bool)(lookFlags & FreeMoveCameraLookAxis::Horizontal), (bool)(lookFlags & FreeMoveCameraLookAxis::Vertical));
        const auto delta = GetPointerDelta(window);
        const auto windowSize = GetSize(window);
        lookAmount.x *= delta.x / (float_t)windowSize.x;
        lookAmount.y *= delta.y / (float_t)windowSize.y;

        motion *= moveSpeed * deltaTime;
        lookAmount *= lookSpeed;

        float_t pitch = glm::asin(camera3D.direction.y) - lookAmount.y;
        const float_t yaw = glm::atan(camera3D.direction.x, camera3D.direction.z) + lookAmount.x;

        const float_t maxPitch = glm::radians(pitchLimits.y);
        const float_t minPitch = glm::radians(pitchLimits.x);
        pitch = glm::clamp(pitch, minPitch, maxPitch);

        const auto yawQuat = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
        const auto pitchQuat = glm::angleAxis(-pitch, glm::vec3(1, 0, 0));
        const auto rotation = yawQuat * pitchQuat;

        camera3D.direction = glm::normalize(rotation * glm::vec3(0, 0, 1));

        camera3D.position += motion.x * glm::cross(camera3D.up, camera3D.direction);
        camera3D.position += motion.y * camera3D.up;
        camera3D.position += motion.z * camera3D.direction;
    }
}
