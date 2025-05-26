#ifndef HINPUTCALLBACKS_H
#define HINPUTCALLBACKS_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "hshared.h"

namespace hf::callbacks
{
    void KeyCallback(GLFWwindow* win, int32_t key, int32_t scancode, int32_t action, int32_t mods);
    void CharCallback(GLFWwindow* win, uint32_t codepoint);
    void PointerMoveCallback(GLFWwindow* win, double xpos, double ypos);
    void PointerStateCallback(GLFWwindow* win, int32_t entered);

    void ButtonCallback(GLFWwindow* win, int32_t button, int32_t action, int32_t mods);
    void ScrollCallback(GLFWwindow* win, double xoffset, double yoffset);

    void DragAndDropCallback(GLFWwindow* win, int32_t count, const char** paths);
    void WindowFocusCallback(GLFWwindow* win, int focused);
    void WindowMinimizedCallback(GLFWwindow* win, int minimized);
    void WindowMaximizedCallback(GLFWwindow* win, int maximized);
    void WindowMoveCallback(GLFWwindow* win, int xpos, int ypos);
    void WindowSizeCallback(GLFWwindow* win, int width, int height);
    void WindowRefreshCallback(GLFWwindow* win);
    void WindowCloseCallback(GLFWwindow* win);

    void JoystickEventCallback(int32_t jid, int32_t event);
}

#endif //HINPUTCALLBACKS_H
