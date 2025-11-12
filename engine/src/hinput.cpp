#include "hwindow.h"
#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    bool IsKeyDown(Key key)			 { return GetKeyState(key) == KeyState::Down; }
    bool IsKeyDownContinues(Key key) { return GetKeyState(key) == KeyState::DownContinues; }
    bool IsKeyUp(Key key)			 { return GetKeyState(key) == KeyState::Up; }

    bool IsButtonDown(Button button)		  { return GetButtonState(button) == ButtonState::Down; }
    bool IsButtonDownContinues(Button button) { return GetButtonState(button) == ButtonState::DownContinues; }
    bool IsButtonUp(Button button)			  { return GetButtonState(button) == ButtonState::Up; }

    KeyState GetKeyState(Key key)			  { return ir::HF.window->eventData.keyStates[(uint32_t)key]; }
    ButtonState GetButtonState(Button button) { return ir::HF.window->eventData.buttonStates[(uint32_t)button]; }
    const std::string& GetWrite()			  { return ir::HF.window->eventData.charData; }

    vec2 GetPointerPosition() { return ir::HF.window->eventData.pointerPosition; }
    vec2 GetPointerDelta()	  { return ir::HF.window->eventData.pointerDelta; }
    vec2 GetScrollDelta()	  { return ir::HF.window->eventData.scrollDelta; }
}