#define private public
#include "components/windowhandling/hwindow.h"
#undef private

#include "hinput.h"
#include "hexception.h"
#include "hyperflow.h"

namespace hf
{
	KeyState Input::GetState(Ref<Window> window, Key key)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		if(key == Key::None) LOG_WARN("Redundant Key State Request!");
		return (KeyState)window->m_EventData.keyStates[(uint8_t)key];
	}

	ButtonState Input::GetState(Ref<Window> window, Button button)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		if(button == Button::None) LOG_WARN("Redundant Button State Request!");
		return (KeyState)window->m_EventData.buttonStates[(uint8_t)button];
	}

	std::string Input::GetWrite(Ref<Window> window)
	{
		if(!window) throw NULL_REF_EXCEPTION(Window, window);
		return window->m_EventData.charData;
	}

	KeyState Input::GetState(Key key) { return GetState(Hyperflow::MainWindow(), key); }
	ButtonState Input::GetState(Button button) { return GetState(Hyperflow::MainWindow(), button); }
	std::string Input::GetWrite() { return GetWrite(Hyperflow::MainWindow()); }

#define W_FUNC(type, state) bool type(Ref<Window> window, Key key) { return GetState(window, key) == state; }

	W_FUNC(Input::IsDown, KeyState::Down)
	W_FUNC(Input::IsDownContinues, KeyState::DownContinues)
	W_FUNC(Input::IsUp, KeyState::Up)

#undef W_FUNC

#define W_FUNC(type, state) bool type(Ref<Window> window, Button button) { return GetState(window, button) == state; }

	W_FUNC(Input::IsDown, ButtonState ::Down)
	W_FUNC(Input::IsDownContinues, ButtonState::DownContinues)
	W_FUNC(Input::IsUp, ButtonState::Up)

#undef W_FUNC

#define N_FUNC(func) bool Input::func(Key key) { return func(Hyperflow::MainWindow(), key); }

	N_FUNC(IsDown)
	N_FUNC(IsDownContinues)
	N_FUNC(IsUp)

#undef N_FUNC

#define N_FUNC(func) bool Input::func(Button button) { return func(Hyperflow::MainWindow(), button); }

	N_FUNC(IsDown)
	N_FUNC(IsDownContinues)
	N_FUNC(IsUp)

#undef N_FUNC
}