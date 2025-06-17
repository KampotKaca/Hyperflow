#define HF_ENGINE_INTERNALS
#include "hinputcallbacks.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf::callbacks
{
    void KeyCallback(GLFWwindow* win, int32_t key, int32_t scancode, int32_t action, int32_t mods)
    {
        if (action == GLFW_REPEAT) return;
        auto window = inter::HF.windows[(uint64_t)win];
        auto& eventData = window->eventData;

        if (action == GLFW_PRESS) eventData.keyStates[key] = KeyState::Down;
        else eventData.keyStates[key] = KeyState::Up;
    }

    void CharCallback(GLFWwindow* win, uint32_t codepoint)
    {
        auto window = inter::HF.windows[(uint64_t)win];
        window->eventData.charData += codepoint;
    }

    void PointerMoveCallback(GLFWwindow* win, double xpos, double ypos)
    {
        auto window = inter::HF.windows[(uint64_t)win];
        auto& eventData = window->eventData;
        vec2 newPos = { (float)xpos, (float)ypos };
        eventData.pointerDelta = newPos - eventData.pointerPosition;
        eventData.pointerPosition = newPos;
    }

    void PointerStateCallback(GLFWwindow* win, int32_t entered)
    {

    }

    void ButtonCallback(GLFWwindow* win, int32_t button, int32_t action, int32_t mods)
    {
        if (action == GLFW_REPEAT) return;
        auto window = inter::HF.windows[(uint64_t)win];
        auto& eventData = window->eventData;
        if (action == GLFW_PRESS) eventData.buttonStates[button] = ButtonState::Down;
        else eventData.buttonStates[button] = ButtonState::Up;
    }

    void ScrollCallback(GLFWwindow* win, double xoffset, double yoffset)
    {
        auto window = inter::HF.windows[(uint64_t)win];
        window->eventData.scrollDelta = { (float)xoffset, (float)yoffset };
    }

    void DragAndDropCallback(GLFWwindow* win, int32_t count, const char** paths)
    {

    }

    void WindowFocusCallback(GLFWwindow* win, int focused)
    {

    }

    void WindowMinimizedCallback(GLFWwindow* win, int minimized)
    {
        auto window = inter::HF.windows[(uint64_t)win];
        if (minimized) window->state = WindowState::Minimized;
        else window->state = WindowState::Restored;

        ForceUpdateFrameBuffer(window.get());
    }

    void WindowMaximizedCallback(GLFWwindow* win, int maximized)
    {
        auto window = inter::HF.windows[(uint64_t)win];
        if (maximized) window->state = WindowState::Maximized;
        else window->state = WindowState::Restored;

        ForceUpdateFrameBuffer(window.get());
    }

    void WindowMoveCallback(GLFWwindow* win, int xpos, int ypos)
    {

    }

    void WindowSizeCallback(GLFWwindow* win, int width, int height)
    {
        auto window = inter::HF.windows[(uint64_t)win];
        ForceUpdateFrameBuffer(window.get());
    }

    void WindowRefreshCallback(GLFWwindow* win)
    {

    }

    void WindowCloseCallback(GLFWwindow* win)
    {
        inter::HF.windows[(uint64_t)win]->Close();
    }

    void JoystickEventCallback(int32_t jid, int32_t event)
    {

    }

    void ForceUpdateFrameBuffer(Window* win)
    {
        auto rn = win->renderer;
        if (rn)
        {
            std::lock_guard lock(rn->threadInfo.threadLock);
            auto size = inter::window::GetSize(win);
            rn->threadInfo.size = size;
            inter::HF.renderingApi.api.RegisterFrameBufferChange(rn->handle, size);
        }
    }
}
