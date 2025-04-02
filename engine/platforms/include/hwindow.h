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
		Window(const WindowData& data, const Ref<Window>& parent);
		~Window();

		[[nodiscard]] const std::string& GetTitle() const;
		[[nodiscard]] ivec2 GetSize() const;
		[[nodiscard]] ivec2 GetPosition() const;
		[[nodiscard]] IRect GetRect() const;
		[[nodiscard]] WindowFlags GetFlags() const;
		[[nodiscard]] WindowStyle GetStyle() const;
		[[nodiscard]] void* GetHandle() const;
		[[nodiscard]] const Renderer* GetRenderer() const;

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

		std::string m_Title{};
		IRect m_Rect{};
		void* m_Handle;
		WindowStyle m_Style;
		WindowFlags m_Flags;
		Ref<Window> m_Parent;

		Ref<Keyboard> m_Keyboard;
		Ref<Mouse> m_Mouse;
		EventData m_EventData;
		Renderer* m_Renderer;

		void Update();
	};
}

#endif //HWINDOW_H
