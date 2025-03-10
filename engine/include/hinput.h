#ifndef HINPUT_H
#define HINPUT_H

#include "hshared.h"

namespace hf
{
	class Window;

	template<typename T>
	struct InputCallback
	{

	};

	typedef InputCallback<Key> KeyCallback;
	typedef InputCallback<Button> ButtonCallback;

	template<typename T>
	struct InputShortcut
	{

	};

	typedef InputShortcut<Key> KeyShortcut;
	typedef InputShortcut<Button> ButtonShortcut;

	typedef void (*InputCharCallback)(std::string str);
	typedef void (*InputPointerMoveCallback)(glm::ivec2 delta);
	typedef void (*InputScrollCallback)(glm::ivec2 delta);

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
		static const std::string& GetWrite();

		static glm::ivec2 GetPointerPosition();
		static glm::ivec2 GetPointerDelta();
		static glm::vec2 GetScrollDelta();

		static KeyState GetState(Ref<Window> window, Key key);
		static ButtonState GetState(Ref<Window> window, Button button);
		static const std::string& GetWrite(Ref<Window> window);

		static glm::ivec2 GetPointerPosition(Ref<Window> window);
		static glm::ivec2 GetPointerDelta(Ref<Window> window);
		static glm::vec2 GetScrollDelta(Ref<Window> window);
	};
}

#endif //HINPUT_H
