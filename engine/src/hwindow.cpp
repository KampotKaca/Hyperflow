#include "hinternal.h"
#include "hyperflow.h"
#include "hwindow.h"
#include "hrenderer.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
	Window::Window(const WindowCreationInfo& info, const Ref<Window>& parent)
		: title(info.title), style(info.style), pointerState(info.pointerState), vSyncMode(info.vsyncMode), parent(parent),
		  rnEventInfo(info.rnEventInfo)
	{
		rect =
		{
			.position = info.position,
			.size = info.size
		};
		renderer = nullptr;

		inter::window::Open(this, info);
		eventData.pointerPosition = inter::platform::GetPointerPosition(this);

		inter::window::SetEventFlags(this, info.eventFlags);
		inter::window::SetState(this, info.state);
		inter::window::Focus(this);
		inter::window::SetTitle(this, info.title);
		inter::window::SetTitle(this, info.title);
		inter::window::SetPointerState(this, info.pointerState);
	}

	Window::~Window()
	{
		inter::window::Close(this);
	}

	namespace window
	{
		Ref<Window> Open(const WindowCreationInfo &data, const Ref<Window> &parent)
		{
			auto newWindow = MakeRef<Window>(data, parent);
			inter::HF.windows[(uint64_t)newWindow->handle] = newWindow;

			if (inter::HF.renderingApi.type != RenderingApiType::None)
			{
				newWindow->renderer = MakeRef<Renderer>(newWindow.get(), newWindow->rnEventInfo);
				auto rn = newWindow->renderer;
				inter::HF.renderingApi.api.PostInstanceLoad(newWindow->renderer->handle,
					rn->eventInfo.onPassCreationCallback(rn));
				inter::rendering::RunRenderThread_i(rn);
			}
			return newWindow;
		}

		void Close(const Ref<Window> &window)
		{
			if (inter::window::Close(window.get()))
				inter::HF.windows.erase((uint64_t)window->handle);
		}

		void CloseAll()
		{
			for (auto& win : inter::HF.windows | std::views::values)
				inter::window::Close(win.get());
			inter::HF.windows.clear();
		}

		const std::string& GetTitle(const Ref<Window> &window) { return window->title; }
		ivec2 GetSize(const Ref<Window> &window) { return inter::window::GetSize(window.get()); }
		ivec2 GetPosition(const Ref<Window> &window) { return inter::window::GetPosition(window.get()); }
		IRect GetRect(const Ref<Window> &window)
		{
			IRect result{};
			result.position = inter::window::GetPosition(window.get());
			result.size = inter::window::GetSize(window.get());
			return result;
		}
		IRect GetFrameRect(const Ref<Window> &window) { return inter::window::GetFrameRect(window.get()); }
		WindowState GetState(const Ref<Window> &window) { return window->state; }
		WindowStyle GetStyle(const Ref<Window> &window) { return window->style; }
		WindowPointerState GetPointerState(const Ref<Window> &window) { return window->pointerState; }
		void* GetHandle(const Ref<Window> &window) { return window->handle; }
		Ref<Renderer> GetRenderer(const Ref<Window> &window) { return window->renderer; }

		bool IsClosed(const Ref<Window> &window) { return window->handle == nullptr; }

		void SetTitle(const Ref<Window> &window, const std::string& title) { if (!IsClosed(window)) inter::window::SetTitle(window.get(), title); }
		void SetSize(const Ref<Window> &window, const ivec2 size) { if (!IsClosed(window)) inter::window::SetSize(window.get(), size); }
		void SetPosition(const Ref<Window> &window, const ivec2 position) { if (!IsClosed(window)) inter::window::SetPosition(window.get(), position); }
		void SetRect(const Ref<Window> &window, const IRect rect) { if (!IsClosed(window)) inter::window::SetRect(window.get(), rect); }

		void SetState(const Ref<Window> &window, WindowState state) { if (!IsClosed(window)) inter::window::SetState(window.get(), state); }
		void SetPointerState(const Ref<Window> &window, WindowPointerState state) { if (!IsClosed(window)) inter::window::SetPointerState(window.get(), state); }
		void Focus(const Ref<Window> &window) { if (!IsClosed(window)) inter::window::Focus(window.get()); }

		void SetVSyncMode(const Ref<Window> &window, VsyncMode mode)
		{
			window->vSyncMode = mode;
			inter::HF.renderingApi.api.SetVSync(window->renderer->handle, mode);
		}

		VsyncMode GetVSyncMode(const Ref<Window> &window) { return window->vSyncMode; }
	}
}