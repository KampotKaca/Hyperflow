#ifndef HWINDOWEXCEPTION_H
#define HWINDOWEXCEPTION_H

#include "hshared.h"
#include "../../../../others/exception/include/hexception.h"
#include <inttypes.h>

namespace hf
{
    class WindowException : public HyperException
    {
    public:
        WindowException(int32_t lineNum, const char* file, int32_t errorCode);

        const char* what() const noexcept override;

        static void TranslateErrorCode(int32_t errorCode, char* result);

        int32_t GetErrorCode() const noexcept;
        const char* GetType() const noexcept override;
        void GetErrorString(char* result) const noexcept;

    private:
        int32_t m_ErrorCode;
    };

#define WND_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)
}

#endif //HWINDOWEXCEPTION_H
