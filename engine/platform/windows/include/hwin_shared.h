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

inline const char* win_translate_error_code(HRESULT result)
{
    static char buffer[512];
    DWORD nMsgLen = FormatMessage
    (
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buffer, 0, nullptr
    );
    if (nMsgLen == 0) return "Unidentified error code";
    return buffer;
}

#define hwin_assert(x, ...)\
        if (!(x))\
        {\
            log_fatal(__VA_ARGS__);\
            auto error = GetLastError();\
            std::ostringstream oss;\
            oss << "[Error Code] " << std::hex << error << std::endl\
            << "[Description] " << win_translate_error_code(error);\
            abort();\
        }

#define hwin_assert_e(err, ...)\
        if (err < 0)\
        {\
            log_fatal(__VA_ARGS__);\
            std::ostringstream oss;\
            oss << "[Error Code] " << std::hex << err << std::endl\
            << "[Description] " << win_translate_error_code(err);\
            abort();\
        }

#endif //HWIN_SHARED_H
