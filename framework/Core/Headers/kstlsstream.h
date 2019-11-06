#ifndef __KSTLSSTREAM_H__
#define __KSTLSSTREAM_H__

#include "Platform/Core/CorePlatformDefines.h"

#include "kstlstring.h"
#include <sstream>

#ifndef _NO_KSTL_OVERLOADING_

#include "CoreSTLAllocator.h"

namespace kstl
{
	//string stream io with simple strings
	using stringbuf = std::basic_stringbuf<char, std::char_traits<char>, CoreSTLAllocator<char> > ;
	using istringstream = std::basic_istringstream<char, std::char_traits<char>, CoreSTLAllocator<char> > ;
	using ostringstream = std::basic_ostringstream<char, std::char_traits<char>, CoreSTLAllocator<char> > ;
	using stringstream = std::basic_stringstream<char, std::char_traits<char>, CoreSTLAllocator<char> > ;

	//string stream io with wide (wchar_t) strings
	using wstringbuf = std::basic_stringbuf<wchar_t, std::char_traits<wchar_t>, CoreSTLAllocator<wchar_t> > ;
	using wistringstream = std::basic_istringstream<wchar_t, std::char_traits<wchar_t>, CoreSTLAllocator<wchar_t> > ;
	using wostringstream = std::basic_ostringstream<wchar_t, std::char_traits<wchar_t>, CoreSTLAllocator<wchar_t> > ;
	using wstringstream = std::basic_stringstream<wchar_t, std::char_traits<wchar_t>, CoreSTLAllocator<wchar_t> > ;
}
#else

namespace kstl = std;
#endif

#endif
