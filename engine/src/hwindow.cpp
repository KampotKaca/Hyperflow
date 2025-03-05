#define private public
#include "hwindow.h"
#undef private

namespace hf
{
	void SubscribeOnKey(Ref<Window> window, KeySubscriptionData* data)
	{
		window->m_KeyEventSubscriptions.push_back(data);
	}

	void UnsubscribeOnKey(Ref<Window> window, KeySubscriptionData* data)
	{
		std::remove(window->m_KeyEventSubscriptions.begin(), window->m_KeyEventSubscriptions.end(), data);
	}

	void Window::Close()
	{
		m_ShouldClose = true;
	}
}