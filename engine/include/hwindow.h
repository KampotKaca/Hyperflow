#ifndef HWINDOW_H
#define HWINDOW_H

#include "hshared.h"

namespace hf
{
	class Window
	{
	public:
		Window(const WindowData& data, hRef<Window> parent);
		~Window();

		glm::ivec2 GetSize();
		glm::ivec2 GetPosition();
		IRect GetRect();
		WindowFlags GetFlags();
		WindowStyle GetStyle();

		void SetSize(glm::ivec2 size);
		void SetPosition(glm::ivec2 position);
		void SetRect(IRect rect);

		bool IsClosing() const;

		void SetFlags(WindowFlags flags);
		void Focus();

		void Close();

	private:
		std::string m_Title;
		void* m_Handle;
		WindowStyle m_Style;
		WindowFlags m_Flags;
		bool m_ShouldClose;
		hRef<Window> m_Parent;
		std::string m_WriteRecords;
		std::vector<KeySubscriptionData*> m_KeyEventSubscriptions;
		KeyCode m_PrevKeyState;
	};
}

#endif //HWINDOW_H
