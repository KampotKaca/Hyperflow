#ifndef HWINDOW_H
#define HWINDOW_H

#include "hshared.h"
#include "hkeyboard.h"
#include "hmouse.h"

namespace hf
{
	class Window
	{
	public:
		Window(const WindowData& data, const Ref<Window>& parent);
		~Window();

		[[nodiscard]] std::string GetTitle() const;
		[[nodiscard]] glm::ivec2 GetSize() const;
		[[nodiscard]] glm::ivec2 GetPosition() const;
		[[nodiscard]] IRect GetRect() const;
		[[nodiscard]] WindowFlags GetFlags() const;
		[[nodiscard]] WindowStyle GetStyle() const;

		void SetTitle(const char* title) const;
		void SetSize(glm::ivec2 size);
		void SetPosition(glm::ivec2 position) const;
		void SetRect(IRect rect);

		[[nodiscard]] bool IsClosing() const;

		void SetFlags(WindowFlags flags);
		void Focus() const;

		Keyboard keyboard;
		Mouse mouse;
		
	private:
		std::string m_Title;
		IRect m_Rect;
		void* m_Handle;
		WindowStyle m_Style;
		WindowFlags m_Flags;
		bool m_ShouldClose;
		Ref<Window> m_Parent;
	};
}

#endif //HWINDOW_H
