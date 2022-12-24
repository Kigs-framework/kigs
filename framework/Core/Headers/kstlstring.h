#pragma once

#include "Platform/Core/CorePlatformDefines.h"

#include <string>
#include <algorithm>
#include <locale>

namespace Kigs
{
	namespace Core
	{
		static inline std::string ToUpperCase(const std::string& a_entry)
		{
			std::locale loc;
			std::string str = "";

			for (std::string::size_type i = 0; i < a_entry.size(); ++i)
				str += std::toupper(a_entry[i], loc);

			return str;
		}

		static inline void str_toupper(std::string& s) {
			std::transform(s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return std::toupper(c); }
			);
		}

		static inline std::string ToLowerCase(const std::string& a_entry)
		{
			std::locale loc;
			std::string str = "";

			for (std::string::size_type i = 0; i < a_entry.size(); ++i)
				str += std::tolower(a_entry[i], loc);

			return str;
		}

		static inline void str_tolower(std::string& s) {
			std::transform(s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return std::tolower(c); }
			);
		}


	}
}
