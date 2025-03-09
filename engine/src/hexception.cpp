#include "hexception.h"
#include <sstream>

namespace hf
{
	HyperException::HyperException(int32_t lineNum, const char* file) noexcept
	: m_Line(lineNum), m_File(file)
	{

	}

	const char* HyperException::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType();
		m_WhatBuffer = oss.str();
		return m_WhatBuffer.c_str();
	}

	int32_t HyperException::GetLine() const noexcept { return m_Line; }
	const std::string& HyperException::GetFile() const noexcept { return m_File; }

	const char* HyperException::GetType() const noexcept { return "[Hyper Exception] "; }

	NullReferenceException::NullReferenceException(int32_t lineNum, const char *file, const char *type, const char *variable) noexcept
	: HyperException(lineNum, file), m_ReferenceType(type), m_VariableName(variable) { }

	const char *NullReferenceException::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl
		    << "[Type] " << GetReferenceType() << std::endl
		    << "[Variable] " << GetVariableName();

		m_WhatBuffer = oss.str();
		return m_WhatBuffer.c_str();
	}

	const char *NullReferenceException::GetReferenceType() const noexcept { return m_ReferenceType.c_str(); }
	const char *NullReferenceException::GetVariableName() const noexcept { return m_VariableName.c_str(); }
	const char *NullReferenceException::GetType() const noexcept { return "[Null Reference Exception]"; }
}