#define private public
#include "hwindow.h"
#include "hkeyboard.h"
#include "hmouse.h"
#undef private

#include "hlnx_eventhandling.h"
#include "hyperflow.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "hlnx_window.h"
#include "hplatform.h"

#ifdef None
#undef None;
#endif

namespace hf
{
	std::unordered_map<::Window, Window*> LNX_WIN_REGISTRY;
	Window* Platform_GetWinPtr(::Window window)
	{
		auto it = LNX_WIN_REGISTRY.find(window);
		if (it != LNX_WIN_REGISTRY.end()) return it->second;
		return nullptr;
	}

	void Platform_PushWindowToRegistry(::Window window, Window* ptr) { LNX_WIN_REGISTRY[window] = ptr; }
	void Platform_PopWindowFromRegistry(::Window window) { LNX_WIN_REGISTRY.erase(window); }

	static Key Platform_TransformToKey(KeySym key);
	static void Platform_HandleDestroyNotify	(XEvent& event);
	static void Platform_HandleClientMessage	(XEvent& event);

	static void Platform_HandleConfigureNotify	(XEvent& event);
	static void Platform_HandleFocusIn	        (XEvent& event);
	static void Platform_HandleFocusOut	        (XEvent& event);
	static void Platform_HandleExpose	        (XEvent& event);

	static void Platform_HandleKeyPress			(XEvent& event);
	static void Platform_HandleButtonPress		(XEvent& event);
	static void Platform_HandleKeyRelease		(XEvent& event);
	static void Platform_HandleButtonRelease	(XEvent& event);
	static void Platform_HandleMotionNotify		(XEvent& event);

    void Platform_HandleNextEvent()
    {
        XEvent event;
        auto display = (Display*)Hyperflow::GetPlatformHandle();
        XNextEvent(display, &event);

        switch (event.type)
        {
            case DestroyNotify:   	Platform_HandleDestroyNotify(event);		break;
            case ClientMessage:   	Platform_HandleClientMessage(event);		break;

            case ConfigureNotify:   Platform_HandleConfigureNotify(event);	break;
            case FocusIn:			Platform_HandleFocusIn(event);			break;
            case FocusOut:			Platform_HandleFocusOut(event);			break;
            case Expose:			Platform_HandleExpose(event);			break;

        	case KeyPress:   		Platform_HandleKeyPress(event);			break;
        	case ButtonPress:   	Platform_HandleKeyPress(event);			break;
            case KeyRelease: 		Platform_HandleKeyRelease(event);		break;
            case ButtonRelease: 	Platform_HandleKeyRelease(event);		break;
            case MotionNotify:	 	Platform_HandleMotionNotify(event);		break;
        }
    }

	static void Platform_HandleClientMessage(XEvent& event)
    {
    	auto message = (Atom)event.xclient.data.l[0];

    	auto it = std::ranges::find_if(LNX_WIN_REGISTRY,
	    [&](const std::pair<const::Window, Window*>& entry)
	    {
		    auto lnxData = (LnxWindowData*)entry.second->m_Handle;
		    return lnxData->closeMessage == message;
	    });

    	if (it != LNX_WIN_REGISTRY.end()) it->second->Close();
    }

	static void Platform_HandleDestroyNotify(XEvent& event)
    {
    	auto window = Platform_GetWinPtr(event.xclient.window);
    	if (window) window->Close();
    }

	static void Platform_HandleConfigureNotify(XEvent& event)
	{

	}
	static void Platform_HandleFocusIn(XEvent& event)
	{

	}
	static void Platform_HandleFocusOut(XEvent& event)
	{

	}
	static void Platform_HandleExpose(XEvent& event)
	{

	}

	//region Input
	static void Platform_HandleKeyPress(XEvent& event)
    {
    	auto win = Platform_GetWinPtr(event.xkey.window);
    	auto key = Platform_TransformToKey(XLookupKeysym(&event.xkey, 0));
    	KeyboardEvent_Key(win->m_Keyboard, key, Keyboard::Event::Type::Press);
    }

	static void Platform_HandleButtonPress(XEvent& event)
    {

    }

	static void Platform_HandleKeyRelease(XEvent& event)
    {
    	auto win = Platform_GetWinPtr(event.xkey.window);
    	auto key = Platform_TransformToKey(XLookupKeysym(&event.xkey, 0));
    	KeyboardEvent_Key(win->m_Keyboard, key, Keyboard::Event::Type::Release);
    }

	static void Platform_HandleButtonRelease(XEvent& event)
	{

	}

	static void Platform_HandleMotionNotify(XEvent& event)
	{

	}
	//endregion

	static Key Platform_TransformToKey(KeySym key)
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