#include "hinputcallbacks.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf::callbacks
{
    void KeyCallback(GLFWwindow* win, int32_t key, int32_t scancode, int32_t action, int32_t mods)
    {
        if (action == GLFW_REPEAT) return;
        auto& eventData = ir::HF.window->eventData;

        if (action == GLFW_PRESS) eventData.keyStates[key] = KeyState::Down;
        else eventData.keyStates[key] = KeyState::Up;
    }

    void CharCallback(GLFWwindow* win, uint32_t codepoint)
    {
        ir::HF.window->eventData.charData += codepoint;
    }

    void PointerMoveCallback(GLFWwindow* win, double xpos, double ypos)
    {
        auto& eventData = ir::HF.window->eventData;
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
        auto& eventData = ir::HF.window->eventData;
        if (action == GLFW_PRESS) eventData.buttonStates[button] = ButtonState::Down;
        else eventData.buttonStates[button] = ButtonState::Up;
    }

    void ScrollCallback(GLFWwindow* win, double xoffset, double yoffset)
    {
        ir::HF.window->eventData.scrollDelta = { (float)xoffset, (float)yoffset };
    }

    void DragAndDropCallback(GLFWwindow* win, int32_t count, const char** paths)
    {

    }

    void WindowFocusCallback(GLFWwindow* win, int focused)
    {

    }

    void WindowMinimizedCallback(GLFWwindow* win, int minimized)
    {
        if (minimized) ir::HF.window->state = WindowState::Minimized;
        else ir::HF.window->state = WindowState::Restored;

        ForceUpdateFrameBuffer(ir::HF.window.get());
    }

    void WindowMaximizedCallback(GLFWwindow* win, int maximized)
    {
        if (maximized) ir::HF.window->state = WindowState::Maximized;
        else ir::HF.window->state = WindowState::Restored;

        ForceUpdateFrameBuffer(ir::HF.window.get());
    }

    void WindowMoveCallback(GLFWwindow* win, int xpos, int ypos)
    {

    }

    void WindowSizeCallback(GLFWwindow* win, int width, int height)
    {
        ForceUpdateFrameBuffer(ir::HF.window.get());
    }

    void WindowRefreshCallback(GLFWwindow* win)
    {

    }

    void WindowCloseCallback(GLFWwindow* win)
    {
        ir::win::Close(ir::HF.window.get());
    }

    void JoystickEventCallback(int32_t jid, int32_t event)
    {

    }

    void ForceUpdateFrameBuffer(Window* win)
    {
        if (ir::HF.renderer)
        {
            std::lock_guard lock(ir::HF.renderer->threadInfo.threadLock);
            auto size = ir::win::GetSize(win);
            ir::HF.renderer->threadInfo.size = size;
            ir::HF.renderingApi.api.RegisterFrameBufferChange(ir::HF.renderer->handle, size);
        }
    }
}
