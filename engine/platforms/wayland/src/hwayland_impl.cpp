#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "hex_platform.h"
#include "hinternal.h"

namespace hf::inter
{
    namespace platform
    {
        void Load()
        {
            if(!glfwInit()) throw GENERIC_EXCEPT("[GLFW]", "Error: Failed to initialize GLFW!");
        }

        void Unload()
        {
            glfwTerminate();
        }

        ivec2 GetPointerPosition(const Window* window)
        {
            return {};
        }

        void HandleEvents(EngineUpdateType updateType)
        {
            glfwPollEvents();
        }

        uint32_t CreateVulkanSurface(void* windowHandle, void* instance, void* surfaceResult)
        {
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

        void SetTitle(const Window* win, const std::string& title)
        {
        }

        void SetSize(const Window* win, ivec2 size)
        {
        }

        void SetPosition(const Window* win, ivec2 position)
        {
        }

        void SetRect(const Window* win, IRect rect)
        {
        }

        void SetFlags(Window* win, WindowFlags flags)
        {
        }

        void Focus(const Window* win)
        {
        }
    }
}
