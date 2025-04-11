#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "hshared.h"
#include "hexception.h"
#include "hgenericexception.h"

namespace hf
{
	void Run(const EngineData& engineData);
	bool IsRunning();
	Ref<Window> GetMainWindow();
	const std::string& GetApplicationTitle();

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

	namespace window
	{
		const std::string& GetTitle(const Ref<Window> &window);
		ivec2 GetSize(const Ref<Window> &window);
		ivec2 GetPosition(const Ref<Window> &window);
		IRect GetRect(const Ref<Window> &window);
		WindowFlags GetFlags(const Ref<Window> &window);
		WindowStyle GetStyle(const Ref<Window> &window);
		void* GetHandle(const Ref<Window> &window);

		void SetTitle(const Ref<Window> &window, const char* title);
		void SetSize(const Ref<Window> &window, ivec2 size);
		void SetPosition(const Ref<Window> &window, ivec2 position);
		void SetRect(const Ref<Window> &window, IRect rect);

		bool IsClosing(const Ref<Window> &window);
		void SetFlags(const Ref<Window> &window, WindowFlags flags);
		void Focus(const Ref<Window> &window);
		bool Close(const Ref<Window> &window);
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

	namespace rendering
	{

	}

	namespace shader
	{
		bool IsAlive(const Ref<Shader>& shader);
	}

	namespace utils
	{
		ivec3 ConvertVersion(const char* version);
		bool ReadFile(const std::string& filename, std::vector<char>& result);
	}
}

#endif //HYPERFLOW_H
