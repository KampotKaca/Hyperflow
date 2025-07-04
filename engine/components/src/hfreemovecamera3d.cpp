#include "hfreemovecamera3d.h"
#include "hexternal.h"

namespace hf
{
    void FreeMoveCamera3D::Update(const Ref<Window>& window, float deltaTime)
    {
        vec3 motion = vec3(0.0f);
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Forward) > 0  && IsKeyDownContinues(window, Key::W))         motion.z += 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Backward) > 0 && IsKeyDownContinues(window, Key::S))         motion.z -= 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Right) > 0    && IsKeyDownContinues(window, Key::D))         motion.x += 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Left) > 0     && IsKeyDownContinues(window, Key::A))         motion.x -= 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Up) > 0       && IsKeyDownContinues(window, Key::Space))     motion.y += 1.0f;
        if ((uint32_t)(moveFlags & FreeMoveCameraDirection::Down) > 0     && IsKeyDownContinues(window, Key::LeftShift)) motion.y -= 1.0f;

        vec2 lookAmount = vec2((bool)(lookFlags & FreeMoveCameraLookAxis::Horizontal), (bool)(lookFlags & FreeMoveCameraLookAxis::Vertical));
        auto delta = GetPointerDelta(window);
        auto windowSize = GetSize(window);
        lookAmount.x *= delta.x / (float)windowSize.x;
        lookAmount.y *= delta.y / (float)windowSize.y;

        motion *= moveSpeed * deltaTime;
        lookAmount *= lookSpeed;

        float pitch = glm::asin(camera3D.direction.y) - lookAmount.y;
        float yaw = glm::atan(camera3D.direction.x, camera3D.direction.z) + lookAmount.x;

        float maxPitch = glm::radians(pitchLimits.y);
        float minPitch = glm::radians(pitchLimits.x);
        pitch = glm::clamp(pitch, minPitch, maxPitch);

        glm::quat yawQuat = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
        glm::quat pitchQuat = glm::angleAxis(-pitch, glm::vec3(1, 0, 0));
        glm::quat rotation = yawQuat * pitchQuat;

        camera3D.direction = glm::normalize(rotation * glm::vec3(0, 0, 1));

        camera3D.position += motion.x * glm::cross(camera3D.up, camera3D.direction);
        camera3D.position += motion.y * camera3D.up;
        camera3D.position += motion.z * camera3D.direction;
    }
}
