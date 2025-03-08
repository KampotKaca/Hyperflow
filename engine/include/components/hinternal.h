#ifndef HINTERNAL_H
#define HINTERNAL_H

#include "components/windowhandling/hwindow.h"

namespace hf
{
	void LoadTime();
	void UpdateTime();
	
	void OnKeyPressed(Keyboard& keyboard, Key key) noexcept;
	void OnKeyReleased(Keyboard& keyboard, Key key) noexcept;
	void OnChar(Keyboard& keyboard, char character) noexcept;
	
	void OnButtonPressed(Mouse& mouse, Button button) noexcept;
	void OnButtonReleased(Mouse& mouse, Button button) noexcept;
	void OnMouseMoved(Mouse& mouse, glm::ivec2 position) noexcept;
	void OnMouseWheelScroll(Mouse& mouse, glm::vec2 scroll) noexcept;
}

#endif //HINTERNAL_H
