#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "hshared.h"
#include "hwindow.h"
#include "hrenderer.h"
#include "hexception.h"

namespace hf
{
	void Run(const EngineData& engineData);
	bool IsRunning();
	Ref<Window> GetMainWindow();

	Ref<Window> OpenWindow(const WindowData& data, const Ref<Window>& parent);
	void Terminate();

	namespace input
	{
		bool IsDown(const Ref<Window> &window, Key key);
		bool IsDownContinues(const Ref<Window> &window, Key key);
		bool IsUp(const Ref<Window> &window, Key key);

		bool IsDown(const Ref<Window> &window, Button button);
		bool IsDownContinues(const Ref<Window> &window, Button button);
		bool IsUp(const Ref<Window> &window, Button button);

		bool IsDown(Key key);
		bool IsDownContinues(Key key);
		bool IsUp(Key key);

		bool IsDown(Button button);
		bool IsDownContinues(Button button);
		bool IsUp(Button button);

		KeyState GetState(Key key);
		ButtonState GetState(Button button);
		const std::string& GetWrite();

		ivec2 GetPointerPosition();
		ivec2 GetPointerDelta();
		vec2 GetScrollDelta();

		KeyState GetState(const Ref<Window> &window, Key key);
		ButtonState GetState(const Ref<Window> &window, Button button);
		const std::string& GetWrite(const Ref<Window> &window);

		ivec2 GetPointerPosition(const Ref<Window> &window);
		ivec2 GetPointerDelta(const Ref<Window> &window);
		vec2 GetScrollDelta(const Ref<Window> &window);
	}

	namespace time
	{
		uint64_t GetFrameCount();
		double GetDeltaTime();
		double GetTimePassed();
		double GetAbsoluteTimePassed();
		int16_t GetTargetFrameRate();
		double GetSystemTime();
		int32_t GetFrameRate();
		void SetTargetFrameRate(int16_t targetFrameRate);
	}
}

#endif //HYPERFLOW_H
