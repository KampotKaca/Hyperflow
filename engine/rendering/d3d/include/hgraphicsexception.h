#ifndef HGRAPHICSEXCEPTION_H
#define HGRAPHICSEXCEPTION_H

#include "hwin_shared.h"
#include "hexception.h"

namespace hf
{
	class GraphicsException : public HyperException
	{
	public:
		GraphicsException(int32_t lineNum, const char* file, HRESULT errorCode);

		const char* what() const noexcept override;

		static std::string TranslateErrorCode(HRESULT result);

		HRESULT GetErrorCode() const noexcept;
		const char* GetType() const noexcept override;
		std::string GetErrorString() const noexcept;

	private:
		HRESULT m_ErrorCode;
	};

#define D3D11_EXCEPT(hr) GraphicsException(__LINE__, __FILE__, hr)
#define D3D11_LAST_EXCEPT() GraphicsException(__LINE__, __FILE__, GetLastError())
#define D3D11_HANDLE_EXCEPT(x) \
        {\
			HRESULT res = x;\
            if(res < 0) throw GraphicsException(__LINE__, __FILE__, res);\
		}
}


#endif //HGRAPHICSEXCEPTION_H
