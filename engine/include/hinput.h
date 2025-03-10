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

		//endregion

		//region Event Handled Input

		static void Subscribe(Ref<Window> window, const InputCallback* callback);
		static void Subscribe(Ref<Window> window, const InputShortcut* shortcut);
		static void SubscribeChar(Ref<Window> window, InputCharCallback callback);
		static void SubscribePointerMove(Ref<Window> window, InputPointerMoveCallback callback);
		static void SubscribeScroll(Ref<Window> window, InputScrollCallback callback);

		static void Unsubscribe(Ref<Window> window, const InputCallback* callback);
		static void Unsubscribe(Ref<Window> window, const InputShortcut* shortcut);
		static void UnsubscribeChar(Ref<Window> window, InputCharCallback callback);
		static void UnsubscribePointerMove(Ref<Window> window, InputPointerMoveCallback callback);
		static void UnsubscribeScroll(Ref<Window> window, InputScrollCallback callback);

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
