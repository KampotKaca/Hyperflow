#ifndef HEXCEPTION_H
#define HEXCEPTION_H

#include <string>
#include <exception>
#include <cinttypes>

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

	class NullReferenceException : public HyperException
	{
	public:
		NullReferenceException(int32_t lineNum, const char* file, const char* type, const char *variable) noexcept;
		const char* what() const noexcept override;
		const char* GetReferenceType() const noexcept;
		const char* GetVariableName() const noexcept;
		const char * GetType() const noexcept override;

	private:
		mutable std::string m_ReferenceType;
		mutable std::string m_VariableName;
	};

#define NULL_REF_EXCEPTION(type, variable) NullReferenceException(__LINE__, __FILE__, #type, #variable)
}

#endif //HEXCEPTION_H
