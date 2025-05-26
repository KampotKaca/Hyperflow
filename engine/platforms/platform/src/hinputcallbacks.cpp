#include "hinputcallbacks.h"

namespace hf::platform
{
    void KeyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
    {
        LOG_INFO("Key Press: %s", glfwGetKeyName(key, scancode));
    }

    void CharCallback(GLFWwindow* window, uint32_t codepoint)
    {

    }

    void PointerMoveCallback(GLFWwindow* window, double xpos, double ypos)
    {

    }

    void PointerStateCallback(GLFWwindow* window, int32_t entered)
    {

    }

    void ButtonCallback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
    {

    }

    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {

    }

    void JoystickEventCallback(int32_t jid, int32_t event)
    {

    }

    void DragAndDropCallback(GLFWwindow* window, int32_t count, const char** paths)
    {

    }
}