#define private public
#include "hwindow.h"
#undef private

#include "hexception.h"
#include "hyperflow.h"

namespace hf::input
{
	ivec2 GetPointerPosition(const Ref<Window> &window)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		return window->m_EventData.pointerPosition;
	}
	
	ivec2 GetPointerDelta(const Ref<Window> &window)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		return window->m_EventData.pointerDelta;
	}
	
	vec2 GetScrollDelta(const Ref<Window> &window)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		return window->m_EventData.scrollDelta;
	}

	KeyState GetState(const Ref<Window> &window, Key key)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		if(key == Key::None) LOG_WARN("Redundant Key State Request!");
		return (KeyState)window->m_EventData.keyStates[(uint8_t)key];
	}

	ButtonState GetState(const Ref<Window> &window, Button button)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		if(button == Button::None) LOG_WARN("Redundant Button State Request!");
		return (KeyState)window->m_EventData.buttonStates[(uint8_t)button];
	}

	const std::string& GetWrite(const Ref<Window> &window)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		return window->m_EventData.charData;
	}

	ivec2 GetPointerPosition() { return GetPointerPosition(GetMainWindow()); }
	ivec2 GetPointerDelta() { return GetPointerDelta(GetMainWindow()); }
	vec2 GetScrollDelta() { return GetScrollDelta(GetMainWindow()); }

	KeyState GetState(Key key) { return GetState(GetMainWindow(), key); }
	ButtonState GetState(Button button) { return GetState(GetMainWindow(), button); }
	const std::string& GetWrite() { return GetWrite(GetMainWindow()); }

	bool IsDown(const Ref<Window> &window, Key key) { return GetState(window, key) == KeyState::Down; }
	bool IsDownContinues(const Ref<Window> &window, Key key) { return GetState(window, key) == KeyState::DownContinues; }
	bool IsUp(const Ref<Window> &window, Key key) { return GetState(window, key) == KeyState::Up; }

	bool IsDown(const Ref<Window> &window, Button button) { return GetState(window, button) == ButtonState ::Down; }
	bool IsDownContinues(const Ref<Window> &window, Button button) { return GetState(window, button) == ButtonState::DownContinues; }
	bool IsUp(const Ref<Window> &window, Button button) { return GetState(window, button) == ButtonState::Up; }

	bool IsDown(Key key) { return IsDown(GetMainWindow(), key); }
	bool IsDownContinues(Key key) { return IsDownContinues(GetMainWindow(), key); }
	bool IsUp(Key key) { return IsUp(GetMainWindow(), key); }

	bool IsDown(Button button) { return IsDown(GetMainWindow(), button); }
	bool IsDownContinues(Button button) { return IsDownContinues(GetMainWindow(), button); }
	bool IsUp(Button button) { return IsUp(GetMainWindow(), button); }
}