#include "hwindow.h"
#include "hexception.h"
#include "hyperflow.h"

namespace hf::input
{
	vec2 GetPointerPosition()	{ return GetMainWindow()->GetPointerPosition(); }
	vec2 GetPointerDelta()		{ return GetMainWindow()->GetPointerDelta(); }
	vec2 GetScrollDelta()		{ return GetMainWindow()->GetScrollDelta(); }

	KeyState GetState(Key key)			{ return GetMainWindow()->GetKeyState(key); }
	ButtonState GetState(Button button) { return GetMainWindow()->GetButtonState(button); }
	const std::string& GetWrite()		{ return GetMainWindow()->GetWrite(); }

	bool IsDown(Key key)		  { return GetMainWindow()->IsKeyDown(key); }
	bool IsDownContinues(Key key) { return GetMainWindow()->IsKeyDownContinues(key); }
	bool IsUp(Key key)			  { return GetMainWindow()->IsKeyUp(key); }

	bool IsDown(Button button)			{ return GetMainWindow()->IsButtonDown(button); }
	bool IsDownContinues(Button button) { return GetMainWindow()->IsButtonDownContinues(button); }
	bool IsUp(Button button)			{ return GetMainWindow()->IsButtonUp(button); }
}