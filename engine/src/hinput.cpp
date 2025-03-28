#define private public
#include "hwindow.h"
#undef private

#include "hinput.h"
#include "hexception.h"
#include "hyperflow.h"

namespace hf
{
	//region Update handled Input

	ivec2 Input::GetPointerPosition(const Ref<Window> &window)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		return window->m_EventData.pointerPosition;
	}
	
	ivec2 Input::GetPointerDelta(const Ref<Window> &window)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		return window->m_EventData.pointerDelta;
	}
	
	vec2 Input::GetScrollDelta(const Ref<Window> &window)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		return window->m_EventData.scrollDelta;
	}

	KeyState Input::GetState(const Ref<Window> &window, Key key)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		if(key == Key::None) LOG_WARN("Redundant Key State Request!");
		return (KeyState)window->m_EventData.keyStates[(uint8_t)key];
	}

	ButtonState Input::GetState(const Ref<Window> &window, Button button)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		if(button == Button::None) LOG_WARN("Redundant Button State Request!");
		return (KeyState)window->m_EventData.buttonStates[(uint8_t)button];
	}

	const std::string& Input::GetWrite(const Ref<Window> &window)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		return window->m_EventData.charData;
	}

	ivec2 Input::GetPointerPosition() { return GetPointerPosition(Hyperflow::MainWindow()); }
	ivec2 Input::GetPointerDelta() { return GetPointerDelta(Hyperflow::MainWindow()); }
	vec2 Input::GetScrollDelta() { return GetScrollDelta(Hyperflow::MainWindow()); }

	KeyState Input::GetState(Key key) { return GetState(Hyperflow::MainWindow(), key); }
	ButtonState Input::GetState(Button button) { return GetState(Hyperflow::MainWindow(), button); }
	const std::string& Input::GetWrite() { return GetWrite(Hyperflow::MainWindow()); }

	bool Input::IsDown(const Ref<Window> &window, Key key) { return GetState(window, key) == KeyState::Down; }
	bool Input::IsDownContinues(const Ref<Window> &window, Key key) { return GetState(window, key) == KeyState::DownContinues; }
	bool Input::IsUp(const Ref<Window> &window, Key key) { return GetState(window, key) == KeyState::Up; }

	bool Input::IsDown(const Ref<Window> &window, Button button) { return GetState(window, button) == ButtonState ::Down; }
	bool Input::IsDownContinues(const Ref<Window> &window, Button button) { return GetState(window, button) == ButtonState::DownContinues; }
	bool Input::IsUp(const Ref<Window> &window, Button button) { return GetState(window, button) == ButtonState::Up; }

	bool Input::IsDown(Key key) { return IsDown(Hyperflow::MainWindow(), key); }
	bool Input::IsDownContinues(Key key) { return IsDownContinues(Hyperflow::MainWindow(), key); }
	bool Input::IsUp(Key key) { return IsUp(Hyperflow::MainWindow(), key); }

	bool Input::IsDown(Button button) { return IsDown(Hyperflow::MainWindow(), button); }
	bool Input::IsDownContinues(Button button) { return IsDownContinues(Hyperflow::MainWindow(), button); }
	bool Input::IsUp(Button button) { return IsUp(Hyperflow::MainWindow(), button); }

	//endregion

	//region Event Handled Input

	void Input::Subscribe(const Ref<Window> &window, const InputCallback *callback)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		if(!callback) throw NULL_REF_EXCEPTION(InputCallback, callback);

		for (auto& key : callback->keys)
			window->m_Callbacks.m_KeyCallbacks[(uint8_t)key].push_back(callback);

		for (auto& button : callback->buttons)
			window->m_Callbacks.m_ButtonCallbacks[(uint8_t)button].push_back(callback);
	}

	void Input::Subscribe(const Ref<Window> &window, const InputShortcut *shortcut)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		if(!shortcut) throw NULL_REF_EXCEPTION(InputShortcut, shortcut);

		for (auto& key : shortcut->keys)
			window->m_Callbacks.m_KeyShortcuts[(uint8_t)key].push_back(shortcut);

		for (auto& button : shortcut->buttons)
			window->m_Callbacks.m_ButtonShortcuts[(uint8_t)button].push_back(shortcut);
	}

#define SUBSCRIBE_NORMAL(n, vec, c)\
	void Input::n(const Ref<Window> &window, c callback)\
	{\
		if(!window) throw NullReferenceException(__LINE__, __FILE__, "Window", "window");\
        if(window->IsClosing()) return;\
		if(!callback) throw NullReferenceException(__LINE__, __FILE__, #c, "callback");\
		window->m_Callbacks.vec.push_back(callback);\
	}

	SUBSCRIBE_NORMAL(SubscribeChar, m_CharCallbacks, InputCharCallback)
	SUBSCRIBE_NORMAL(SubscribePointerMove, m_PointerMoveCallbacks, InputPointerMoveCallback)
	SUBSCRIBE_NORMAL(SubscribeScroll, m_ScrollCallbacks, InputScrollCallback)

#undef SUBSCRIBE_NORMAL

	void Input::Unsubscribe(const Ref<Window> &window, const InputCallback *callback)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		if(!callback) throw NULL_REF_EXCEPTION(InputCallback, callback);

		for (auto& key : callback->keys)
		{
			auto& v = window->m_Callbacks.m_KeyCallbacks[(uint8_t)key];
			auto res = std::remove_if(v.begin(), v.end(), [&](const auto &item)
			{ return item == callback; });
			if (res == v.end()) LOG_WARN("Unable to unsubscibe");
		}

		for (auto& button : callback->buttons)
		{
			auto& v = window->m_Callbacks.m_ButtonCallbacks[(uint8_t)button];
			auto res = std::remove_if(v.begin(), v.end(), [&](const auto &item)
			{ return item == callback; });
			if (res == v.end()) LOG_WARN("Unable to unsubscibe");
		}
	}

	void Input::Unsubscribe(const Ref<Window> &window, const InputShortcut *shortcut)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		if(!shortcut) throw NULL_REF_EXCEPTION(InputShortcut, shortcut);

		for (auto& key : shortcut->keys)
		{
			auto& v = window->m_Callbacks.m_KeyShortcuts[(uint8_t)key];
			auto res = std::remove_if(v.begin(), v.end(), [&](const auto &item)
			{ return item == shortcut; });
			if (res == v.end()) LOG_WARN("Unable to unsubscibe");
		}

		for (auto& button : shortcut->buttons)
		{
			auto& v = window->m_Callbacks.m_ButtonShortcuts[(uint8_t)button];
			auto res = std::remove_if(v.begin(), v.end(), [&](const auto &item)
			{ return item == shortcut; });
			if (res == v.end()) LOG_WARN("Unable to unsubscibe");
		}
	}

#define UNSUBSCRIBE_NORMAL(n, vec, c)\
	void Input::n(const Ref<Window> &window, c callback)\
	{\
		if(!window) throw NullReferenceException(__LINE__, __FILE__, "Window", "window");\
        if(window->IsClosing()) return;\
		if(!callback) throw NullReferenceException(__LINE__, __FILE__, #c, "callback");\
		auto& v = window->m_Callbacks.vec;\
		std::remove_if(v.begin(), v.end(), [&](const auto &item)\
		{ return item == callback; });\
	}

	UNSUBSCRIBE_NORMAL(UnsubscribeChar, m_CharCallbacks, InputCharCallback)
	UNSUBSCRIBE_NORMAL(UnsubscribePointerMove, m_PointerMoveCallbacks, InputPointerMoveCallback)
	UNSUBSCRIBE_NORMAL(UnsubscribeScroll, m_ScrollCallbacks, InputScrollCallback)

#undef UNSUBSCRIBE_NORMAL

	void Input::Subscribe(const InputCallback *callback) { Subscribe(Hyperflow::MainWindow(), callback); }
	void Input::Subscribe(const InputShortcut *shortcut) { Subscribe(Hyperflow::MainWindow(), shortcut); }
	void Input::SubscribeChar(InputCharCallback callback) { SubscribeChar(Hyperflow::MainWindow(), callback); }
	void Input::SubscribePointerMove(InputPointerMoveCallback callback) { SubscribePointerMove(Hyperflow::MainWindow(), callback); }
	void Input::SubscribeScroll(InputScrollCallback callback) { SubscribeScroll(Hyperflow::MainWindow(), callback); }

	void Input::Unsubscribe(const InputCallback *callback) { Unsubscribe(Hyperflow::MainWindow(), callback); }
	void Input::Unsubscribe(const InputShortcut *shortcut) { Unsubscribe(Hyperflow::MainWindow(), shortcut); }
	void Input::UnsubscribeChar(InputCharCallback callback) { UnsubscribeChar(Hyperflow::MainWindow(), callback); }
	void Input::UnsubscribePointerMove(InputPointerMoveCallback callback) { UnsubscribePointerMove(Hyperflow::MainWindow(), callback); }
	void Input::UnsubscribeScroll(InputScrollCallback callback) { UnsubscribeScroll(Hyperflow::MainWindow(), callback); }

	//endregion
}