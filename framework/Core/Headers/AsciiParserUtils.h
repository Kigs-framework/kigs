#ifndef _ASCIIPARSERUTILS_H_
#define _ASCIIPARSERUTILS_H_

#include "Core.h"
#include "GenericRefCountedBaseClass.h"
#include "CoreRawBuffer.h"
#include "SmartPointer.h"


// ****************************************
// * ValidCharArray class
// * --------------------------------------
/**
* \file	AsciiParserUtils.h
* \class	ValidCharArray
* \ingroup Core
* \brief	Utility class used by AsciiParserUtils.
*/
// ****************************************

template<typename charType>
class ValidCharArray
{
protected:
	unsigned char*	mArray;
	int				mMinIndex;
	int				mMaxIndex;

	void	growArray(int c1, int c2)
	{
		int newmin = mMinIndex;
		int newmax = mMaxIndex;
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
		if (mArray)
		{
			memcpy(newArray + mMinIndex - newmin, mArray, mMaxIndex - mMinIndex + 1);
			// delete old array
			delete[] mArray;
		}
		// replace 
		mArray = newArray;
		mMinIndex = newmin;
		mMaxIndex = newmax;
	}

public:
	ValidCharArray() : mArray(0)
		, mMinIndex(0x0FFFFFFF)
		, mMaxIndex(-1)
	{

	}
	~ValidCharArray()
	{
		if (mArray)
			delete[] mArray;
	}

	void	addValidCharacter(charType	c)
	{
		int ic = ((int)c);
		if ((ic >= mMinIndex) && (ic <= mMaxIndex))
		{
			// array is already ok
		}
		else // grow array
		{
			growArray(ic, ic);
		}
		mArray[ic - mMinIndex] = 1;
	}
	// c1 must be <= c2
	void	addValidCharacterRange(charType	c1, charType c2)
	{
		int ic1 = ((int)c1);
		int ic2 = ((int)c2);
		if ((ic1 >= mMinIndex) && (ic2 <= mMaxIndex))
		{
			// array is already ok
		}
		else
		{
			growArray(ic1, ic2);
		}
		for (int i = ic1; i <= ic2; i++)
		{
			mArray[i - mMinIndex] = 1;
		}
	}

	bool	isValid(charType c) const
	{
		int ic = ((int)c);
		if ((ic >= mMinIndex) && (ic <= mMaxIndex))
		{
			return (mArray[ic - mMinIndex] != 0);
		}
		return false;
	}
};

// ****************************************
// * AsciiParserUtils class
// * --------------------------------------
/**
* \file	AsciiParserUtils.h
* \class	AsciiParserUtilsTemplate
* \ingroup Core
* \brief	Utility class to parse ascii buffer.
*
* Utility method to parse char stream. 
* Check IniFileParser.cpp for a sample code.
* 
*/
// ****************************************

template<typename charType>
class	AsciiParserUtilsTemplate 
{
protected:
	// empty constructor do nothing
	AsciiParserUtilsTemplate() : mBufferedEnd(0)
	{
		mTextLen=0;
		mCurrentReadPos = 0;
		mTextEnd = 0;
	}

public:

	AsciiParserUtilsTemplate(const AsciiParserUtilsTemplate<charType>& other) :  mText(other.mText), mTextEnd(other.mTextEnd), mTextLen(other.mTextLen), mCurrentReadPos(other.mCurrentReadPos), mBufferedEnd(0)
	{
		mRawBuffer=other.mRawBuffer;
	}

	AsciiParserUtilsTemplate(charType* buffer, int len);

	AsciiParserUtilsTemplate(CoreRawBuffer* buffer);

	~AsciiParserUtilsTemplate()
	{
		if(mBufferedEnd)
		{
			*mTextEnd=mBufferedEnd;
			mBufferedEnd=0;
		}
	}

	inline void	Reset()
	{
		mCurrentReadPos=0;
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
		return mCurrentReadPos;
	}

	void		SetPosition(unsigned int pos)
	{
		mCurrentReadPos=pos;
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
		charType* p = (charType*)mText + mCurrentReadPos;
		return *p;
	}

	kstl::string	subString(int startpos, int len);

	inline const charType& operator[](unsigned int i)const
	{ 
		if(i<mTextLen)
			return mText[i];

		return mZeroChar;
	}

	inline unsigned int	length()
	{
		return mTextLen;
	}

	// convert raw unsigned char buffer memory to string (each byte is converted to two letters from "a" to "p")
	static kstl::string	BufferToString(unsigned char* zone, unsigned int size);

	// convert string to raw unsigned char buffer (to delete with delete[] buffer)
	static unsigned char*	StringToBuffer(const kstl::string&, unsigned int& size);

protected:

	static void	copyUSToCharBuffer(unsigned char*,unsigned short*,int sizeMax);

	void	Set(charType* txt, int txtl = -1);

	SmartPointer<CoreRawBuffer>	mRawBuffer;
	charType*				mText = nullptr;
	charType*				mTextEnd = nullptr;
	unsigned int			mTextLen = 0;
	unsigned int			mCurrentReadPos = 0;
	static const charType	mZeroChar;
	charType				mBufferedEnd = 0;
};

typedef AsciiParserUtilsTemplate<char> AsciiParserUtils;
typedef AsciiParserUtilsTemplate<unsigned short> US16ParserUtils;

#endif //_ASCIIPARSERUTILS_H_