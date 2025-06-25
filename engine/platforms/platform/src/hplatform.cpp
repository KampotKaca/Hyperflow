#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "hinputcallbacks.h"

#include "hplatform.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf::inter
{
    namespace platform
    {
        void Load()
        {
            if(!glfwInit()) throw GENERIC_EXCEPT("[Hyperflow]", "Error: Failed to initialize GLFW!");
            hf::platform::Load();
        }

        void Unload()
        {
            glfwTerminate();
            hf::platform::Unload();
        }

        void Sleep(double seconds)
        {
            hf::platform::Sleep(seconds);
        }

        ivec2 GetPointerPosition(const Window* window)
        {
            double px = 0, py = 0;
            glfwGetCursorPos((GLFWwindow*)window->handle, &px, &py);
            return { px, py };
        }

        void HandleEvents(EngineUpdateType updateType)
        {
            for(const auto& window : HF.windows | std::views::values)
            {
                if (window->IsClosed()) continue;
                auto& eventData = window->eventData;
                for (auto& currentState : eventData.keyStates)
                {
                    switch (currentState)
                    {
                        case KeyState::None: continue;
                        case KeyState::Up: currentState = KeyState::None; continue;
                        case KeyState::Down: currentState = KeyState::DownContinues; break;
                        case KeyState::DownContinues: break;
                    }
                }

                for (auto& currentState : eventData.buttonStates)
                {
                    switch (currentState)
                    {
                        case KeyState::None: continue;
                        case KeyState::Up: currentState = KeyState::None; continue;
                        case KeyState::Down: currentState = KeyState::DownContinues; break;
                        case KeyState::DownContinues: break;
                    }
                }

                eventData.pointerDelta = {};
                eventData.scrollDelta = {};
            }

            switch (updateType)
            {
                case EngineUpdateType::Continues: glfwPollEvents(); break;
                case EngineUpdateType::EventRaised: glfwWaitEvents(); break;
            }

            hf::platform::HandleEvents(updateType);
        }

        void* LoadDll(const char* dllName)
        {
            return hf::platform::LoadDll(dllName);
        }

        void UnloadDll(void* dll)
        {
            hf::platform::UnloadDll(dll);
        }

        void* GetFuncPtr(void* dll, const char* funcName)
        {
            return hf::platform::GetFuncPtr(dll, funcName);
        }

        RenderingApiType GetBestRenderingApi()
        {
            return hf::platform::GetBestRenderingApi();
        }

        bool IsValidRenderingApi(RenderingApiType api)
        {
            return hf::platform::IsValidRenderingApi(api);
        }

        uint32_t CreateVulkanSurface(void* windowHandle, void* instance, void* surfaceResult)
        {
#if defined(VULKAN)
            auto* winHandle = (GLFWwindow*)windowHandle;
            return glfwCreateWindowSurface((VkInstance)instance, winHandle, nullptr, (VkSurfaceKHR*)surfaceResult);
#else
            return -1;
#endif
        }
    }

    namespace window
    {
        void Open(Window* win, const WindowCreationInfo& info)
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

            auto newWindow = glfwCreateWindow(win->rect.size.x, win->rect.size.y,
                info.title.c_str(), nullptr, nullptr);
            if (!newWindow) throw GENERIC_EXCEPT("[Hyperflow]", "Failed to create window!");

            win->handle = newWindow;
        }

        bool Close(Window* win)
        {
            if (win->handle)
            {
                rendering::DestroyRenderer_i(win->renderer.get());
                win->renderer = nullptr;

                auto window = (GLFWwindow*)win->handle;
                glfwDestroyWindow(window);
                win->handle = nullptr;
                win->parent = nullptr;

                return true;
            }
            return false;
        }

        void SetTitle(Window* win, const std::string& title)
        {
            glfwSetWindowTitle((GLFWwindow*)win->handle, title.c_str());
            win->title = title;
        }

        void SetSize(const Window* win, ivec2 size)
        {
            glfwSetWindowSize((GLFWwindow*)win->handle, size.x, size.y);
        }

        void SetPosition(const Window* win, ivec2 position)
        {
            glfwSetWindowPos((GLFWwindow*)win->handle, position.x, position.y);
        }

        void SetRect(const Window* win, IRect rect)
        {
            glfwSetWindowSize((GLFWwindow*)win->handle, rect.size.x, rect.size.y);
            glfwSetWindowPos((GLFWwindow*)win->handle, rect.position.x, rect.position.y);
        }

        void SetPointerState(Window* win, WindowPointerState state)
        {
            switch (state)
            {
            case WindowPointerState::Normal:
                glfwSetInputMode((GLFWwindow*)win->handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
            case WindowPointerState::Hidden:
                glfwSetInputMode((GLFWwindow*)win->handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                break;
            case WindowPointerState::Disabled:
                glfwSetInputMode((GLFWwindow*)win->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;
            case WindowPointerState::Captured:
                glfwSetInputMode((GLFWwindow*)win->handle, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
                break;
            }

            win->pointerState = state;
        }

        void SetState(Window* win, WindowState state)
        {
            auto window = (GLFWwindow*)win->handle;
            if (win->state == state) return;

            auto rect = win->rect;
            switch (state)
            {
            case WindowState::Restored:
                {
                    glfwRestoreWindow(window);
                    glfwShowWindow(window);
                    glfwSetWindowMonitor(window, nullptr,
                        rect.position.x, rect.position.y,
                        rect.size.x, rect.size.y, 0);
                    break;
                }
            case WindowState::Minimized: glfwIconifyWindow(window);  break;
            case WindowState::Maximized: glfwMaximizeWindow(window); break;
            case WindowState::Hidden:    glfwHideWindow(window);     break;
            case WindowState::Fullscreen:
                {
                    GLFWmonitor* monitor = glfwGetWindowMonitor(window);
                    if (!monitor) monitor = glfwGetPrimaryMonitor();

                    if (win->state != WindowState::FullscreenBorderless)
                    {
                        win->rect.position = GetPosition(win);
                        win->rect.size = GetSize(win);
                    }

                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                    break;
                }
            case WindowState::FullscreenBorderless:
                {
                    GLFWmonitor* monitor = glfwGetWindowMonitor(window);
                    if (!monitor) monitor = glfwGetPrimaryMonitor();
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

                    if (win->state != WindowState::Fullscreen)
                    {
                        win->rect.position = GetPosition(win);
                        win->rect.size = GetSize(win);
                    }

                    glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
                    glfwSetWindowMonitor(window, nullptr, 0, 0, mode->width, mode->height, 0);
                    break;
                }
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Invalid window state!");
            }

            if (win->state == WindowState::FullscreenBorderless)
                glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);

            win->state = state;
        }

        void SetEventFlags(Window* win, WindowEventFlags eventFlags)
        {
            auto handle = (GLFWwindow*)win->handle;
            if ((bool)(eventFlags & WindowEventFlags::Key)) glfwSetKeyCallback(handle, callbacks::KeyCallback);
            else glfwSetKeyCallback(handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Char)) glfwSetCharCallback(handle, callbacks::CharCallback);
            else glfwSetCharCallback(handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::PointerMove)) glfwSetCursorPosCallback(handle, callbacks::PointerMoveCallback);
            else glfwSetCursorPosCallback(handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::PointerState)) glfwSetCursorEnterCallback(handle, callbacks::PointerStateCallback);
            else glfwSetCursorEnterCallback(handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Button)) glfwSetMouseButtonCallback(handle, callbacks::ButtonCallback);
            else glfwSetMouseButtonCallback(handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Scroll)) glfwSetScrollCallback(handle, callbacks::ScrollCallback);
            else glfwSetScrollCallback(handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::DragAndDrop)) glfwSetDropCallback(handle, callbacks::DragAndDropCallback);
            else glfwSetDropCallback(handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Close)) glfwSetWindowCloseCallback((GLFWwindow*)win->handle, callbacks::WindowCloseCallback);
            else glfwSetWindowCloseCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Focus)) glfwSetWindowFocusCallback((GLFWwindow*)win->handle, callbacks::WindowFocusCallback);
            else glfwSetWindowFocusCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Minimize)) glfwSetWindowIconifyCallback((GLFWwindow*)win->handle, callbacks::WindowMinimizedCallback);
            else glfwSetWindowIconifyCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Maximize)) glfwSetWindowMaximizeCallback((GLFWwindow*)win->handle, callbacks::WindowMaximizedCallback);
            else glfwSetWindowMaximizeCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Move)) glfwSetWindowPosCallback((GLFWwindow*)win->handle, callbacks::WindowMoveCallback);
            else glfwSetWindowPosCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Resize)) glfwSetWindowSizeCallback((GLFWwindow*)win->handle, callbacks::WindowSizeCallback);
            else glfwSetWindowSizeCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Refresh)) glfwSetWindowRefreshCallback((GLFWwindow*)win->handle, callbacks::WindowRefreshCallback);
            else glfwSetWindowRefreshCallback((GLFWwindow*)win->handle, nullptr);

            win->eventFlags = eventFlags;
        }

        void Focus(const Window* win)
        {
            glfwFocusWindow((GLFWwindow*)win->handle);
        }

        bool IsFocused(const Window* win)
        {
            return glfwGetCurrentContext() == (GLFWwindow*)win->handle;
        }

        IRect GetFrameRect(const Window* win)
        {
            int32_t top = 0, bottom = 0,
                    right = 0, left = 0;
            glfwGetWindowFrameSize((GLFWwindow*)win->handle, &left, &top, &right, &bottom);
            return
            {
                .position = { left, top },
                .size = { right - left, bottom - top },
            };
        }

        ivec2 GetSize(const Window* win)
        {
            ivec2 size = { 0, 0 };
            glfwGetWindowSize((GLFWwindow*)win->handle, &size.x, &size.y);
            return size;
        }

        ivec2 GetPosition(const Window* win)
        {
            ivec2 pos = { 0, 0 };
            glfwGetWindowPos((GLFWwindow*)win->handle, &pos.x, &pos.y);
            return pos;
        }

        const char* GetTitle(const Window* win) { return glfwGetWindowTitle((GLFWwindow*)win->handle); }

        void SetIcons(const Window* win, const Image* pImages, uint32_t count)
        {
            glfwSetWindowIcon((GLFWwindow*)win->handle, (int)count, (GLFWimage*)pImages);
        }

        bool ShouldClose(const Window* win)
        {
            return glfwWindowShouldClose((GLFWwindow*)win->handle);
        }
    }
}
