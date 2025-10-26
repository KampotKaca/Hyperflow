#ifndef HGENERICEXCEPTION_H
#define HGENERICEXCEPTION_H

#include "hexception.h"

namespace hf
{
    class GenericException : public HyperException
    {
    public:
        GenericException(int32_t lineNum, const char* file, const char* type, const char *err) noexcept;
        const char* what() const noexcept override;
        const char* GetError() const noexcept;
        const char* GetType() const noexcept override;

        static GenericException Create(int32_t lineNum, const char* file, const char* type, const char* error, ...);

    private:
        mutable std::string m_Type;
        mutable std::string m_Error;
    };

#define GENERIC_EXCEPT(type, error, ...) GenericException::Create(__LINE__, __FILE__, type, error, ##__VA_ARGS__)
}

#endif //HGENERICEXCEPTION_H
