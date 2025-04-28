#ifndef HWIN_SHARED_H
#define HWIN_SHARED_H

//Windows 7 or later
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <sdkddkver.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef STRICT
#define STRICT
#endif

#include <dde.h>
#include <ddeml.h>
#include <mmsystem.h>
#include <rpc.h>

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define _UNICODE
#include <windowsx.h>

#include "hshared.h"
#include "hwindow.h"
#include <dbghelp.h>
#include "hinternal.h"

namespace hf
{
	constexpr char WINDOWS_CLASS_NAME[] = "HyperflowClass";

	void Windows_ConvertSize(const Window* window, ivec2& size);
	uint64_t Windows_GetTicks();
	uint32_t Windows_GetStyleID(WindowStyle style);

	struct Win_Platform_Data
	{
		HINSTANCE instance;
	};

	extern Win_Platform_Data PLATFORM_DATA;
	void InitThreadHook();
}

#endif //HWIN_SHARED_H
