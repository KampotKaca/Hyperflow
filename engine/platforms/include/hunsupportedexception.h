#ifndef HUNSUPPORTEDEXCEPTION_H
#define HUNSUPPORTEDEXCEPTION_H

#include "hexception.h"

namespace hf
{
    class UnsupportedException : public HyperException
    {
    public:
        UnsupportedException(int32_t lineNum, const char* file, const char* error);

        const char* what() const noexcept override;

        const char* GetType() const noexcept override;
        const std::string& GetErrorString() const noexcept;

    private:
        std::string m_Error;
    };
#define UNSUPPORTED_EXCEPT(err) UnsupportedException(__LINE__, __FILE__, err)
}

#endif //HUNSUPPORTEDEXCEPTION_H
