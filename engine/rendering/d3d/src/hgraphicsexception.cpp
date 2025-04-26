#include <sstream>
#include "hgraphicsexception.h"

namespace hf
{
	GraphicsException::GraphicsException(int32_t lineNum, const char* file, HRESULT errorCode)
		: HyperException(lineNum, file), m_ErrorCode(errorCode) { }

	const char* GraphicsException::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl
		    << "[Error Code] " << std::hex << GetErrorCode() << std::endl
		    << "[Description] " << GetErrorString();

		m_WhatBuffer = oss.str();
		return m_WhatBuffer.c_str();
	}

	std::string GraphicsException::TranslateErrorCode(HRESULT result)
	{
		char* pMsgBuf = nullptr;
		DWORD nMsgLen = FormatMessage
		(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
		);
		if(nMsgLen == 9) return "Unidentified error code";
		std::string errorString = pMsgBuf;
		LocalFree(pMsgBuf);
		return errorString;
	}

	HRESULT GraphicsException::GetErrorCode() const noexcept { return m_ErrorCode; }
	const char* GraphicsException::GetType() const noexcept { return "[D3D11 Exception]"; }
	std::string GraphicsException::GetErrorString() const noexcept { return TranslateErrorCode(m_ErrorCode); }
}