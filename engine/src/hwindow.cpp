#include "hinternal.h"
#include "hyperflow.h"
#include "hwindow.h"
#include "hrenderer.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
	Window::Window(const WindowCreationInfo& info, const Ref<Window>& parent)
		: title(info.title), style(info.style), pointerState(info.pointerState),
          vSyncMode(info.vsyncMode), parent(parent), rnEventInfo(info.rnEventInfo)
	{
		rect.position = info.position;
		rect.size     = info.size;
		renderer = nullptr;

		ir::window::Open(this, info);
		eventData.pointerPosition = ir::platform::GetPointerPosition(this);

		ir::window::SetEventFlags(this, info.eventFlags);
		ir::window::SetState(this, WindowState::Restored);
		ir::window::Focus(this);
		ir::window::SetTitle(this, info.title);
		ir::window::SetTitle(this, info.title);
	    ir::window::SetPointerState(this, info.pointerState);

		if (info.iconFolderPath) ir::rdr::SetWindowIcons_i(this, info.iconFolderPath);
	}

	Window::~Window()
	{
		ir::window::Close(this);
	}

	bool IsKeyDown(const Ref<Window>& win, Key key)			 { return GetKeyState(win, key) == KeyState::Down; }
	bool IsKeyDownContinues(const Ref<Window>& win, Key key) { return GetKeyState(win, key) == KeyState::DownContinues; }
	bool IsKeyUp(const Ref<Window>& win, Key key)			 { return GetKeyState(win, key) == KeyState::Up; }

	bool IsButtonDown(const Ref<Window>& win, Button button)		  { return GetButtonState(win, button) == ButtonState::Down; }
	bool IsButtonDownContinues(const Ref<Window>& win, Button button) { return GetButtonState(win, button) == ButtonState::DownContinues; }
	bool IsButtonUp(const Ref<Window>& win, Button button)			  { return GetButtonState(win, button) == ButtonState::Up; }

	KeyState GetKeyState(const Ref<Window>& win, Key key)			  { return win->eventData.keyStates[(uint32_t)key]; }
	ButtonState GetButtonState(const Ref<Window>& win, Button button) { return win->eventData.buttonStates[(uint32_t)button]; }
	const std::string& GetWrite(const Ref<Window>& win)			      { return win->eventData.charData; }

	vec2 GetPointerPosition(const Ref<Window>& win) { return win->eventData.pointerPosition; }
	vec2 GetPointerDelta(const Ref<Window>& win)	{ return win->eventData.pointerDelta; }
	vec2 GetScrollDelta(const Ref<Window>& win)	    { return win->eventData.scrollDelta; }

	const std::string& GetTitle(const Ref<Window>& win) { return win->title; }
	ivec2 GetSize(const Ref<Window>& win)				{ return ir::window::GetSize(win.get()); }
	ivec2 GetPosition(const Ref<Window>& win)			{ return ir::window::GetPosition(win.get()); }

	IRect GetRect(const Ref<Window>& win)
	{
		IRect result{};
		result.position = ir::window::GetPosition(win.get());
		result.size = ir::window::GetSize(win.get());
		return result;
	}

	IRect GetFrameRect(const Ref<Window>& win)				   { return ir::window::GetFrameRect(win.get()); }
	WindowState GetState(const Ref<Window>& win) 			   { return win->state; }
	WindowStyle GetStyle(const Ref<Window>& win) 			   { return win->style; }
	WindowPointerState GetPointerState(const Ref<Window>& win) { return win->pointerState; }
	void* GetHandle(const Ref<Window>& win)					   { return win->handle; }
	Ref<Renderer> GetRenderer(const Ref<Window>& win)		   { return win->renderer; }
	bool IsClosed(const Ref<Window>& win)					   { return win->handle == nullptr; }
	VsyncMode GetVSyncMode(const Ref<Window>& win)			   { return win->vSyncMode; }

	void SetTitle(const Ref<Window>& win, const std::string& title) 	   { if (!IsClosed(win)) ir::window::SetTitle(win.get(), title); }
	void SetSize(const Ref<Window>& win, ivec2 size)				       { if (!IsClosed(win)) ir::window::SetSize(win.get(), size); }
	void SetPosition(const Ref<Window>& win, ivec2 position)  			   { if (!IsClosed(win)) ir::window::SetPosition(win.get(), position); }
	void SetRect(const Ref<Window>& win, IRect rect)					   { if (!IsClosed(win)) ir::window::SetRect(win.get(), rect); }
	void SetState(const Ref<Window>& win, WindowState state)			   { if (!IsClosed(win)) ir::window::SetState(win.get(), state); }
	void SetPointerState(const Ref<Window>& win, WindowPointerState state) { if (!IsClosed(win)) ir::window::SetPointerState(win.get(), state); }

	void Focus(const Ref<Window>& win) { if (!IsClosed(win)) ir::window::Focus(win.get()); }
	void Destroy(const Ref<Window>& win) { if (ir::window::Close(win.get())) ir::HF.windows.erase((uint64_t)win.get()); }

	void SetVSyncMode(const Ref<Window>& win, VsyncMode mode)
	{
		win->vSyncMode = mode;
		ir::HF.renderingApi.api.SetVSync(win->renderer->handle, mode);
	}

	bool SetIcons(const Ref<Window>& win, const char* folderPath) { return ir::rdr::SetWindowIcons_i(win.get(), folderPath); }

	void DestroyAllWindows()
	{
		for (auto& win : ir::HF.windows | std::views::values)
			ir::window::Close(win.get());
		ir::HF.windows.clear();
	}

	Ref<Window> Create(const WindowCreationInfo &data, const Ref<Window> &parent)
	{
		auto newWindow = MakeRef<Window>(data, parent);
		ir::HF.windows[(uint64_t)newWindow->handle] = newWindow;

		if (ir::HF.renderingApi.type != RenderingApiType::None)
		{
			newWindow->renderer = MakeRef<Renderer>(newWindow.get(), newWindow->rnEventInfo);
			ir::rdr::RunRenderThread_i(newWindow->renderer);
		}
		return newWindow;
	}
}