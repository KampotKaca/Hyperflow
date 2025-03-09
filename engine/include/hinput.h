#ifndef HINPUT_H
#define HINPUT_H

#include "hshared.h"

namespace hf
{
	class Window;

	class Input
	{
	public:
		static bool IsDown(Ref<Window> window, Key key);
		static bool IsDownContinues(Ref<Window> window, Key key);
		static bool IsUp(Ref<Window> window, Key key);

		static bool IsDown(Ref<Window> window, Button button);
		static bool IsDownContinues(Ref<Window> window, Button button);
		static bool IsUp(Ref<Window> window, Button button);

		static bool IsDown(Key key);
		static bool IsDownContinues(Key key);
		static bool IsUp(Key key);

		static bool IsDown(Button button);
		static bool IsDownContinues(Button button);
		static bool IsUp(Button button);

		static KeyState GetState(Key key);
		static ButtonState GetState(Button button);
		static std::string GetWrite();

		static KeyState GetState(Ref<Window> window, Key key);
		static ButtonState GetState(Ref<Window> window, Button button);
		static std::string GetWrite(Ref<Window> window);
	};
}

#endif //HINPUT_H
