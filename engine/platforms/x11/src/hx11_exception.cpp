#include "hx11_shared.h"
#include "hx11_exception.h"
#include "hx11_platform.h"
#include <sstream>
#include <X11/extensions/XInput2.h>

namespace hf
{
    WindowException::WindowException(int32_t lineNum, const char* file, int32_t errorCode)
    : HyperException(lineNum, file), m_ErrorCode(errorCode) { }

    const char* WindowException::what() const noexcept
    {
        char errorString[256];
        GetErrorString(errorString);
        std::ostringstream oss;
        oss << GetType() << std::endl
        << "[Error Code] " << std::hex << GetErrorCode() << std::endl
        << "[Description] " << errorString;

        m_WhatBuffer = oss.str();
        return m_WhatBuffer.c_str();
    }

    void WindowException::TranslateErrorCode(int32_t errorCode, char* result)
    {
        XGetErrorText(PLATFORM_DATA.display, errorCode, result, 256);
    }

    int32_t WindowException::GetErrorCode() const noexcept { return m_ErrorCode; }
    const char* WindowException::GetType() const noexcept { return "[Window Exception]"; }
    void WindowException::GetErrorString(char* result) const noexcept { return TranslateErrorCode(m_ErrorCode, result); }
}
