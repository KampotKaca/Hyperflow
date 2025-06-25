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

		if (info.iconFolderPath) inter::rendering::SetWindowIcons_i(this, info.iconFolderPath);
	}

	Window::~Window()
	{
		inter::window::Close(this);
	}

	bool Window::IsKeyDown(Key key)			 const	{ return GetKeyState(key) == KeyState::Down; }
	bool Window::IsKeyDownContinues(Key key) const  { return GetKeyState(key) == KeyState::DownContinues; }
	bool Window::IsKeyUp(Key key)			 const	{ return GetKeyState(key) == KeyState::Up; }

	bool Window::IsButtonDown(Button button)		  const	{ return GetButtonState(button) == ButtonState::Down; }
	bool Window::IsButtonDownContinues(Button button) const { return GetButtonState(button) == ButtonState::DownContinues; }
	bool Window::IsButtonUp(Button button)			  const	{ return GetButtonState(button) == ButtonState::Up; }

	KeyState Window::GetKeyState(Key key)			  const	{ return eventData.keyStates[(uint32_t)key]; }
	ButtonState Window::GetButtonState(Button button) const { return eventData.buttonStates[(uint32_t)button]; }
	const std::string& Window::GetWrite()			  const	{ return eventData.charData; }

	vec2 Window::GetPointerPosition() const { return eventData.pointerPosition; }
	vec2 Window::GetPointerDelta()	  const { return eventData.pointerDelta; }
	vec2 Window::GetScrollDelta()	  const { return eventData.scrollDelta; }

	const std::string& Window::GetTitle() const { return title; }
	ivec2 Window::GetSize() const { return inter::window::GetSize(this); }
	ivec2 Window::GetPosition() const { return inter::window::GetPosition(this); }

	IRect Window::GetRect() const
	{
		IRect result{};
		result.position = inter::window::GetPosition(this);
		result.size = inter::window::GetSize(this);
		return result;
	}

	IRect Window::GetFrameRect()				 const { return inter::window::GetFrameRect(this); }
	WindowState Window::GetState() 				 const { return state; }
	WindowStyle Window::GetStyle() 				 const { return style; }
	WindowPointerState Window::GetPointerState() const { return pointerState; }
	void* Window::GetHandle()					 const { return handle; }
	Ref<Renderer> Window::GetRenderer()			 const { return renderer; }
	bool Window::IsClosed()						 const { return handle == nullptr; }
	VsyncMode Window::GetVSyncMode()			 const { return vSyncMode; }

	void Window::SetTitle(const std::string& title) 	   { if (!IsClosed()) inter::window::SetTitle(this, title); }
	void Window::SetSize(ivec2 size) const				   { if (!IsClosed()) inter::window::SetSize(this, size); }
	void Window::SetPosition(ivec2 position) const  	   { if (!IsClosed()) inter::window::SetPosition(this, position); }
	void Window::SetRect(IRect rect) const				   { if (!IsClosed()) inter::window::SetRect(this, rect); }
	void Window::SetState(WindowState state)			   { if (!IsClosed()) inter::window::SetState(this, state); }
	void Window::SetPointerState(WindowPointerState state) { if (!IsClosed()) inter::window::SetPointerState(this, state); }

	void Window::Focus() const { if (!IsClosed()) inter::window::Focus(this); }

	void Window::SetVSyncMode(VsyncMode mode)
	{
		vSyncMode = mode;
		inter::HF.renderingApi.api.SetVSync(renderer->handle, mode);
	}

	bool Window::SetIcons(const char* folderPath) const { return inter::rendering::SetWindowIcons_i(this, folderPath); }
	void Window::Close() { if (inter::window::Close(this)) inter::HF.windows.erase((uint64_t)handle); }

	void Window::CloseAll()
	{
		for (auto& win : inter::HF.windows | std::views::values)
			inter::window::Close(win.get());
		inter::HF.windows.clear();
	}

	Ref<Window> Window::Open(const WindowCreationInfo &data, const Ref<Window> &parent)
	{
		auto newWindow = MakeRef<Window>(data, parent);
		inter::HF.windows[(uint64_t)newWindow->handle] = newWindow;

		if (inter::HF.renderingApi.type != RenderingApiType::None)
		{
			newWindow->renderer = MakeRef<Renderer>(newWindow.get(), newWindow->rnEventInfo);
			inter::rendering::RunRenderThread_i(newWindow->renderer);
		}
		return newWindow;
	}
}