#ifndef __KSTLSTRING_H__
#define __KSTLSTRING_H__

#include "Platform/Core/CorePlatformDefines.h"

#include <string>

#ifndef _NO_KSTL_OVERLOADING_

#include "CoreSTLAllocator.h"

namespace kstl
{
	using string = std::basic_string<char, std::char_traits<char>, CoreSTLAllocator<char>>;
	using wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, CoreSTLAllocator<wchar_t>>;
	using u16string = std::basic_string<char16_t, std::char_traits<char16_t>, CoreSTLAllocator<char16_t>>;

	inline string to_string(int i)
	{
		char buffer[32]; 
		snprintf(buffer, 32, "%d", i);
		return buffer;
	}
}



#else	// _NO_KSTL_OVERLOADING_


namespace kstl = std;
#endif // _NO_KSTL_OVERLOADING_

#include <locale>

static inline kstl::string ToUpperCase(const kstl::string& a_entry)
{
	std::locale loc;
	kstl::string str = "";

	for (std::string::size_type i = 0; i<a_entry.size(); ++i)
		str += std::toupper(a_entry[i], loc);

	return str;
}

static inline kstl::string ToLowerCase(const kstl::string& a_entry)
{
	std::locale loc;
	kstl::string str = "";

	for (std::string::size_type i = 0; i<a_entry.size(); ++i)
		str += std::tolower(a_entry[i], loc);

	return str;
}

#ifdef __ANDROID__
#include "kstlsstream.h"
// Workaround for Android NDK builds (version r10e) that does not support std::to_string
namespace std
{
	template <typename T>
	kstl::string to_string(T value)
	{
		kstl::ostringstream tmp;
		tmp << value;
		return tmp.str();
	}
}
#endif

#endif
