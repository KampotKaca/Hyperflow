#define private public
#include "hwindow.h"
#include "hkeyboard.h"
#include "hmouse.h"
#undef private

#include "hlnx_eventhandling.h"
#include "hyperflow.h"
#include "hinternal.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput2.h>

#include "hlnx_window.h"
#include "hplatform.h"

#ifdef None
#undef None;
#endif

namespace hf
{
	static Key Platform_TransformToKey(KeySym key);
	static void Platform_HandleGenericEvent		(XEvent& event);
	static void Platform_HandleDestroyNotify	(XEvent& event, Window* window);
	static void Platform_HandleClientMessage	(const XEvent& event, Window* window);

	static void Platform_HandleConfigureNotify	(const XEvent& event, Window* window);
	static void Platform_HandleFocusIn	        (XEvent& event, Window* window);
	static void Platform_HandleFocusOut	        (XEvent& event, Window* window);
	static void Platform_HandleExpose	        (XEvent& event, Window* window);

	static void Platform_HandleKeyPress			(XEvent& event, const Window* window);
	static void Platform_HandleButtonPress		(const XEvent& event, const Window* window);
	static void Platform_HandleKeyRelease		(XEvent& event, const Window* window);
	static void Platform_HandleButtonRelease	(const XEvent& event, const Window* window);
	static void Platform_HandleMotionNotify		(XEvent& event, const Window* window);

    void Platform_HandleNextEvent()
    {
        XEvent event;
        XNextEvent(PLATFORM_DATA.display, &event);

    	auto it = WIN_REGISTRY.find(event.xclient.window);
		Window* window = nullptr;
    	if (it != WIN_REGISTRY.end()) window = it->second;
    	switch (event.type)
        {
            case GenericEvent:   	Platform_HandleGenericEvent(event);		        return;
            case DestroyNotify:   	Platform_HandleDestroyNotify(event, window);		return;
            case ClientMessage:   	Platform_HandleClientMessage(event, window);		return;

            case ConfigureNotify:   Platform_HandleConfigureNotify(event, window);	return;
            case FocusIn:			Platform_HandleFocusIn(event, window);			return;
            case FocusOut:			Platform_HandleFocusOut(event, window);			return;
            case Expose:			Platform_HandleExpose(event, window);			return;

        	case KeyPress:   		Platform_HandleKeyPress(event, window);			return;
        	case ButtonPress:   	Platform_HandleButtonPress(event, window);		return;
            case KeyRelease: 		Platform_HandleKeyRelease(event, window);		return;
            case ButtonRelease: 	Platform_HandleButtonRelease(event, window);		return;
            case MotionNotify:	 	Platform_HandleMotionNotify(event, window);		return;
        }
    }

	static void Platform_HandleGenericEvent(XEvent& event)
    {
    	auto display = PLATFORM_DATA.display;
    	if (event.xcookie.extension == PLATFORM_DATA.xiOpcode)
    	{
    		auto rootWindow = XRootWindow(display, 0);
    		XGetEventData(display, &event.xcookie);
    		if (event.xcookie.evtype != XI_RawMotion)
    		{
    			XFreeEventData(display, &event.xcookie);
    			return;
    		}
    		XFreeEventData(display, &event.xcookie);

    		::Window root, child;
    		ivec2 rootPos{}, winPos{};
    		unsigned int mask_return;
    		int retval = XQueryPointer(display, rootWindow, &root, &child,
									   &rootPos.x, &rootPos.y,
									   &winPos.x, &winPos.y,
									   &mask_return);
    		if (!retval || !child) return;

    		// auto window = Platform_GetWinPtr(root);
    		// ivec2 localPos = window->GetPosition() - rootPos;
    		// LOG_INFO("X: %i, Y: %i", localPos.x, localPos.y);
    	}
    }

	static void Platform_HandleClientMessage(const XEvent& event, Window* window)
    {
    	if (!window) return;
    	auto message = (Atom)event.xclient.data.l[0];
    	if (message == PLATFORM_DATA.closeMessage) inter::window::Close(window);
    }

	static void Platform_HandleDestroyNotify(XEvent& event, Window* window)
    {
    	if (window) inter::window::Close(window);
    }

	static void Platform_HandleConfigureNotify(const XEvent& event, Window* window)
	{
    	window->rect = (IRect)
		{
			.position = ivec2(event.xconfigure.x, event.xconfigure.y),
			.size = ivec2(event.xconfigure.width, event.xconfigure.height),
		};
	}
	static void Platform_HandleFocusIn(XEvent& event, Window* window)
	{
    	if (!window) return;
		WindowEvent_Focus(window, true);
	}
	static void Platform_HandleFocusOut(XEvent& event, Window* window)
	{
    	if (!window) return;
    	WindowEvent_Focus(window, false);
	}
	static void Platform_HandleExpose(XEvent& event, Window* window)
	{
    	if (!window) return;
    	WindowEvent_Show(window, true);
	}

	//region Input
	static void Platform_HandleKeyPress(XEvent& event, const Window* window)
    {
    	auto key = Platform_TransformToKey(XLookupKeysym(&event.xkey, 0));
    	KeyboardEvent_Key(window->keyboard, key, Keyboard::Event::Type::Press);
    }

	static void Platform_HandleButtonPress(const XEvent& event, const Window* window)
    {
    	switch (event.xbutton.button)
    	{
    	    case Button1: MouseEvent_Button(window->mouse, Button::Left, Mouse::Event::Type::Press);   break;
    	    case Button2: MouseEvent_Button(window->mouse, Button::Wheel, Mouse::Event::Type::Press);  break;
    	    case Button3: MouseEvent_Button(window->mouse, Button::Right, Mouse::Event::Type::Press);  break;

    	    case 8:       MouseEvent_Button(window->mouse, Button::Extra1, Mouse::Event::Type::Press); break;
    	    case 9:       MouseEvent_Button(window->mouse, Button::Extra2, Mouse::Event::Type::Press); break;

    		case Button4: MouseEvent_Scroll(window->mouse, vec2(0, 1));  break;
    		case Button5: MouseEvent_Scroll(window->mouse, vec2(0, -1)); break;

    	    case 6: MouseEvent_Scroll(window->mouse, vec2(1, 0));  break;
    	    case 7: MouseEvent_Scroll(window->mouse, vec2(-1, 0)); break;
			default: break;
	    }
    }

	static void Platform_HandleKeyRelease(XEvent& event, const Window* window)
    {
    	auto key = Platform_TransformToKey(XLookupKeysym(&event.xkey, 0));
    	KeyboardEvent_Key(window->keyboard, key, Keyboard::Event::Type::Release);
    }

	static void Platform_HandleButtonRelease(const XEvent& event, const Window* window)
	{
    	switch (event.xbutton.button)
    	{
    	    case Button1: MouseEvent_Button(window->mouse, Button::Left, Mouse::Event::Type::Release);   break;
    	    case Button2: MouseEvent_Button(window->mouse, Button::Wheel, Mouse::Event::Type::Release);  break;
    	    case Button3: MouseEvent_Button(window->mouse, Button::Right, Mouse::Event::Type::Release);  break;
    	    case 8:       MouseEvent_Button(window->mouse, Button::Extra1, Mouse::Event::Type::Release); break;
    	    case 9:       MouseEvent_Button(window->mouse, Button::Extra2, Mouse::Event::Type::Release); break;
    	    default: break;
    	}
	}

	static void Platform_HandleMotionNotify(XEvent& event, const Window* window)
	{
    	MouseEvent_Moved(window->mouse, { event.xmotion.x, event.xmotion.y });
	}

	//endregion

	static Key Platform_TransformToKey(const KeySym key)
    {
	    switch (key)
    	{
			default:			 return Key::None;
        	case XK_BackSpace:   return Key::Backspace;
        	case XK_Tab:		 return Key::Tab;
        	case XK_Return:
        	case XK_ISO_Enter:
        	case XK_KP_Enter:	 return Key::Enter;
        	case XK_Shift_L: 	 return Key::LeftShift;
        	case XK_Shift_R: 	 return Key::RightShift;
        	case XK_Control_L:   return Key::LeftControl;
        	case XK_Control_R:   return Key::RightControl;
        	case XK_Alt_L: 		 return Key::LeftAlt;
        	case XK_Alt_R: 		 return Key::RightAlt;
        	case XK_Pause: 		 return Key::Pause;
        	case XK_Caps_Lock:   return Key::CapsLock;
        	case XK_Escape:		 return Key::Escape;
        	case XK_space:		 return Key::Space;
        	case XK_Page_Up:	 return Key::PageUp;
        	case XK_Page_Down:   return Key::PageDown;
        	case XK_End:		 return Key::End;
        	case XK_Home:		 return Key::Home;
        	case XK_Left:		 return Key::Left;
        	case XK_Up:			 return Key::Up;
        	case XK_Right:		 return Key::Right;
        	case XK_Down:		 return Key::Down;
        	case XK_Print:		 return Key::PrintScreen;
        	case XK_Insert:		 return Key::Insert;
        	case XK_Delete:		 return Key::Delete;
        	case XK_Super_L:	 return Key::LeftSuper;
        	case XK_Super_R:	 return Key::RightSuper;
        	case XK_Num_Lock:	 return Key::NumLock;
        	case XK_Scroll_Lock: return Key::ScrollLock;

        	case XK_0: return Key::Zero;
        	case XK_1: return Key::One;
        	case XK_2: return Key::Two;
        	case XK_3: return Key::Three;
        	case XK_4: return Key::Four;
        	case XK_5: return Key::Five;
        	case XK_6: return Key::Six;
        	case XK_7: return Key::Seven;
        	case XK_8: return Key::Eight;
        	case XK_9: return Key::Nine;

        	case XK_KP_0: 		 return Key::Pad0;
        	case XK_KP_1: 		 return Key::Pad1;
        	case XK_KP_2: 		 return Key::Pad2;
        	case XK_KP_3: 		 return Key::Pad3;
        	case XK_KP_4: 		 return Key::Pad4;
        	case XK_KP_5: 		 return Key::Pad5;
        	case XK_KP_6: 		 return Key::Pad6;
        	case XK_KP_7: 		 return Key::Pad7;
        	case XK_KP_8: 		 return Key::Pad8;
        	case XK_KP_9: 		 return Key::Pad9;
        	case XK_KP_Multiply: return Key::PadMultiply;
        	case XK_KP_Add:		 return Key::PadAdd;
        	case XK_KP_Equal:	 return Key::PadEqual;
        	case XK_KP_Subtract: return Key::PadSubtract;
        	case XK_KP_Decimal:  return Key::PadDecimal;
        	case XK_KP_Divide:   return Key::PadDivide;

        	case XK_F1:  return Key::F1;
        	case XK_F2:  return Key::F2;
        	case XK_F3:  return Key::F3;
        	case XK_F4:  return Key::F4;
        	case XK_F5:  return Key::F5;
        	case XK_F6:  return Key::F6;
        	case XK_F7:  return Key::F7;
        	case XK_F8:  return Key::F8;
        	case XK_F9:  return Key::F9;
        	case XK_F10: return Key::F10;
        	case XK_F11: return Key::F11;
        	case XK_F12: return Key::F12;

        	case XK_a: return Key::A;
        	case XK_b: return Key::B;
        	case XK_c: return Key::C;
        	case XK_d: return Key::D;
        	case XK_e: return Key::E;
        	case XK_f: return Key::F;
        	case XK_g: return Key::G;
        	case XK_h: return Key::H;
        	case XK_i: return Key::I;
        	case XK_j: return Key::J;
        	case XK_k: return Key::K;
        	case XK_l: return Key::L;
        	case XK_m: return Key::M;
        	case XK_n: return Key::N;
        	case XK_o: return Key::O;
        	case XK_p: return Key::P;
        	case XK_q: return Key::Q;
        	case XK_r: return Key::R;
        	case XK_s: return Key::S;
        	case XK_t: return Key::T;
        	case XK_u: return Key::U;
        	case XK_v: return Key::V;
        	case XK_w: return Key::W;
        	case XK_x: return Key::X;
        	case XK_y: return Key::Y;
        	case XK_z: return Key::Z;

        	case XK_apostrophe:   return Key::Apostrophe;
        	case XK_bracketleft:  return Key::LeftBracket;
        	case XK_backslash:    return Key::Backslash;
        	case XK_bracketright: return Key::RightBracket;
        	case XK_semicolon:	  return Key::Semicolon;
        	case XK_equal: 		  return Key::Equal;
        	case XK_comma: 		  return Key::Comma;
        	case XK_minus: 		  return Key::Minus;
        	case XK_period: 	  return Key::Period;
        	case XK_slash:  	  return Key::Slash;
        	case XK_grave:  	  return Key::Grave;
    	}
    }
}