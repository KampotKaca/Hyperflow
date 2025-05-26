#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "hplatform.h"
#include "hinputcallbacks.h"
#include "hinternal.h"

namespace hf::inter
{
    namespace platform
    {
        void Load()
        {
            if(!glfwInit()) throw GENERIC_EXCEPT("[GLFW]", "Error: Failed to initialize GLFW!");
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
            return hf::platform::GetPointerPosition(window);
        }

        void SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value)
        {
            hf::platform::SetWindowFlag(flags, target, value);
        }

        void HandleEvents(EngineUpdateType updateType)
        {
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
            hf::platform::CreateVulkanSurface(windowHandle, instance, surfaceResult);
            auto* winHandle = (GLFWwindow*)windowHandle;
            return glfwCreateWindowSurface((VkInstance)instance, winHandle, nullptr, (VkSurfaceKHR*)surfaceResult);
        }
    }

    namespace window
    {
        void Open(Window* win)
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

            auto newWindow = glfwCreateWindow(win->rect.size.x, win->rect.size.y, win->title.c_str(), nullptr, nullptr);
            if (!newWindow) throw GENERIC_EXCEPT("[Hyperflow]", "Failed to create window!");

            glfwFocusWindow(newWindow);
            win->handle = newWindow;
            hf::platform::window::Open(win);
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

                return hf::platform::window::Close(win);
            }
            return false;
        }

        void SetTitle(const Window* win, const std::string& title)
        {
            hf::platform::window::SetTitle(win, title);
        }

        void SetSize(const Window* win, ivec2 size)
        {
            hf::platform::window::SetSize(win, size);
        }

        void SetPosition(const Window* win, ivec2 position)
        {
            hf::platform::window::SetPosition(win, position);
        }

        void SetRect(const Window* win, IRect rect)
        {
            hf::platform::window::SetRect(win, rect);
        }

        void SetFlags(Window* win, WindowFlags flags)
        {
            hf::platform::window::SetFlags(win, flags);
        }

        void SetEventFlags(Window* win, WindowEventFlags eventFlags)
        {
            if ((bool)(eventFlags & WindowEventFlags::Key)) glfwSetKeyCallback((GLFWwindow*)win->handle, hf::platform::KeyCallback);
            else glfwSetKeyCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Char)) glfwSetCharCallback((GLFWwindow*)win->handle, hf::platform::CharCallback);
            else glfwSetCharCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::PointerMove)) glfwSetCursorPosCallback((GLFWwindow*)win->handle, hf::platform::PointerMoveCallback);
            else glfwSetCursorPosCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::PointerState)) glfwSetCursorEnterCallback((GLFWwindow*)win->handle, hf::platform::PointerStateCallback);
            else glfwSetCursorEnterCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Button)) glfwSetMouseButtonCallback((GLFWwindow*)win->handle, hf::platform::ButtonCallback);
            else glfwSetMouseButtonCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::Scroll)) glfwSetScrollCallback((GLFWwindow*)win->handle, hf::platform::ScrollCallback);
            else glfwSetScrollCallback((GLFWwindow*)win->handle, nullptr);

            if ((bool)(eventFlags & WindowEventFlags::DragAndDrop)) glfwSetDropCallback((GLFWwindow*)win->handle, hf::platform::DragAndDropCallback);
            else glfwSetDropCallback((GLFWwindow*)win->handle, nullptr);

            win->eventFlags = eventFlags;
        }

        void Focus(const Window* win)
        {
            hf::platform::window::Focus(win);
        }
    }
}