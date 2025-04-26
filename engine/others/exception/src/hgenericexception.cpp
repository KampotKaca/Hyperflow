#include "hgenericexception.h"
#include <cstdarg>
#include <sstream>

namespace hf
{
    GenericException::GenericException(int32_t lineNum, const char *file, const char *type, const char *err) noexcept
    : HyperException(lineNum, file), m_Type(type), m_Error(err) { }

    const char *GenericException::what() const noexcept
    {
        std::ostringstream oss;
        oss << GetType() << std::endl
            << "[Description] " << GetError() << std::endl;

        m_WhatBuffer = oss.str();
        return m_WhatBuffer.c_str();
    }

    const char *GenericException::GetError() const noexcept { return m_Error.c_str(); }
    const char *GenericException::GetType() const noexcept { return m_Type.c_str(); }

    GenericException GenericException::Create(int32_t lineNum, const char* file, const char* type, const char* error, ...)
    {
        char res[256]{};
        va_list args;
        va_start(args, error);
        va_end(args);

        vsprintf(res, error, args);
        return {lineNum, file, type, res};
    }
}