#include "hinternal.h"
#include "hyperflow.h"
#include "hwindow.h"
#include "hrenderer.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
	Window::Window(const WindowCreationInfo& info)
		: title(info.title), style(info.style), pointerState(info.pointerState),
          vSyncMode(info.vsyncMode), rnEventInfo(info.rnEventInfo)
	{
		rect.position = info.position;
		rect.size     = info.size;

		ir::win::Open(this, info);
		eventData.pointerPosition = ir::platform::GetPointerPosition(this);

		ir::win::SetEventFlags(this, info.eventFlags);
		ir::win::SetState(this, WindowState::Restored);
		ir::win::Focus(this);
		ir::win::SetTitle(this, info.title);
		ir::win::SetTitle(this, info.title);
	    ir::win::SetPointerState(this, info.pointerState);

		if (info.iconFolderPath) ir::rdr::SetWindowIcons_i(this, info.iconFolderPath);
	}

	Window::~Window()
	{
		ir::win::Close(this);
	}

	const std::string& GetWindowTitle() { return ir::HF.window->title; }
	ivec2 GetWindowSize()				{ return ir::win::GetSize(ir::HF.window.get()); }
	ivec2 GetWindowPosition()			{ return ir::win::GetPosition(ir::HF.window.get()); }

	IRect GetWindowRect()
	{
		IRect result{};
		result.position = ir::win::GetPosition(ir::HF.window.get());
		result.size = ir::win::GetSize(ir::HF.window.get());
		return result;
	}

	IRect GetWindowFrameRect()				   { return ir::win::GetFrameRect(ir::HF.window.get()); }
	WindowState GetWindowState() 			   { return ir::HF.window->state; }
	WindowStyle GetWindowStyle() 			   { return ir::HF.window->style; }
	WindowPointerState GetWindowPointerState() { return ir::HF.window->pointerState; }
	void* GetWindowHandle()					   { return ir::HF.window->handle; }
	bool IsWindowClosed()					   { return ir::HF.window->handle == nullptr; }
	VsyncMode GetVSyncMode()			       { return ir::HF.window->vSyncMode; }

	void SetWindowTitle(const std::string& title) 	     { if (!IsWindowClosed()) ir::win::SetTitle(ir::HF.window.get(), title); }
	void SetWindowSize(ivec2 size)				         { if (!IsWindowClosed()) ir::win::SetSize(ir::HF.window.get(), size); }
	void SetWindowPosition(ivec2 position)  		     { if (!IsWindowClosed()) ir::win::SetPosition(ir::HF.window.get(), position); }
	void SetWindowRect(IRect rect)					     { if (!IsWindowClosed()) ir::win::SetRect(ir::HF.window.get(), rect); }
	void SetWindowState(WindowState state)			     { if (!IsWindowClosed()) ir::win::SetState(ir::HF.window.get(), state); }
	void SetWindowPointerState(WindowPointerState state) { if (!IsWindowClosed()) ir::win::SetPointerState(ir::HF.window.get(), state); }

	void FocusWindow() { if (!IsWindowClosed()) ir::win::Focus(ir::HF.window.get()); }

	void SetVSyncMode(VsyncMode mode)
	{
		ir::HF.window->vSyncMode = mode;
		ir::HF.renderingApi.api.SetVSync(ir::HF.renderer->handle, mode);
	}

	bool SetWindowIcons(const char* folderPath) { return ir::rdr::SetWindowIcons_i(ir::HF.window.get(), folderPath); }
}