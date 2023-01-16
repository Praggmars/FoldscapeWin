#pragma once

#include <string>
#include <complex>

namespace foldscape
{
	template <typename T>
	class StrFunction
	{
		class FuncElem
		{
		};

		class ConstValue : public FuncElem
		{
			const std::complex<T> m_value;
		};

		class Variable : public FuncElem
		{
			const int m_varIndex;
		};

		template <size_t P>
		class Operation : public FuncElem
		{
			const FuncElem& m_param[P];
			const wchar_t* const m_funcName;
			std::complex<T>(*m_function const)(std::complex<T>[]);
		};

		std::wstring m_function;

	public:
		StrFunction() = default;
		StrFunction(const wchar_t* str) : StrFunction()
		{
			Parse();
		}

		void Clear()
		{
			m_function.clear();
		}

		void Parse(const wchar_t* str)
		{

		}

		std::wstring&& ShaderFunction() const
		{

		}
	};
}