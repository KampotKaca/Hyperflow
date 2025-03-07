#ifndef HWINDOW_H
#define HWINDOW_H

#include "hshared.h"

namespace hf
{
	class Window
	{
	public:
		Window(const WindowData& data, const Ref<Window>& parent);
		~Window();

		[[nodiscard]] glm::ivec2 GetSize() const;
		[[nodiscard]] glm::ivec2 GetPosition() const;
		[[nodiscard]] IRect GetRect() const;
		[[nodiscard]] WindowFlags GetFlags() const;
		[[nodiscard]] WindowStyle GetStyle() const;

		void SetSize(glm::ivec2 size) const;
		void SetPosition(glm::ivec2 position) const;
		void SetRect(IRect rect);

		[[nodiscard]] bool IsClosing() const;

		void SetFlags(WindowFlags flags);
		void Focus() const;

	private:
		std::string m_Title;
		void* m_Handle;
		WindowStyle m_Style;
		WindowFlags m_Flags;
		bool m_ShouldClose;
		Ref<Window> m_Parent;
		std::string m_WriteRecords;
		std::vector<KeySubscriptionData*> m_KeyEventSubscriptions;
		KeyCode m_PrevKeyState{};
	};

	void SubscribeOnKey(Ref<Window> window, KeySubscriptionData* data);
	void UnsubscribeOnKey(Ref<Window> window, KeySubscriptionData* data);
}

#endif //HWINDOW_H
