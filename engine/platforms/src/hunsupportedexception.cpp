#include "hunsupportedexception.h"

#include <sstream>

namespace hf
{
    UnsupportedException::UnsupportedException(int32_t lineNum, const char* file, const char* error)
        : HyperException(lineNum, file), m_Error(error) { }

    const char* UnsupportedException::what() const noexcept
    {
        std::ostringstream oss;
        oss << GetType() << std::endl
            << "[Description] " << GetErrorString();

        m_WhatBuffer = oss.str();
        return m_WhatBuffer.c_str();
    }

    const char* UnsupportedException::GetType() const noexcept { return "[Unsupported]"; }
    const std::string& UnsupportedException::GetErrorString() const noexcept { return m_Error; }

    UnsupportedException UnsupportedException::Create(int32_t lineNum, const char* file, const char* error, ...)
    {
        char* res = nullptr;
        va_list args;
        va_start(args, error);
        log_fmt(error, &res, args);
        va_end(args);
        return UnsupportedException(lineNum, file, res);
    }
}