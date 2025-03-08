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
}