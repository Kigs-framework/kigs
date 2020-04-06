#ifndef _USSTRING_H
#define _USSTRING_H

#include "CoreTypes.h"
#include <string>
#include <locale>

// ****************************************
// * usString class
// * --------------------------------------
/**
* \class	usString
*/
// ****************************************
// Unsigned short String Management

#include "kstlvector.h"

// define sprintf on wchar
extern "C" int		sprintfw(unsigned short* _Dest, const unsigned short* _format, ...);
extern "C" int		vsprintfw(unsigned short* _Dest, const unsigned short* _format, va_list args);
// copy a nul terminated string to a wchar buffer
extern "C" void		strcpyw(unsigned short* _Dest, const char* src);
extern "C" void		strcpywUtoC(char * _Dest, const unsigned short * src);
extern "C" int		setUnsignedShortToInt(unsigned short * buffer);
extern "C" float	setUnsignedShortToFloat(unsigned short * buffer);
extern "C" void		convertIntToString(char * buffer, int number);

typedef unsigned char		UTF8Char;

class	usString
{
public:
	usString();

	// set this one as explicit to avoid ambiguity for method with kstl::string or usString parameter called with const char* parameter
	explicit usString(const char* str) :
		mString(0)
	{
		copy(str);
	}

	usString(const UTF8Char* str) :
		mString(0)
	{
		copy(str);
	}

	usString(const unsigned short* str) :
		mString(0)
	{
		copy(str);
	}
	
	usString(const kstl::string& other) : mString(0) {
		copy(other.c_str());
	}
	
	usString(const usString& other);

	// reserve clear the current string if any
	void reserve(int size);

	~usString();

	// cast operator
	operator bool() const;

	operator float() const;

	operator int() const;

	operator unsigned int() const;

	explicit operator kstl::string() const;

	const unsigned short*	us_str() const
	{
		return mString;
	}
	/*const char*	c_str() const
	{
		char * _Dest = new char[length()];
		this->strcpywUtoC(_Dest, mString);
		return _Dest;
	}*/


	bool	operator == (const char* str)
	{
		usString usstr(str);
		return usstr == *this;
	}

	bool	operator == (const usString& other)
	{
		const unsigned short* read_char1 = mString;
		const unsigned short* read_char2 = other.mString;
		
		while (*read_char2 == *read_char1)
		{
			if (*read_char1 == 0)
			{
				return true;
			}
			++read_char2;
			++read_char1;
		}

		return false;
	}

	inline const unsigned short& operator[](unsigned int i)const
	{
		// warning, no check for out of bound
		return mString[i];
	}

	inline unsigned short& operator[](unsigned int i)
	{
		// warning, no check for out of bound
		return mString[i];
	}

	void		strcpywUtoC(char * _Dest, const unsigned short * src)const
	{
		const unsigned short* read_char = src;
		char* write_char = _Dest;

		while (*read_char)
		{
			*write_char = (char)*read_char;
			++read_char;
			++write_char;
		}

		*write_char = 0;
	}

	bool operator<(const usString& other) const
	{
		const unsigned short* read_char1 = mString;
		const unsigned short* read_char2 = other.mString;

		while (*read_char1)
		{
			if (*read_char1 < *read_char2)
			{
				return true;
			}
			else if (*read_char1 > *read_char2)
			{
				return false;
			}
			++read_char1;
			++read_char2;
		}
		return false;
	}

	kstl::string		ToString() const
	{
		kstl::string ret = "";
		ret.reserve(strlen());

		const unsigned short* read_char = mString;

		while (*read_char)
		{
			ret += ((char)*read_char);
			++read_char;
		}

		return ret;
	}

	// encode current unsigned short string to a char string :
	// each unsigned short is print as a integer and each integer is separated by '.'
	kstl::string encode();

	// decode a serie of integer separated by '.' to a unsigned short string
	void decode(const kstl::string& todecode);


	// len = -1 remove everything from start to end of the string
	void removeRange(size_t start, int len);

	kstl::vector<usString>	SplitByCharacter(unsigned short _value) const
	{
		kstl::vector<usString> returnedString;

		unsigned short* read = mString;

		unsigned int startCount = 0;
		unsigned int count = 0;
		while (*read)
		{
			if ((*read) == _value)
			{
				unsigned int size = count + 1;
				unsigned short* temp = new unsigned short[size];

				memset(temp, 0, (size) * sizeof(unsigned short));
				memcpy(temp, mString + startCount, count * sizeof(unsigned short));
				returnedString.push_back(temp);
				delete[] temp;

				startCount += size;
				count = 0;
				++read;
			}
			else
			{
				++read;
				count++;
			}

		}
		unsigned int size = count + 1;
		unsigned short* temp = new unsigned short[size];

		memset(temp, 0, (size) * sizeof(unsigned short));
		memcpy(temp, mString + startCount, count * sizeof(unsigned short));
		returnedString.push_back(temp);
		delete[] temp;

		return returnedString;
	}

	static inline unsigned int strlen(const char* str)
	{
		char*	read = (char*)str;

		unsigned int len = 0;

		while (*read)
		{
			++read;
			++len;
		}

		return len;

	}

	bool	empty() const
	{
		return (strlen(mString) == 0);
	}

	inline unsigned int length() const
	{
		return strlen(mString);
	}

	usString	substr(int pos, int len) const
	{
		usString result;
		int origlen = strlen(mString);
		if ((origlen - pos) < len)
		{
			len = origlen - pos;
			if (len < 0)
			{
				len = 0;
			}
		}

		if (result.mString)
			delete[] result.mString;
		result.mString = new unsigned short[len + 1];
		memcpy(result.mString, mString + pos, len * sizeof(unsigned short));
		result.mString[len] = 0;

		return result;
	}

	std::string::size_type	find(unsigned short toFind, int startpos = 0) const 
	{
		int Len = strlen(mString);
		if (startpos >= Len)
		{
			return  std::string::npos;
		}

		unsigned short*	read = ((unsigned short*)mString) + startpos;
		unsigned int pos = startpos;
		while (*read)
		{
			if (*read == toFind)
			{
				return pos;
			}
			++pos;
			++read;
		}
		return std::string::npos;
	}
	std::string::size_type	rfind(unsigned short toFind, int startpos = 0) const 
	{
		int Len = strlen(mString);
		if (startpos < 0)
		{
			return  std::string::npos;
		}

		unsigned short*	read = ((unsigned short*)mString) + startpos;
		unsigned int pos = Len - 1 - startpos;
		while (*read)
		{
			if (*read == toFind)
			{
				return pos;
			}
			--pos;
			--read;
		}
		return std::string::npos;
	}

	std::string::size_type	find(const usString& toFind, int startpos = 0) const;
	std::string::size_type	rfind(const usString& toFind, int startpos = 0) const;

	
	void	replaceAllOccurence(const usString& toReplace, const usString& Replacement, int startpos = 0);

	void	replaceAt(const usString& Replacement, int start,int lenToReplace);

	void replaceEscapeUnicode();

	static inline unsigned int strlen(const unsigned short* str)
	{
		unsigned short*	read = (unsigned short*)str;

		unsigned int len = 0;

		while (*read)
		{
			++read;
			++len;
		}

		return len;

	}

	inline unsigned int strlen() const
	{
		return strlen(mString);
	}

	usString& operator=(const usString& toCopy)
	{
		copy(toCopy.us_str());

		return *this;
	}
	/*
	usString& operator=(const unsigned short* toCopy)
	{
		copy(toCopy);
		return *this;
	}

	usString& operator=(const char * toCopy)
	{
		copy(toCopy);
		return *this;
	}

	usString& operator=(const UTF8Char * toCopy)
	{
		copy(toCopy);
		return *this;
	}

	usString& operator=(const kstl::string& toCopy)
	{
		copy(toCopy.c_str());
		return *this;
	}*/

	bool operator==(const usString& _value) const
	{
		unsigned int len1 = _value.strlen();
		unsigned int len2 = strlen();

		if (len1 == len2)
		{
			unsigned short*	read = _value.mString;
			unsigned short* read2 = mString;

			while (*read)
			{
				if ((*read2) != (*read))
				{
					return false;
				}
				++read;
				++read2;
			}

			return true;
		}

		return false;
	}

	bool operator==(unsigned short* _value) const
	{
		unsigned int len1 = strlen(_value);
		unsigned int len2 = strlen();

		if (len1 == len2)
		{
			unsigned short*	read = _value;
			unsigned short* read2 = mString;

			while (*read)
			{
				if ((*read2) != (*read))
				{
					return false;
				}
				++read;
				++read2;
			}

			return true;
		}

		return false;
	}

	bool operator!=(const usString& _value) const
	{
		unsigned int len1 = _value.strlen();
		unsigned int len2 = strlen();

		if (len1 == len2)
		{
			unsigned short*	read = _value.mString;
			unsigned short* read2 = mString;

			while (*read)
			{
				if ((*read2) != (*read))
				{
					return true;
				}
				++read;
				++read2;
			}

			return false;
		}

		return true;
	}

	bool operator!=(unsigned short* _value) const
	{
		unsigned int len1 = strlen(_value);
		unsigned int len2 = strlen();

		if (len1 == len2)
		{
			unsigned short*	read = _value;
			unsigned short* read2 = mString;

			while (*read)
			{
				if ((*read2) != (*read))
				{
					return true;
				}
				++read;
				++read2;
			}
			return false;
		}
		return true;
	}

	usString& operator+=(const usString& _value)
	{
		unsigned int len1 = _value.strlen();
		unsigned int len2 = strlen();

		unsigned int totalsize = len1 + len2 + 1;

		unsigned short* result = new unsigned short[totalsize];
		memset(result, 0, totalsize * sizeof(unsigned short));
		if (mString)
			memcpy(result, mString, len2 * sizeof(unsigned short));

		memcpy(result + len2, _value.mString, len1 * sizeof(unsigned short));

		if (mString)
			delete[] mString;
		mString = result;

		return *this;

	}

	usString& operator+=(unsigned short _value)
	{
		unsigned int len2 = strlen();


		unsigned int totalsize = 1 + len2 + 1;

		unsigned short* result = new unsigned short[totalsize];
		result[totalsize - 2] = _value;
		result[totalsize - 1] = 0;
		if (mString)
			memcpy(result, mString, len2 * sizeof(unsigned short));

		if (mString)
			delete[] mString;
		mString = result;

		return *this;

	}

	inline unsigned int hexaCharacterToInt(unsigned short character)
	{
		unsigned int result = 0;
		if ((character >= 0x30) && (character <= 0x39))
		{
			result = character - 0x30;
		}
		else if ((character >= 0x61) && (character <= 0x66))
		{
			result = character - 0x61 + 10;
		}
		else if ((character >= 0x41) && (character <= 0x46)) // manage upper character also ?
		{
			result = character - 0x61 + 10;
		}
		return result;
	}

	usString& operator+=(unsigned short* _value)
	{
		unsigned int len1 = strlen(_value);
		unsigned int len2 = strlen();

		unsigned int totalsize = len1 + len2 + 1;

		unsigned short* result = new unsigned short[totalsize];
		memset(result, 0, totalsize * sizeof(unsigned short));
		if (mString)
			memcpy(result, mString, len2 * sizeof(unsigned short));
		memcpy(result + len2, _value, len1 * sizeof(unsigned short));

		if (mString)
			delete[] mString;
		mString = result;
		return *this;
	}

	void	removeEscapeBackslash();

protected:

	inline void copy(const char* str)
	{
		if (mString)
		{
			delete[] mString;
		}
		unsigned int len = strlen(str);
		mString = new unsigned short[len + 1];
		unsigned char*	read = (unsigned char*)str;
		unsigned short*	write = mString;
		*write = *read;

		while (*read)
		{
			*++write = *++read;
		}

	}

	inline void copy(const unsigned short* str)
	{
		// mString is always valid and allocated
		if (mString)
			delete[] mString;

		unsigned int len = strlen(str);
		mString = new unsigned short[len + 1];
		memcpy(mString, str, (len + 1) * sizeof(unsigned short));
	}

	void copy(const UTF8Char* str);


	unsigned short*	mString;
};


inline usString operator+(const usString& _l, const usString& _r)
{
	usString ret = _l;
	ret += _r;

	return ret;
}

static inline usString ToLowerCase(const usString& a_entry)
{
	std::locale loc;
	usString str;

	unsigned int al = a_entry.length();

	for (unsigned int i = 0; i < al; ++i)
	{
		if (a_entry[i] < 128)
		{
			str += std::tolower((char)a_entry[i], loc);
		}
		else
		{
			str += a_entry[i];
		}
	}
	return str;
}

#endif // _USSTRING_H
