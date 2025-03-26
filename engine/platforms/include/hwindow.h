#ifndef HWINDOW_H
#define HWINDOW_H

#include "hshared.h"

namespace hf
{
	class Keyboard;
	class Mouse;
	class Renderer;

	class Window
	{
	public:
		Window(void* platformHandle, const WindowData& data, const Ref<Window>& parent);
		~Window();

		[[nodiscard]] const std::string& GetTitle() const;
		[[nodiscard]] ivec2 GetSize() const;
		[[nodiscard]] ivec2 GetPosition() const;
		[[nodiscard]] IRect GetRect() const;
		[[nodiscard]] WindowFlags GetFlags() const;
		[[nodiscard]] WindowStyle GetStyle() const;
		[[nodiscard]] void* GetHandle() const;
		[[nodiscard]] Ref<Renderer> GetRenderer() const;

		void SetTitle(const char* title) const;
		void SetSize(ivec2 size) const;
		void SetPosition(ivec2 position) const;
		void SetRect(IRect rect) const;

		[[nodiscard]] bool IsClosing() const;

		void SetFlags(WindowFlags flags);
		void Focus() const;
		bool Close();

	private:
		struct EventData
		{
			KeyState keyStates[(uint8_t)Key::Count]{};
			ButtonState buttonStates[(uint8_t)Button::Count]{};
			std::string charData;
			ivec2 pointerPosition;
			ivec2 pointerDelta;
			vec2 scrollDelta;
		};

		struct EventCallbacks
		{
			std::vector<const InputCallback*> m_KeyCallbacks[(uint8_t)Key::Count];
			std::vector<const InputCallback*> m_ButtonCallbacks[(uint8_t)Button::Count];

			std::vector<const InputShortcut*> m_KeyShortcuts[(uint8_t)Key::Count];
			std::vector<const InputShortcut*> m_ButtonShortcuts[(uint8_t)Button::Count];

			std::vector<InputCharCallback> m_CharCallbacks;
			std::vector<InputPointerMoveCallback> m_PointerMoveCallbacks;
			std::vector<InputScrollCallback> m_ScrollCallbacks;
		};

		std::string m_Title{};
		IRect m_Rect{};
		void* m_Handle;
		WindowStyle m_Style;
		WindowFlags m_Flags;
		Ref<Window> m_Parent;

		Ref<Keyboard> m_Keyboard;
		Ref<Mouse> m_Mouse;
		EventData m_EventData;
		EventCallbacks m_Callbacks;
		Ref<Renderer> m_Renderer;

		void Update();
	};
}

#endif //HWINDOW_H
