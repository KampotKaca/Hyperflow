#include "hwindow.h"
#include "hexception.h"
#include "hyperflow.h"

namespace hf
{
	vec2 GetPointerPosition()	{ return GetPointerPosition(GetMainWindow()); }
	vec2 GetPointerDelta()		{ return GetPointerDelta(GetMainWindow()); }
	vec2 GetScrollDelta()		{ return GetScrollDelta(GetMainWindow()); }

	KeyState GetKeyState(Key key)			    { return GetKeyState(GetMainWindow(), key); }
	ButtonState GetButtonState(Button button)   { return GetButtonState(GetMainWindow(), button); }
	const std::string& GetWrite()		        { return GetWrite(GetMainWindow()); }

	bool IsKeyDown(Key key)		        { return IsKeyDown(GetMainWindow(), key); }
	bool IsKeyDownContinues(Key key)    { return IsKeyDownContinues(GetMainWindow(), key); }
	bool IsKeyUp(Key key)			    { return IsKeyUp(GetMainWindow(), key); }

	bool IsButtonDown(Button button)			{ return IsButtonDown(GetMainWindow(), button); }
	bool IsButtonDownContinues(Button button)   { return IsButtonDownContinues(GetMainWindow(), button); }
	bool IsButtonUp(Button button)			    { return IsButtonUp(GetMainWindow(), button); }
}