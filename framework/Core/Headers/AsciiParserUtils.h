#ifndef _ASCIIPARSERUTILS_H_
#define _ASCIIPARSERUTILS_H_

#include "Core.h"
#include "GenericRefCountedBaseClass.h"
#include "CoreRawBuffer.h"
#include "SmartPointer.h"


template<typename charType>
class ValidCharArray
{
protected:
	unsigned char*	myArray;
	int				myMinIndex;
	int				myMaxIndex;

	void	growArray(int c1, int c2)
	{
		int newmin = myMinIndex;
		int newmax = myMaxIndex;
		int ic1 = ((int)c1);
		if (ic1 < newmin)
		{
			newmin = ic1;
		}
		if (ic1 > newmax)
		{
			newmax = ic1;
		}
		int ic2 = ((int)c2);
		if (ic2 < newmin)
		{
			newmin = ic2;
		}
		if (ic2 > newmax)
		{
			newmax = ic2;
		}
		// create new array
		unsigned char*	newArray = new unsigned char[newmax - newmin + 1];
		memset(newArray, 0, newmax - newmin + 1 * sizeof(unsigned char));
		// copy old values
		if (myArray)
		{
			memcpy(newArray + myMinIndex - newmin, myArray, myMaxIndex - myMinIndex + 1);
			// delete old array
			delete[] myArray;
		}
		// replace 
		myArray = newArray;
		myMinIndex = newmin;
		myMaxIndex = newmax;
	}

public:
	ValidCharArray() : myArray(0)
		, myMinIndex(0x0FFFFFFF)
		, myMaxIndex(-1)
	{

	}
	~ValidCharArray()
	{
		if (myArray)
			delete[] myArray;
	}

	void	addValidCharacter(charType	c)
	{
		int ic = ((int)c);
		if ((ic >= myMinIndex) && (ic <= myMaxIndex))
		{
			// array is already ok
		}
		else // grow array
		{
			growArray(ic, ic);
		}
		myArray[ic - myMinIndex] = 1;
	}
	// c1 must be <= c2
	void	addValidCharacterRange(charType	c1, charType c2)
	{
		int ic1 = ((int)c1);
		int ic2 = ((int)c2);
		if ((ic1 >= myMinIndex) && (ic2 <= myMaxIndex))
		{
			// array is already ok
		}
		else
		{
			growArray(ic1, ic2);
		}
		for (int i = ic1; i <= ic2; i++)
		{
			myArray[i - myMinIndex] = 1;
		}
	}

	bool	isValid(charType c) const
	{
		int ic = ((int)c);
		if ((ic >= myMinIndex) && (ic <= myMaxIndex))
		{
			return (myArray[ic - myMinIndex] != 0);
		}
		return false;
	}
};
/**
		utility method to parse char stream
		check IniFileParser.cpp for a sample code
*/

template<typename charType>
class	AsciiParserUtilsTemplate 
{
protected:
	// empty constructor do nothing
	AsciiParserUtilsTemplate() : myBufferedEnd(0)
	{
		myTextLen=0;
		myCurrentReadPos = 0;
		myTextEnd = 0;
	}

public:

	AsciiParserUtilsTemplate(const AsciiParserUtilsTemplate<charType>& other) :  myText(other.myText), myTextEnd(other.myTextEnd), myTextLen(other.myTextLen), myCurrentReadPos(other.myCurrentReadPos), myBufferedEnd(0)
	{
		myRawBuffer=other.myRawBuffer;
	}

	AsciiParserUtilsTemplate(charType* buffer, int len);

	AsciiParserUtilsTemplate(CoreRawBuffer* buffer);

	~AsciiParserUtilsTemplate()
	{
		if(myBufferedEnd)
		{
			*myTextEnd=myBufferedEnd;
			myBufferedEnd=0;
		}
	}

	inline void	Reset()
	{
		myCurrentReadPos=0;
	}

	bool	GetLine(AsciiParserUtilsTemplate<charType>& result, bool	removeLeadTrailWhitespace = false);
	bool	GetWord(AsciiParserUtilsTemplate<charType>& result, charType	separator = ' ', bool	removeLeadTrailWhitespace = false);
	bool	GetWord(AsciiParserUtilsTemplate<charType>& result, const ValidCharArray<charType>&	validCharArray);
	bool	GetString(AsciiParserUtilsTemplate<charType>& result, charType	separator = ' ', bool uniqueSeparator = false);
	bool	GetChars(AsciiParserUtilsTemplate<charType>& result, int charCount);
	bool	GetPart(AsciiParserUtilsTemplate<charType>& result, int startPos,int charCount);
	bool	GoAfterNextSeparator(charType	separator);
	bool	GoToNextNonWhitespace();
	bool	GoToNextValid(const ValidCharArray<charType>&	validCharArray);
	bool	MoveToString(const charType*);
	bool	ReadInt(int&);
	bool	ReadFloat(float&);
	bool	ReadDouble(double&);
	bool    ReadChar(charType&);
	bool	ReadString(charType*, const int MAX_STRING_L = 512);
	bool	GetQuotationWord(AsciiParserUtilsTemplate<charType>& result);

	// search for given char starting at startpos and return found pos or -1 if not found
	unsigned int		FindNext(charType toFind, unsigned int startpos);

	// search for given word starting at startpos and return found pos or -1 if not found
	unsigned int		FindNext(const charType* toFind, unsigned int startpos);

	unsigned int		GetPosition()
	{
		return myCurrentReadPos;
	}

	void		SetPosition(unsigned int pos)
	{
		myCurrentReadPos=pos;
	}

	// retreive a block defined by a starting and ending character. If "embedded" blocks are found, don't stop to first end character but
	// push start / pop end until real block end is found
	bool	GetBlock(AsciiParserUtilsTemplate<charType>& result, charType	blkStart = (charType)'{', charType	blkEnd = (charType)'}');

	bool	GetBlockExcludeString(AsciiParserUtilsTemplate<charType>& result, charType	blkStart = (charType)'{', charType	blkEnd = (charType)'}');

	// get part before CurrentReadPos
	bool	GetLeadingPart(AsciiParserUtilsTemplate<charType>& result, bool	removeLeadTrailWhitespace = false);

	// get part before CurrentReadPos
	bool	GetTrailingPart(AsciiParserUtilsTemplate<charType>& result, bool	removeLeadTrailWhitespace = false);


	operator kstl::string() const;

	operator usString() const;

	const char * c_str();

	const unsigned short* us_str() const;

	charType operator*() const{
		charType* p = (charType*)myText + myCurrentReadPos;
		return *p;
	}

	kstl::string	subString(int startpos, int len);

	inline const charType& operator[](unsigned int i)const
	{ 
		if(i<myTextLen)
			return myText[i];

		return myZeroChar;
	}

	inline unsigned int	length()
	{
		return myTextLen;
	}

	// convert raw unsigned char buffer memory to string (each byte is converted to two letters from "a" to "p")
	static kstl::string	BufferToString(unsigned char* zone, unsigned int size);

	// convert string to raw unsigned char buffer (to delete with delete[] buffer)
	static unsigned char*	StringToBuffer(const kstl::string&, unsigned int& size);

protected:

	static void	copyUSToCharBuffer(unsigned char*,unsigned short*,int sizeMax);

	void	Set(charType* txt, int txtl = -1);

	SmartPointer<CoreRawBuffer>	myRawBuffer;
	charType*				myText = nullptr;
	charType*				myTextEnd = nullptr;
	unsigned int			myTextLen = 0;
	unsigned int			myCurrentReadPos = 0;
	static const charType	myZeroChar;
	charType				myBufferedEnd = 0;
};

typedef AsciiParserUtilsTemplate<char> AsciiParserUtils;
typedef AsciiParserUtilsTemplate<unsigned short> US16ParserUtils;

#endif //_ASCIIPARSERUTILS_H_