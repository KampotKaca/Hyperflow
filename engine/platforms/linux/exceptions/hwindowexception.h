#ifndef HWINDOWEXCEPTION_H
#define HWINDOWEXCEPTION_H

#include "hshared.h"
#include "hexception.h"
#include <inttypes.h>

namespace hf
{
    class WindowException : public HyperException
    {
    public:
        WindowException(int32_t lineNum, const char* file, int32_t errorCode);

        const char* what() const noexcept override;

        static std::string TranslateErrorCode(int32_t result);

        int32_t GetErrorCode() const noexcept;
        const char* GetType() const noexcept override;
        std::string GetErrorString() const noexcept;

    private:
        int32_t m_ErrorCode;
    };

#define WND_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)
}

#endif //HWINDOWEXCEPTION_H
