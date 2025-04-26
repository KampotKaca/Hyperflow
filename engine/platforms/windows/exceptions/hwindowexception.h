#ifndef HWINDOWEXCEPTION_H
#define HWINDOWEXCEPTION_H

#include "../hwindows.h"
#include "hshared.h"
#include "../../../others/exception/include/hexception.h"

namespace hf
{
	class WindowException : public HyperException
	{
	public:
		WindowException(int32_t lineNum, const char* file, HRESULT errorCode);

		const char* what() const noexcept override;

		static std::string TranslateErrorCode(HRESULT result);

		HRESULT GetErrorCode() const noexcept;
		const char* GetType() const noexcept override;
		std::string GetErrorString() const noexcept;

	private:
		HRESULT m_ErrorCode;
	};

#define WND_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)
#define WND_LAST_EXCEPT() WindowException(__LINE__, __FILE__, GetLastError())
}

#endif //HWINDOWEXCEPTION_H
