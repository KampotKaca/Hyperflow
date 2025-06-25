#ifndef HWINDOW_H
#define HWINDOW_H

#include "hshared.h"
#include "hrenderer.h"

namespace hf
{
	struct Window
	{
		Window(const WindowCreationInfo& info, const Ref<Window>& parent);
		~Window();

		[[nodiscard]] bool IsKeyDown(Key key) const;
		[[nodiscard]] bool IsKeyDownContinues(Key key) const;
		[[nodiscard]] bool IsKeyUp(Key key) const;

		[[nodiscard]] bool IsButtonDown(Button button) const;
		[[nodiscard]] bool IsButtonDownContinues(Button button) const;
		[[nodiscard]] bool IsButtonUp(Button button) const;

		[[nodiscard]] KeyState GetKeyState(Key key) const;
		[[nodiscard]] ButtonState GetButtonState(Button button) const;
		[[nodiscard]] const std::string& GetWrite() const;

		[[nodiscard]] vec2 GetPointerPosition() const;
		[[nodiscard]] vec2 GetPointerDelta() const;
		[[nodiscard]] vec2 GetScrollDelta() const;

		[[nodiscard]] const std::string& GetTitle() const;
		[[nodiscard]] ivec2 GetSize() const;
		[[nodiscard]] ivec2 GetPosition() const;
		[[nodiscard]] IRect GetRect() const;
		[[nodiscard]] IRect GetFrameRect() const;
		[[nodiscard]] WindowState GetState() const;
		[[nodiscard]] WindowStyle GetStyle() const;
		[[nodiscard]] WindowPointerState GetPointerState() const;
		[[nodiscard]] void* GetHandle() const;
		[[nodiscard]] Ref<Renderer> GetRenderer() const;
		[[nodiscard]] bool IsClosed() const;
		[[nodiscard]] VsyncMode GetVSyncMode() const;

		void SetTitle(const std::string& title);
		void SetSize(ivec2 size) const;
		void SetPosition(ivec2 position) const;
		void SetRect(IRect rect) const;
		void SetState(WindowState state);
		void SetPointerState(WindowPointerState state);

		void Focus() const;
		void SetVSyncMode(VsyncMode mode);
		bool SetIcons(const char* folderPath) const;

		void Close();

		static Ref<Window> Open(const WindowCreationInfo &data, const Ref<Window> &parent);
		static void CloseAll();

		struct EventData
		{
			KeyState keyStates[(uint32_t)Key::MAX]{};
			ButtonState buttonStates[(uint32_t)Button::MAX]{};
			std::string charData{};
			vec2 pointerPosition{};
			vec2 pointerDelta{};
			vec2 scrollDelta{};
		};

		void* handle{};
		std::string title{};
		IRect rect{};
		WindowStyle style{};
		WindowState state{};
		WindowPointerState pointerState = WindowPointerState::Normal;
		WindowEventFlags eventFlags{};
		VsyncMode vSyncMode = VsyncMode::Relaxed;
		Ref<Window> parent{};

		EventData eventData{};
		Ref<Renderer> renderer{};

		RendererEventInfo rnEventInfo{};
	};
}

#endif //HWINDOW_H
