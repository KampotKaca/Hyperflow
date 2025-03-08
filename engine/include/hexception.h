#ifndef HEXCEPTION_H
#define HEXCEPTION_H

#include <string>
#include <exception>
#include <inttypes.h>

namespace hf
{
	class HyperException : public std::exception
	{
	public:
		HyperException(int32_t lineNum, const char* file) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;

		int32_t GetLine() const noexcept;
		const std::string& GetFile() const noexcept;

	private:
		int32_t m_Line;
		std::string m_File;

	protected:
		mutable std::string m_WhatBuffer;
	};
}

#endif //HEXCEPTION_H
