#include "hwin_shared.h"
#include "hplatform.h"
#include "hwinproc.h"
#include "exceptions/hwindowexception.h"

namespace hf
{
	void Platform_SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value)
	{
		*flags = (WindowFlags)(((int32_t)*flags & ~(int32_t)target) | (-value & (int32_t)target));
	}

	void Platform_HandleEvents(EngineUpdateType updateType)
	{
		MSG msg;
		switch (updateType)
		{
			case EngineUpdateType::Continues:

				if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				break;
			case EngineUpdateType::EventRaised:
			default:

				if(GetMessage(&msg, nullptr, 0, 0) > 0)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				break;
		}
	}

	LRESULT CALLBACK Platform_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		auto* window = (Window*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		switch (msg)
		{
			//Window
			case WM_SETTEXT:              Platform_HandleEvents_WindowTitle      (window, wparam, lparam); break;
			case WM_CLOSE:         return Platform_HandleEvents_WindowClose      (window, wparam, lparam);
			case WM_SHOWWINDOW:    return Platform_HandleEvents_WindowShow       (window, wparam, lparam);
			case WM_MOVE:          return Platform_HandleEvents_WindowMove       (window, wparam, lparam);
			case WM_SIZE:          return Platform_HandleEvents_WindowResize     (window, wparam, lparam);
			case WM_SETFOCUS:      return Platform_HandleEvents_WindowFocus      (window, wparam, lparam, true);
			case WM_KILLFOCUS:     return Platform_HandleEvents_WindowFocus      (window, wparam, lparam, false);

			//KeyBoard
			case WM_CHAR:          return Platform_HandleEvents_KeyboardChar(window, wparam, lparam);
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:    return Platform_HandleEvents_KeyboardKey(window, wparam, lparam, Keyboard::Event::Type::Press);
			case WM_KEYUP:         return Platform_HandleEvents_KeyboardKey(window, wparam, lparam, Keyboard::Event::Type::Release);

			//Mouse
			case WM_LBUTTONDOWN:   return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Left, Mouse::Event::Type::Press);
			case WM_RBUTTONDOWN:   return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Right, Mouse::Event::Type::Press);
			case WM_MBUTTONDOWN:   return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Wheel, Mouse::Event::Type::Press);
			case WM_XBUTTONDOWN:   return Platform_HandleEvents_MouseButtonExtra(window, wparam, lparam, Mouse::Event::Type::Press);

			case WM_LBUTTONUP:     return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Left, Mouse::Event::Type::Release);
			case WM_RBUTTONUP:     return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Right, Mouse::Event::Type::Release);
			case WM_MBUTTONUP:     return Platform_HandleEvents_MouseButton(window, wparam, lparam, Button::Wheel, Mouse::Event::Type::Release);
			case WM_XBUTTONUP:     return Platform_HandleEvents_MouseButtonExtra(window, wparam, lparam, Mouse::Event::Type::Release);

			case WM_MOUSEMOVE:     return Platform_HandleEvents_MouseMove(window, wparam, lparam);
			case WM_MOUSEWHEEL:    return Platform_HandleEvents_MouseScroll(window, wparam, lparam, ivec2{ 0, 1 });
			case WM_MOUSEHWHEEL:   return Platform_HandleEvents_MouseScroll(window, wparam, lparam, ivec2{ 1, 0 });

			default: break;
		}

		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}

	LRESULT CALLBACK Platform_WindowProc_Create(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if(msg == WM_NCCREATE)
		{
			auto* pCreate = (CREATESTRUCT*)lparam;
			auto* window = (Window*)pCreate->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)Platform_WindowProc);
		}
		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}

	void Platform_Initialize()
	{
		HINSTANCE hinstance = GetModuleHandle(nullptr);

		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(wndClass);
		wndClass.lpszClassName = WINDOWS_CLASS_NAME;
		wndClass.style = CS_OWNDC;
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hbrBackground = nullptr;
		wndClass.lpszMenuName = nullptr;
		wndClass.hInstance = hinstance;
		wndClass.lpfnWndProc = Platform_WindowProc_Create;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hIconSm = nullptr;
		wndClass.hInstance = hinstance;

		RegisterClassEx(&wndClass);
		PLATFORM_DATA.instance = hinstance;
	}

	void Platform_Dispose()
    {

    }

	void Platform_BeginTemporarySystemTimer(uint16_t millisecondPrecision)
	{
		timeBeginPeriod(millisecondPrecision);
	}
	void Platform_EndTemporarySystemTimer(uint16_t millisecondPrecision)
	{
		timeEndPeriod(millisecondPrecision);
	}

	void Platform_Sleep(double seconds)
	{
		// A waitable timer seems to be better than the Windows Sleep().
		HANDLE WaitTimer;
		LARGE_INTEGER dueTime;
		seconds *= -10.0 * 1000.0 * 1000.0;
		dueTime.QuadPart = static_cast<LONGLONG>(seconds); //dueTime is in 100ns
		// We don't name the timer (third parameter) because CreateWaitableTimer will fail if the name
		// matches an existing name (e.g., if two threads call osaSleep).
		WaitTimer = CreateWaitableTimer(nullptr, true, nullptr);
		SetWaitableTimer(WaitTimer, &dueTime, 0, nullptr, nullptr, 0);
		WaitForSingleObject(WaitTimer, INFINITE);
		CloseHandle(WaitTimer);
	}

	ivec2 Platform_GetPointerPosition(Window* window)
	{
		POINT point;
		if(!GetCursorPos(&point)) throw WND_LAST_EXCEPT();
		if(!ScreenToClient((HWND)window->GetHandle(), &point)) throw WND_LAST_EXCEPT();
		return { (int32_t)point.x, (int32_t)point.y };
	}
}