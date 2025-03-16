#ifndef HINPUT_H
#define HINPUT_H

#include "hshared.h"

namespace hf
{
	class Window;

	class Input
	{
	public:
		//region Update handled Input

		static bool IsDown(const Ref<Window> &window, Key key);
		static bool IsDownContinues(const Ref<Window> &window, Key key);
		static bool IsUp(const Ref<Window> &window, Key key);

		static bool IsDown(const Ref<Window> &window, Button button);
		static bool IsDownContinues(const Ref<Window> &window, Button button);
		static bool IsUp(const Ref<Window> &window, Button button);

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

		static KeyState GetState(const Ref<Window> &window, Key key);
		static ButtonState GetState(const Ref<Window> &window, Button button);
		static const std::string& GetWrite(const Ref<Window> &window);

		static glm::ivec2 GetPointerPosition(const Ref<Window> &window);
		static glm::ivec2 GetPointerDelta(const Ref<Window> &window);
		static glm::vec2 GetScrollDelta(const Ref<Window> &window);

		//endregion

		//region Event Handled Input

		static void Subscribe(const Ref<Window> &window, const InputCallback* callback);
		static void Subscribe(const Ref<Window> &window, const InputShortcut* shortcut);
		static void SubscribeChar(const Ref<Window> &window, InputCharCallback callback);
		static void SubscribePointerMove(const Ref<Window> &window, InputPointerMoveCallback callback);
		static void SubscribeScroll(const Ref<Window> &window, InputScrollCallback callback);

		static void Unsubscribe(const Ref<Window> &window, const InputCallback* callback);
		static void Unsubscribe(const Ref<Window> &window, const InputShortcut* shortcut);
		static void UnsubscribeChar(const Ref<Window> &window, InputCharCallback callback);
		static void UnsubscribePointerMove(const Ref<Window> &window, InputPointerMoveCallback callback);
		static void UnsubscribeScroll(const Ref<Window> &window, InputScrollCallback callback);

		static void Subscribe(const InputCallback* callback);
		static void Subscribe(const InputShortcut* shortcut);
		static void SubscribeChar(InputCharCallback callback);
		static void SubscribePointerMove(InputPointerMoveCallback callback);
		static void SubscribeScroll(InputScrollCallback callback);

		static void Unsubscribe(const InputCallback* callback);
		static void Unsubscribe(const InputShortcut* shortcut);
		static void UnsubscribeChar(InputCharCallback callback);
		static void UnsubscribePointerMove(InputPointerMoveCallback callback);
		static void UnsubscribeScroll(InputScrollCallback callback);

		//endregion
	};
}

#endif //HINPUT_H
