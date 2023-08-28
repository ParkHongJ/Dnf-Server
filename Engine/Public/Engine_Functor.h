#pragma once

namespace Engine
{
	class CTag_Finder
	{
	private:
		const wchar_t*	m_pStr;

	public:
		CTag_Finder(const wchar_t* pStr)
			: m_pStr(pStr)
		{

		}

	public:
		template<typename T>
		bool	operator()(T& Pair)
		{
			if (!lstrcmp(m_pStr, Pair.first))
				return true;

			return false;
		}
	};
}
