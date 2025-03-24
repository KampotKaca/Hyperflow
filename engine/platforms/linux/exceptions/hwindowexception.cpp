#include "hwindowexception.h"
#include <sstream>
#include "hyperflow.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace hf
{
    WindowException::WindowException(int32_t lineNum, const char* file, int32_t errorCode)
    : HyperException(lineNum, file), m_ErrorCode(errorCode) { }

    const char* WindowException::what() const noexcept
    {
        std::ostringstream oss;
        oss << GetType() << std::endl
        << "[Error Code] " << std::hex << GetErrorCode() << std::endl
        << "[Description] " << GetErrorString();

        m_WhatBuffer = oss.str();
        return m_WhatBuffer.c_str();
    }

    std::string WindowException::TranslateErrorCode(int32_t result)
    {
        char error_text[256];
        XGetErrorText((Display*)Hyperflow::GetPlatformHandle(), result, error_text, sizeof(error_text));
        return error_text;
    }

    int32_t WindowException::GetErrorCode() const noexcept { return m_ErrorCode; }
    const char* WindowException::GetType() const noexcept { return "[Window Exception]"; }
    std::string WindowException::GetErrorString() const noexcept { return TranslateErrorCode(m_ErrorCode); }
}