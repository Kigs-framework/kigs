#include "PrecompiledHeaders.h"
#include "usString.h"
#include "utf8_decode.h"

#include <stdlib.h>
#include <stdarg.h>


// ************************************************************
//
// Utilities to print formatted wchar strings
//
// ************************************************************


static void printcharw(unsigned short **str, int c)
{
	**str = (unsigned short)c;
	++(*str);
}

#define PADRIGHT 1
#define PAD_ZERO 2

static int printsw(unsigned short **out, const unsigned short *string, int width, int pad)
{
	int pc = 0, padchar = ' ';

	if (width > 0) {
		int len = 0;
		const unsigned short *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PADRIGHT)) {
		for (; width > 0; --width) {
			printcharw(out, padchar);
			++pc;
		}
	}
	for (; *string; ++string) {
		printcharw(out, *string);
		++pc;
	}
	for (; width > 0; --width) {
		printcharw(out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINTBUF_LEN 12

static int printiw(unsigned short **out, int i, int b, int sg, int width, int pad, int letbase)
{
	unsigned short printBuf[PRINTBUF_LEN];
	unsigned short *s;
	int t, neg = 0, pc = 0;
	unsigned int u = i;

	if (i == 0) {
		printBuf[0] = '0';
		printBuf[1] = '\0';
		return printsw(out, printBuf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = printBuf + PRINTBUF_LEN - 1;
	*s = '\0';

	while (u) {
		t = u % b;
		if (t >= 10)
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if (width && (pad & PAD_ZERO)) {
			printcharw(out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + printsw(out, s, width, pad);
}


static int printw(unsigned short **out, const unsigned short *format, va_list args)
{
	int width, pad;
	int pc = 0;
	unsigned short scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PADRIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if (*format == 's') {
				unsigned short *s = (unsigned short *)va_arg(args, int);
				if (s)
				{
					pc += printsw(out, s, width, pad);
				}
				continue;
			}
			if ((*format == 'd') || (*format == 'i')) {
				pc += printiw(out, va_arg(args, int), 10, 1, width, pad, 'a');
				continue;
			}
			if (*format == 'x') {
				pc += printiw(out, va_arg(args, int), 16, 0, width, pad, 'a');
				continue;
			}
			if (*format == 'X') {
				pc += printiw(out, va_arg(args, int), 16, 0, width, pad, 'A');
				continue;
			}
			if (*format == 'u') {
				pc += printiw(out, va_arg(args, int), 10, 0, width, pad, 'a');
				continue;
			}
			if (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = (unsigned short)va_arg(args, int);
				scr[1] = '\0';
				pc += printsw(out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			printcharw(out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end(args);
	return pc;
}




extern "C"
{
	int sprintfw(unsigned short* _Dest, const unsigned short* _format, ...)
	{
		va_list args;
		va_start(args, _format);
		return printw(&_Dest, _format, args);
	}


	int vsprintfw(unsigned short* _Dest, const unsigned short* _format, va_list args)
	{
		return printw(&_Dest, _format, args);
	}

	// warning ! no check on the src is done before copy on the null terminated & size of dest
	void		strcpyw(unsigned short* _Dest, const char* src)
	{
		unsigned char* _src = (unsigned char*)src;
		while (*_src)
		{
			*_Dest = *_src;
			++_Dest;
			++_src;
		}
	}

	void		strcpywUtoC(char * _Dest, const unsigned short * src)
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

	int setUnsignedShortToInt(unsigned short * buffer)
	{
		char tmptrans[32];
		strcpywUtoC(tmptrans, buffer);
		int number = atoi(tmptrans);
		return number;
	}

	float setUnsignedShortToFloat(unsigned short * buffer)
	{
		char tmptrans[32];
		strcpywUtoC(tmptrans, buffer);
		float number = (float)atof(tmptrans);
		return number;
	}

	void convertIntToString(char * buffer, int number)
	{
		if (number == 0) { buffer[0] = '0'; }

		char temp[10];

		int size = 0;
		if (number < 0) {
			number = number * -1;
			while (number > 0) {
				temp[size] = (number % 10) + 48;
				number /= 10;
				size++;
			}
			temp[size++] = '-';
		}
		else {
			while (number > 0) {
				temp[size] = (number % 10) + 48;
				number /= 10;
				size++;
			}
		}

		for (int i = 0; i < size; i++) {
			buffer[i] = temp[size - i - 1];
		}
		buffer[size++] = '\0';
	}

}



usString::usString()
{
	mString = new unsigned short[1];
	mString[0] = 0;
}

usString::usString(const usString& other) :
	mString(0)
{
	copy(other.us_str());
}

usString::~usString()
{
	if (mString)
		delete[] mString;
}

void usString::reserve(int size)
{
	int cursize = 0;
	if (mString) cursize = strlen(mString);
	int nsize = std::max(cursize, size);
	if (nsize > cursize)
	{
		auto nmString = new unsigned short[nsize + 1];
		nmString[nsize] = 0;
		if (mString)
		{
			memcpy(nmString, mString, (cursize + 1) * sizeof(unsigned short));
			delete[] mString;
		}
		mString = nmString;
	}
}


kstl::string usString::encode()
{
	unsigned int len = strlen();

	kstl::string result = "";

	if (len)
	{
		unsigned int i;
		char buffer[32];
		for (i = 0; i < len; i++)
		{
			int value = (int)mString[i];
			sprintf(buffer, "%d.", value);
			result += buffer;
		}
	}

	return result;
}

void usString::decode(const kstl::string& todecode)
{
	// first count '.' numbers to find decoded string len
	unsigned int len = todecode.length();
	char*	read = (char*)todecode.c_str();
	unsigned int dotcount = 0;
	unsigned int i = 0;
	for (i = 0; i < len; i++)
	{
		if ((*read++) == '.')
		{
			dotcount++;
		}
	}

	len = dotcount;
	if (len)
	{
		if (mString)
			delete[] mString;

		mString = new unsigned short[len + 1];

		unsigned short* write = mString;

		char*	read = (char*)todecode.c_str();
		char	buffer[32];
		unsigned int bufferpos = 0;
		int		value;

		for (i = 0; i < len; i++)
		{
			if ((*read) == '.')
			{
				buffer[bufferpos] = 0;
				sscanf(buffer, "%d", &value);
				bufferpos = 0;
				*write++ = (unsigned short)value;
			}
			else
			{
				buffer[bufferpos++] = *read;
			}
			++read;
		}
		*write = 0;
	}

}

void usString::copy(const UTF8Char* str)
{
	if (mString)
	{
		delete[] mString;
	}

	int decodedLen = 0;
	int strl = strlen((char*)str);
	utf8_decode decoder((char*)str, strl);
	while (decoder.utf8_decode_next() >= 0)
	{
		decodedLen++;
	}

	if (decodedLen)
	{
		mString = new unsigned short[decodedLen + 1];
		unsigned short*	write = mString;

		decoder.reset();
		int decoded = 0;
		do
		{
			decoded = decoder.utf8_decode_next();

			if (decoded < 65536)
			{
				if (decoded < 0)
				{
					break;
				}
				*write = decoded;
			}
			else
			{
				*write = 32;
			}
			++write;
		} while (decoded >= 0);
		*write = 0;
	}
	else
	{
		mString = new unsigned short[1];
		mString[0] = 0;
	}
}

usString::operator bool() const
{
	kstl::string result = ToString();
	if ((result == "true") || (result == "1"))
	{
		return true;
	}
	return false;
}

usString::operator float() const
{
	float result = (float)atof(ToString().c_str());
	return result;
}

usString::operator int() const
{
	int result = (int)atof(ToString().c_str());
	return result;
}

usString::operator unsigned int() const
{
	unsigned int result = (unsigned int)atof(ToString().c_str());
	return result;
}

usString::operator kstl::string() const
{
	kstl::string result = ToString();
	return result;
}

// remove all first level backslash escape character
void	usString::removeEscapeBackslash()
{
	unsigned int len = strlen();

	unsigned short*	read = mString;

	// new buffer will be shorter, but keep same size
	unsigned short* result = new unsigned short[len + 1];
	unsigned short* write = result;

	int inJumpCount = 0;
	while (*read)
	{
		if (inJumpCount)
		{
			inJumpCount--;
		}
		else
		{
			if (*read == '\\')
			{
				inJumpCount = 1;
			}
		}

		if (inJumpCount == 0)
		{
			*write = *read;
			++write;
		}
		++read;

	}
	// end
	*write = 0;

	if (mString)
		delete[] mString;
	mString = result;


}

std::string::size_type	usString::find(const usString& toFind, int startpos) const
{
	int Len = strlen(mString);
	int	toFindL = toFind.length();
	if ( (startpos+ toFindL) > Len) 
	{
		return  std::string::npos;
	}

	unsigned short*	read = ((unsigned short*)mString) +  startpos ;
	unsigned short*	readcompare = (unsigned short*)toFind.us_str();

	toFindL -= 1;

	int pos = startpos;
	while (*read)
	{
		if (*read == *readcompare)
		{
			bool found = true;
			// compare from end to start
			int i;
			for (i = toFindL; i > 0; i--)
			{
				if (read[i] != readcompare[i])
				{
					found = false;
					break;
				}
			}
			if (found)
				return pos;
		}
		++pos;
		++read;
		if ((pos + toFindL) >= Len)
		{
			break;
		}
	}
	return std::string::npos;
}

std::string::size_type	usString::rfind(const usString&  toFind, int startpos) const
{
	if (startpos < 0)
	{
		return  std::string::npos;
	}

	unsigned short*	read = ((unsigned short*)mString) + startpos;
	unsigned short*	readcompare = (unsigned short*)toFind.us_str();
	int	toFindL = toFind.length() - 1;

	unsigned int pos = startpos;
	while (*read)
	{
		if (*read == *readcompare)
		{
			bool found = true;
			// compare from end to start
			int i;
			for (i = toFindL; i > 0; i--)
			{
				if (read[i] != readcompare[i])
				{
					found = false;
					break;
				}
			}
			if (found)
				return pos;
		}
		--pos;
		--read;
	}
	return std::string::npos;
}

void usString::replaceEscapeUnicode()
{
	int Len = strlen(mString);
	if (Len == 0)
	{
		return;
	}

	usString result = "";

	usString toReplace = "\\u";

	int pos = 0;
	int lastpos = pos;
	pos = find(toReplace, pos);

	unsigned int Replacement = 32;

	while (pos != std::string::npos)
	{
		result += substr(lastpos, pos - lastpos);

		usString hexa = substr(pos + 2, 4);

		Replacement = (hexaCharacterToInt(hexa[0]) << 12) + (hexaCharacterToInt(hexa[1]) << 8) + (hexaCharacterToInt(hexa[2]) << 4) + (hexaCharacterToInt(hexa[3]));

		result += (unsigned short)Replacement;
		pos += 6; // \u####
		lastpos = pos;
		pos = find(toReplace, pos);
	}

	pos = Len;
	result += substr(lastpos, pos - lastpos);

	*this = result;

	result = "";

	// also replace \n
	toReplace = "\\n";

	pos = 0;
	lastpos = pos;
	pos = find(toReplace, pos);

	Replacement = 10;

	while (pos != std::string::npos)
	{
		result += substr(lastpos, pos - lastpos);
		result += (unsigned short)Replacement;
		pos += 2; // \n
		lastpos = pos;
		pos = find(toReplace, pos);
	}

	pos = Len;
	result += substr(lastpos, pos - lastpos);


	*this = result;

}

void	usString::replaceAllOccurence(const usString& toReplace, const usString& Replacement, int startpos)
{
	int Len = strlen(mString);
	int	toReplaceL = toReplace.length();
	
	if ((startpos + toReplaceL) > Len)
	{
		return;
	}

	usString result = "";

	int pos = 0;
	int lastpos = pos;
	pos = find(toReplace, pos);

	while (pos != std::string::npos)
	{
		result += substr(lastpos, pos - lastpos);
		result += Replacement;
		pos += toReplaceL;
		lastpos = pos;
		pos = find(toReplace, pos);
	}

	pos = Len;
	result += substr(lastpos, pos - lastpos);

	*this = result;
}

void	usString::replaceAt(const usString& Replacement, int start, int lenToReplace)
{
	int Len = strlen(mString);
	
	if ((start + lenToReplace) > Len)
	{
		return;
	}

	usString result = "";


	result += substr(0, start);
	result += Replacement;
	result += substr(start+ lenToReplace, Len - (start + lenToReplace));

	*this = result;
}

void usString::removeRange(size_t start, int len)
{
	if (len == 0) return;

	size_t size = strlen(mString);
	if (start + len >= size)
		len = -1;

	if (len == -1)
	{
		mString[start] = 0;
		return;
	}
	memmove(mString + start, mString + (start + len), (size + 1 - start - len) * sizeof(unsigned short));
}