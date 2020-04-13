#include "PrecompiledHeaders.h"
#include "AsciiParserUtils.h"

template<typename charType>
const charType	AsciiParserUtilsTemplate<charType>::myZeroChar = 0;

template<typename charType>
AsciiParserUtilsTemplate<charType>::AsciiParserUtilsTemplate(CoreRawBuffer* buffer)
{
	myRawBuffer = NonOwningRawPtrToSmartPtr(buffer);
	// text len is buffer len -1
	Set((charType*)buffer->buffer(), buffer->length() / sizeof(charType) - 1);

}

template<typename charType>
AsciiParserUtilsTemplate<charType>::AsciiParserUtilsTemplate(charType* buffer, int len)
{
	Set(buffer, len);
}


template<typename charType>
void	AsciiParserUtilsTemplate<charType>::Set(charType* txt, int txtl)
{
	if(myBufferedEnd)
	{
		*myTextEnd=myBufferedEnd;
	}

	myText=txt;
	myCurrentReadPos=0;

	if(txtl == -1)
	{
		myTextLen = 0;
		charType* readtxt = txt;
		while (*readtxt != 0)
		{
			++readtxt;
			++myTextLen;
		}
	}
	else
	{
		myTextLen=txtl;
	}

	myTextEnd = ((charType*)txt) + myTextLen;
	myBufferedEnd=*(myTextEnd);
	*(myTextEnd)=0;
}

// try to get a line at current position in the char stream
// if a line was found, return true, and fill AsciiParserUtilsTemplate struct name result, with the found line data
template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GetLine(AsciiParserUtilsTemplate<charType>& result, bool	removeLeadTrailWhitespace)
{
	bool	found=false;
	charType *p = ((charType*)myText) + myCurrentReadPos;
	while(!found)
	{
		if(removeLeadTrailWhitespace)
		{
			while (*p <= (charType)' ' && p<myTextEnd)
				p++;
		}

		charType *pLineStart = p;

		while ((*p != (charType)'\n') && (*p != (charType)'\r') && (p<myTextEnd))
			p++;

		charType *pLineEnd = p;

		if(removeLeadTrailWhitespace)
		{
			while ((*(pLineEnd - 1)) <= (charType)' ' && pLineEnd>pLineStart)
			{
				pLineEnd--;
			}
		}

		int lsize = ((int)((unsigned char*)pLineEnd - (unsigned char*)pLineStart)) / sizeof(charType);
		if(lsize>0)
		{
			result.Set(pLineStart, lsize);
			found=true;
		}

		while ((*p == (charType)'\n' || *p == (charType)'\r' || *p == 0) && p<myTextEnd)
			p++;	

		myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText) / sizeof(charType));
			
		if (p==myTextEnd)
		{
			myCurrentReadPos=myTextLen;
			break;
		}
	}
	return found;
}

// try to get a word at current position in the char stream
// a word is a list of characters separated by a separator (default is white space) 
template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GetWord(AsciiParserUtilsTemplate<charType>& result, charType	separator, bool	removeLeadTrailWhitespace)
{
	bool	found=false;
	charType *p = ((charType*)myText) + myCurrentReadPos;
	while(!found)
	{
		// remove previous separators
		while (((*p<(charType)' ') || (*p == separator)) && p<myTextEnd)
			p++;

		if(removeLeadTrailWhitespace)
		{
			while (*p <= (charType)' ' && p<myTextEnd)
				p++;
		}

		charType *pWordStart = p;

		// search word end
		while (*p >= (charType)' ' && *p != separator && p<myTextEnd)
			p++;

		charType *pWordEnd = p;
		if(removeLeadTrailWhitespace)
		{
			while ((*(pWordEnd - 1)) <= (charType)' ' && pWordEnd>pWordStart)
			{
				pWordEnd--;
			}
		}

		int lsize = ((int)((unsigned char*)pWordEnd - (unsigned char*)pWordStart)) / sizeof(charType);
		if(lsize>0)
		{
			result.Set(pWordStart, lsize );
			found=true;
		}

		// remove next separators
		while (((*p<(charType)' ') || (*p == separator) || (*p == (charType)0)) && p<myTextEnd)
			p++;

		myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);
			
		if (p==myTextEnd)
		{
			myCurrentReadPos=myTextLen;
			break;
		}
	}
	return found;
}

template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GetWord(AsciiParserUtilsTemplate<charType>& result, const ValidCharArray<charType>&	validCharArray)
{
	bool	found = false;
	charType *p = ((charType*)myText) + myCurrentReadPos;
	while (!found)
	{
		// remove previous invalid
		while ((!validCharArray.isValid(*p)) && p<myTextEnd)
			p++;

		charType *pWordStart = p;

		// search word end
		while (validCharArray.isValid(*p) && p<myTextEnd)
			p++;

		charType *pWordEnd = p;

		int lsize = ((int)((unsigned char*)pWordEnd - (unsigned char*)pWordStart)) / sizeof(charType);
		if (lsize>0)
		{
			result.Set(pWordStart, lsize);
			found = true;
		}

		// remove next invalid
		while ((!validCharArray.isValid(*p)) && p<myTextEnd)
			p++;

		myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);

		if (p == myTextEnd)
		{
			myCurrentReadPos = myTextLen;
			break;
		}
	}
	return found;
}

template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GetQuotationWord(AsciiParserUtilsTemplate<charType>& result)
{
	bool	found = false;
	charType *p = ((charType*)myText) + myCurrentReadPos;
	charType *prevP = p;
	while (!found)
	{
		// remove previous white space
		while ((*p<=(charType)' ') && p<myTextEnd)
		{
			prevP = p;
			p++;
		}
		// search first quotation mark (word Start)
		// first quote can not be escaped 
		while ((*p != (charType)'"') && (p<myTextEnd))
		{
			prevP = p;
			p++;
		}

		charType *pWordStart = p;
		bool EscapeChar = false;
		if (p < myTextEnd)
		{
			// next character
			prevP = p;
			p++;
		}
		if (*prevP == (charType)'\\')
		{
			EscapeChar = !EscapeChar;
		}

		// search word end
		while (((*p != (charType)'"') || (EscapeChar)) && p<myTextEnd)
		{
			prevP = p;
			p++;
			if (*prevP == (charType)'\\')
			{
				EscapeChar = !EscapeChar;
			}
			else
			{
				EscapeChar = false;
			}
		}
		// second quotation mark is included in word
		if (p < myTextEnd)
		{
			// next character
			prevP = p;
			p++;
		}

		charType *pWordEnd = p;
	
		int lsize = ((int)((unsigned char*)pWordEnd - (unsigned char*)pWordStart)) / sizeof(charType);
		if (lsize>0)
		{
			result.Set(pWordStart, lsize);
			found = true;
		}

		myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);

		if (p == myTextEnd)
		{
			myCurrentReadPos = myTextLen;
			break;
		}
	}
	return found;
}

// read charCount chars
template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GetChars(AsciiParserUtilsTemplate<charType>& result, int charCount)
{
	
	charType *p = ((charType*)myText) + myCurrentReadPos;
	int currentCount = 0;
	charType *pWordStart = p;

	// search word end
	while (currentCount < charCount && p < myTextEnd)
	{
		currentCount++;
		p++;
	}
		
	charType *pWordEnd = p;

	int lsize = ((int)((unsigned char*)pWordEnd - (unsigned char*)pWordStart)) / sizeof(charType);

	if (lsize>0)
	{
		result.Set(pWordStart, lsize);
	}

	myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);

	if (p == myTextEnd)
	{
		myCurrentReadPos = myTextLen;
	}
	
	return (lsize>0);
}

template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GetPart(AsciiParserUtilsTemplate<charType>& result, int startPos, int charCount)
{
	charType *p = ((charType*)myText) + startPos;
	int currentCount = 0;
	charType *pWordStart = p;

	// search word end
	while (currentCount < charCount && p < myTextEnd)
	{
		currentCount++;
		p++;
	}

	charType *pWordEnd = p;

	int lsize = ((int)((unsigned char*)pWordEnd - (unsigned char*)pWordStart)) / sizeof(charType);

	if (lsize>0)
	{
		result.Set(pWordStart, lsize);
	}

	myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);

	if (p == myTextEnd)
	{
		myCurrentReadPos = myTextLen;
	}

	return (lsize>0);
}

// retreive the string before the next separator
template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GetString(AsciiParserUtilsTemplate<charType>& result, charType	separator, bool uniqueSeparator)
{
	bool	found=false;
	charType *p = ((charType*)myText) + myCurrentReadPos;
	while(!found)
	{
		// remove previous separators
		if(!uniqueSeparator)
		{
			while((*p==separator) && p<myTextEnd)
				p++;
		}

		charType *pWordStart = p;

		// search word end
		while(*p!=separator && p<myTextEnd)
			p++;

		charType *pWordEnd = p;

		int lsize = ((int)((unsigned char*)pWordEnd - (unsigned char*)pWordStart)) / sizeof(charType);

		// remove next separators
		if(!uniqueSeparator)
		{
			if(lsize>0)
			{
				result.Set(pWordStart, lsize);
				found=true;
			}

			while(((*p==separator) || (*p==(charType)0))  && p<myTextEnd)
				p++;
		}
		else
		{
			result.Set(pWordStart, lsize);
			found=true;

			if (((*p == separator) || (*p == (charType)0)) && p<myTextEnd)
				p++;
		}

		myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);
			
		if (p==myTextEnd)
		{
			myCurrentReadPos=myTextLen;
			break;
		}
	}
	return found;
}

template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::MoveToString(const charType* tofound)
{
	bool	found=false;
	charType *p = ((charType*)myText) + myCurrentReadPos;
	charType* toFoundChar = (charType*)tofound;
	while(!found)
	{
		if((*toFoundChar)!=(*p))
		{
			if (*toFoundChar == (charType)0)
			{
				// this is the end of tofound, so we have found the whole word
				myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);
				found=true;
				break;
			}
			toFoundChar = (charType*)tofound;
			++p;
			if(p>=myTextEnd)
			{
				// not found
				break;
			}
		}
		else // the word to find is in recognition
		{
			++toFoundChar;
			++p;
			if(p>=myTextEnd)
			{
				// not found
				break;
			}
		}
	}
	return found;
}

template<>
bool	AsciiParserUtilsTemplate<char>::ReadInt(int& result)
{
	// search int start
	bool	found=false;
	char *p = ((char*)myText) + myCurrentReadPos;
	while(!found)
	{
		if (((*p) == (char)' ') || ((*p) < (char)0x13)) // separators or special chars
		{
			++p;
			if(p>=myTextEnd)
			{
				// not found
				break;
			}
		}
		else // the word to find is in recognition
		{
			int readlen;
			int test=sscanf(p,"%d%n",&result,&readlen);
			
			if(test)
			{
				myCurrentReadPos = (p - (char*)myText);
				found=true;
				myCurrentReadPos+=readlen;
			}
			
			break;
		}
	}
	return found;	
}

template<>
bool    AsciiParserUtilsTemplate<char>::ReadChar(char& result)
{
	bool	found = false;
	char *p = ((char*)myText) + myCurrentReadPos;
	if (p < myTextEnd)
	{
		result = *p;
		myCurrentReadPos++;
		found = true;
	}

	return found;
}

template<>
bool	AsciiParserUtilsTemplate<char>::ReadFloat(float& result)
{
	// search int start
	bool	found=false;
	char *p = ((char*)myText) + myCurrentReadPos;
	while(!found)
	{
		if (((*p) == (char)' ') || ((*p) < (char)0x13)) // separators or special chars
		{
			++p;
			if(p>=myTextEnd)
			{
				// not found
				break;
			}
		}
		else 
		{
			
			int readlen;
			int test=sscanf(p,"%f%n",&result,&readlen);
			
			if(test)
			{
				
				myCurrentReadPos = (p - (char*)myText);
				found=true;
				myCurrentReadPos+=readlen;
			}
			break;
		}
	}
	return found;	
}

template<>
bool	AsciiParserUtilsTemplate<char>::ReadDouble(double& result)
{
	// search int start
	bool	found=false;
	char *p = ((char*)myText) + myCurrentReadPos;
	while(!found)
	{
		if (((*p) == (char)' ') || ((*p) < (char)0x13)) // separators or special chars
		{
			++p;
			if(p>=myTextEnd)
			{
				// not found
				break;
			}
		}
		else 
		{
			
			int readlen;
			int test=sscanf(p,"%lf%n",&result,&readlen);

			if(test)
			{
				
				myCurrentReadPos = (p - (char*)myText);
				found=true;
				myCurrentReadPos+=readlen;
			}
			break;
		}
	}
	return found;	
}

template<>
bool	AsciiParserUtilsTemplate<char>::ReadString(char* result, const int MAX_STRING_L)
{
	// search int start
	bool	found=false;
	char *p = ((char*)myText) + myCurrentReadPos;
	while(!found)
	{
		if (((*p) == (char)' ') || ((*p) < (char)0x13)) // separators or special chars
		{
			++p;
			if(p>=myTextEnd)
			{
				// not found
				break;
			}
		}
		else // the word to find is in recognition
		{
			// test if we have to add a 0 to "cut" the string before the end for optimisation
			// as sscanf check string validity and search for a trailing 0 each time
			
			int test=sscanf(p,"%s",result);
			
			if(test)
			{
				myCurrentReadPos = (p - (char*)myText);
				found=true;
				myCurrentReadPos+=strlen(result);
			}
			break;
		}
	}
	return found;	

}

template<typename charType>
void	AsciiParserUtilsTemplate<charType>::copyUSToCharBuffer(unsigned char* tobuffer, unsigned short* frombuffer, int sizeMax)
{
	unsigned char* writechar = tobuffer;
	unsigned short* readUS = frombuffer;

	int currentSize = 0;
	while ((*readUS != 0) && (currentSize<(sizeMax-1)))
	{
		*writechar = (unsigned char)(*readUS);
		++writechar;
		++readUS;
		++currentSize;
	}
	*writechar = 0;
}

template<>
bool	AsciiParserUtilsTemplate<unsigned short>::ReadInt(int& result)
{
	// search int start
	bool	found = false;
	unsigned short *p = ((unsigned short*)myText) + myCurrentReadPos;
	while (!found)
	{
		if (((*p) == (unsigned short)' ') || ((*p) < (unsigned short)0x13)) // separators or special chars
		{
			++p;
			if (p >= myTextEnd)
			{
				// not found
				break;
			}
		}
		else // the word to find is in recognition
		{
			int readlen;

			unsigned char	tmpBuffer[64];

			copyUSToCharBuffer(tmpBuffer, p, 64);

			int test = sscanf((const char*)tmpBuffer, "%d%n", &result, &readlen);

			if (test)
			{
				myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(unsigned short);
				found = true;
				myCurrentReadPos += readlen;
			}

			break;
		}
	}
	return found;
}

template<>
bool    AsciiParserUtilsTemplate<unsigned short>::ReadChar(unsigned short& result)
{
	bool	found = false;
	unsigned short *p = ((unsigned short*)myText) + myCurrentReadPos;
	if (p < myTextEnd)
	{
		result = *p;
		myCurrentReadPos++;
		found = true;
	}

	return found;
}

template<>
bool	AsciiParserUtilsTemplate<unsigned short>::ReadFloat(float& result)
{
	// search int start
	bool	found = false;
	unsigned short *p = ((unsigned short*)myText) + myCurrentReadPos;
	while (!found)
	{
		if (((*p) == (unsigned short)' ') || ((*p) < (unsigned short)0x13)) // separators or special chars
		{
			++p;
			if (p >= myTextEnd)
			{
				// not found
				break;
			}
		}
		else
		{

			int readlen;

			unsigned char	tmpBuffer[64];

			copyUSToCharBuffer(tmpBuffer, p, 64);

			int test = sscanf((const char*)tmpBuffer, "%f%n", &result, &readlen);

			if (test)
			{

				myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(unsigned short);
				found = true;
				myCurrentReadPos += readlen;
			}
			break;
		}
	}
	return found;
}

template<>
bool	AsciiParserUtilsTemplate<unsigned short>::ReadDouble(double& result)
{
	// search int start
	bool	found = false;
	unsigned short *p = ((unsigned short*)myText) + myCurrentReadPos;
	while (!found)
	{
		if (((*p) == (unsigned short)' ') || ((*p) < (unsigned short)0x13)) // separators or special chars
		{
			++p;
			if (p >= myTextEnd)
			{
				// not found
				break;
			}
		}
		else
		{

			int readlen;
			unsigned char	tmpBuffer[64];

			copyUSToCharBuffer(tmpBuffer, p, 64);

			int test = sscanf((const char*)tmpBuffer, "%lf%n", &result, &readlen);

			if (test)
			{

				myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(unsigned short);
				found = true;
				myCurrentReadPos += readlen;
			}
			break;
		}
	}
	return found;
}

template<>
bool	AsciiParserUtilsTemplate<unsigned short>::ReadString(unsigned short* result, const int MAX_STRING_L)
{
	// search int start
	bool	found = false;
	unsigned short *p = ((unsigned short*)myText) + myCurrentReadPos;
	while (!found)
	{
		if (((*p) == (unsigned short)' ') || ((*p) < (unsigned short)0x13)) // separators or special chars
		{
			++p;
			if (p >= myTextEnd)
			{
				// not found
				break;
			}
		}
		else // the word to find is in recognition
		{
			// not optimised 
			// TODO : check if OK.

			usString	tmpString(p);

			kstl::string tmpstdstring = tmpString.ToString();

			int test = sscanf(tmpstdstring.c_str(), "%s", (char*)result); // copy to result a tmp buffer

			if (test)
			{
				memcpy(result, p, tmpString.strlen()*sizeof(unsigned short));

				myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(unsigned short);
				found = true;
				myCurrentReadPos += tmpString.strlen();
			}

			break;
		}
	}
	return found;

}

template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GetBlock(AsciiParserUtilsTemplate<charType>& result, charType	blkStart, charType	blkEnd)
{
	bool	found=false;
	charType *p = ((charType*)myText) + myCurrentReadPos;
	int openingfound=-1;
	charType *pBlockStart = 0, *pBlockEnd = 0;
	while(!found)
	{
		// search block start / end
		while(*p!=blkStart && *p!=blkEnd && p<myTextEnd)
			p++;

		if(*p == blkStart)
		{
			p++;
			if(pBlockStart == 0)
			{
				pBlockStart=p;
				openingfound=1;
			}
			else
			{
				openingfound++;
			}
		}

		if(*p == blkEnd)
		{
			p++;
			if(pBlockStart)
			{
				openingfound--;
			}
		}

		if(openingfound == 0)
		{
			pBlockEnd=p;
			pBlockEnd-=1;
			int lsize = ((int)((unsigned char*)pBlockEnd - (unsigned char*)pBlockStart)) / sizeof(charType);
			if(lsize>=0)
			{
				result.Set(pBlockStart, lsize);
				found=true;
			}
		}

		myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);
			
		if (p>=myTextEnd)
		{
			myCurrentReadPos=myTextLen;
			break;
		}
	}
	return found;
}

// retreive block but don't search in parts between quotes
template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GetBlockExcludeString(AsciiParserUtilsTemplate<charType>& result, charType	blkStart, charType	blkEnd)
{
	bool	found = false;
	charType *p = ((charType*)myText) + myCurrentReadPos;
	int openingfound = -1;
	charType *pBlockStart = 0, *pBlockEnd = 0;

	bool inQuote = false;
	charType  prevp = 0;
	while (!found)
	{
		bool EscapeChar = false;
		// search block start / end (not in quote)
		while (((*p != blkStart && *p != blkEnd) || inQuote) && p < myTextEnd)
		{
			// jmp escape char in quote
			if ((inQuote) && (prevp == '\\'))
			{
				EscapeChar = !EscapeChar;
			}
			else
			{
				EscapeChar = false;
			}
			if ((*p == '"') && (EscapeChar == false))
			{
				inQuote = !inQuote;
			}
			prevp = *p;
			p++;
		}

		// blkStart found
		if (*p == blkStart)
		{
			prevp = *p;
			p++;

			// first one ?
			if (pBlockStart == 0)
			{
				pBlockStart = p;
				openingfound = 1;
			}
			else // only increment opening
			{
				openingfound++;
			}
		}

		// blkEnd found
		if (*p == blkEnd)
		{
			prevp = *p;
			p++;
			if (pBlockStart)
			{
				openingfound--;
				// this is the one corresponding to pBlockStart, search can stop here
				if (openingfound == 0)
				{
					pBlockEnd = p;
					pBlockEnd -= 1;
					int lsize = ((int)((unsigned char*)pBlockEnd - (unsigned char*)pBlockStart)) / sizeof(charType);
					if (lsize >= 0)
					{
						result.Set(pBlockStart, lsize);
						found = true;
					}
				}
			}
		}

		if (p >= myTextEnd)
		{
			myCurrentReadPos = myTextLen;
			break;
		}
	}

	myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);

	return found;
}

template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GoAfterNextSeparator(charType	separator)
{
	charType *p = ((charType*)myText) + myCurrentReadPos;

	while((*p!=separator) && p<myTextEnd)
		p++;

	if(*p == separator)
	{
		p++;
		myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);
		return true;
	}

	return false;
}

template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GoToNextNonWhitespace()
{
	charType *p = ((charType*)myText) + myCurrentReadPos;

	while ((*p <= (charType)' ') && p<myTextEnd)
		p++;

	if(p < myTextEnd)
	{
		myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);
		return true;
	}

	return false;

}

// search for given char starting at startpos and return found pos or -1 if not found
template<typename charType>
unsigned int		AsciiParserUtilsTemplate<charType>::FindNext(charType toFind, unsigned int startpos)
{
	unsigned int currentPos = startpos;
	charType *p = ((charType*)myText) + currentPos;

	while ((*p != (charType)toFind) && p < myTextEnd)
	{
		currentPos++;
		p++;
	}
	if (p < myTextEnd)
	{
		return currentPos;
	}
	return (unsigned int)-1;
}

// search for given word starting at startpos and return found pos or -1 if not found
template<typename charType>
unsigned int		AsciiParserUtilsTemplate<charType>::FindNext(const charType* toFind, unsigned int startpos)
{
	unsigned int currentPos = startpos;
	charType *p = ((charType*)myText) + currentPos;

	// search toFind Len
	unsigned int toFindLen = 0;
	while (toFind[toFindLen] != 0)
	{
		toFindLen++;
	}

	while (true)
	{
		while ((*p != (charType)toFind[0]) && p < myTextEnd)
		{
			currentPos++;
			p++;
		}
		if (p < myTextEnd)
		{
			bool wasFound = true;
			// check if word toFind is here
			for (unsigned int i = 0; i < toFindLen; i++)
			{
				if (p[i] != toFind[0])
				{
					wasFound = false;
					break;
				}
			}

			if (wasFound)
			{
				return currentPos;
			}
			p += toFindLen;
			currentPos += toFindLen;
		}
		else
		{
			break;
		}
	}
	return (unsigned int)-1;
}

template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GoToNextValid(const ValidCharArray<charType>&	validCharArray)
{
	charType *p = ((charType*)myText) + myCurrentReadPos;


	while ((!validCharArray.isValid(*p)) && p<myTextEnd)
		p++;

	if (p < myTextEnd)
	{
		myCurrentReadPos = ((int)((unsigned char*)p - (unsigned char*)myText)) / sizeof(charType);
		return true;
	}

	return false;
}

// get part before CurrentReadPos
template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GetLeadingPart(AsciiParserUtilsTemplate<charType>& result, bool	removeLeadTrailWhitespace)
{
	if(myCurrentReadPos)
	{
		if(removeLeadTrailWhitespace)
		{
			bool	found=false;
			charType *p = ((charType*)myText);
			while(!found)
			{
				// remove previous separators
				while ((*p <= (charType)' ') && p<myTextEnd)
					p++;

				charType *pWordStart = p;

				// search word end
				while (*p >= (charType)' ' && p<myTextEnd)
					p++;

				charType *pWordEnd = p;
				
				while ((*(pWordEnd - 1)) <= (charType)' ' && pWordEnd>pWordStart)
				{
					pWordEnd--;
				}
				
				int lsize = ((int)((unsigned char*)pWordEnd - (unsigned char*)pWordStart)) / sizeof(charType);
				if(lsize>0)
				{
					result.Set(pWordStart, lsize);
					found=true;
				}
				break;
			}
			return found;
		}
		else
		{
			result.Set(myText,myCurrentReadPos);
			return true;
		}
	}
	return false;
}
	
template<typename charType>
bool	AsciiParserUtilsTemplate<charType>::GetTrailingPart(AsciiParserUtilsTemplate<charType>& result, bool	removeLeadTrailWhitespace)
{
	if(myCurrentReadPos<myTextLen)
	{
		if(removeLeadTrailWhitespace)
		{
			bool	found=false;
			charType *p = ((charType*)myText) + myCurrentReadPos;
			while(!found)
			{
				// remove previous separators
				while ((*p <= (charType)' ') && p<myTextEnd)
					p++;

				charType *pWordStart = p;

				p=myTextEnd;
				charType *pWordEnd = myTextEnd;
				
				while ((*(pWordEnd - 1)) <= (charType)' ' && pWordEnd>pWordStart)
				{
					pWordEnd--;
				}
				
				int lsize = ((int)((unsigned char*)pWordEnd - (unsigned char*)pWordStart)) / sizeof(charType);
				if(lsize>0)
				{
					result.Set(pWordStart, lsize);
					found=true;
				}
			
				break;
				
			}
			return found;
		}
		else
		{
			charType *p = ((charType*)myText) + myCurrentReadPos;
			result.Set(p,myTextLen-myCurrentReadPos);
			return true;
		}
	}
	return false;
}

// convert raw unsigned char buffer memory to string (hexadecimal)
template<typename charType>
kstl::string	AsciiParserUtilsTemplate<charType>::BufferToString(unsigned char* zone, unsigned int size)
{
	// each unsigned char is coded in two bytes in hexadecimal notation
	kstl::string result;
	result="";
	unsigned char* readzone=zone;
	unsigned int i;
	for(i=0;i<size;i++)
	{
		char	a1,a2;
		a1='a'+((*readzone&0xF0)>>4);
		a2='a'+((*readzone&0xF));
		result+=a1;
		result+=a2;
		++readzone;
	}
	return result;
}

// convert string (hexadecimal) to raw unsigned char buffer (to delete with delete[] buffer)
template<typename charType>
unsigned char*	AsciiParserUtilsTemplate<charType>::StringToBuffer(const kstl::string& str, unsigned int& size)
{
	// byte size is half string size
	size=str.size()/2;
	unsigned char* result=0;
	unsigned char*	readzone=(unsigned char*)str.c_str();	
	if(size)
	{
		result=new unsigned char[size];
		unsigned char* writezone=result;
		unsigned int i;
		for(i=0;i<size;i++)
		{
			unsigned char byteresult=((readzone[0]-'a')<<4)| ((readzone[1]-'a'));
			
			*writezone=byteresult;

			readzone+=2;
			++writezone;
		}
	}
	
	return result;
}

template<>
AsciiParserUtilsTemplate<char>::operator kstl::string() const
{
	kstl::string	tmp;
	tmp.assign(myText, myTextLen);
	return tmp;
}

template<>
AsciiParserUtilsTemplate<unsigned short>::operator kstl::string() const
{
	kstl::string	tmp;
	
	usString	tmpUSString(myText);

	tmp = tmpUSString.ToString();

	return tmp;
}

template<>
AsciiParserUtilsTemplate<char>::operator usString() const
{
	usString	tmp(myText);
	return tmp;
}

template<>
AsciiParserUtilsTemplate<unsigned short>::operator usString() const 
{
	usString	tmp(myText);
	return tmp;
}


template<>
const char * AsciiParserUtilsTemplate<char>::c_str()
{
	return (const char *)myText;
}

template<>
const char * AsciiParserUtilsTemplate<unsigned short>::c_str()
{
	// error
	return 0;
}

template<>
const unsigned short* AsciiParserUtilsTemplate<char>::us_str() const 
{
	// error
	return 0;
}


template<>
const unsigned short* AsciiParserUtilsTemplate<unsigned short>::us_str() const
{
	return (const unsigned short*)myText;
}

template<>
kstl::string	AsciiParserUtilsTemplate<char>::subString(int startpos, int len)
{
	kstl::string	tmp;
	char* p = (char*)myText + startpos;

	tmp.assign(p, len);
	return tmp;
}

template<>
kstl::string	AsciiParserUtilsTemplate<unsigned short>::subString(int startpos, int len)
{
	kstl::string	tmp;
	unsigned short* p = (unsigned short*)myText + startpos;

	usString	tmpUSString(p);

	tmp = tmpUSString.ToString();

	tmp = tmp.substr(0, len);
	
	return tmp;
}

template class AsciiParserUtilsTemplate<char>;
template class AsciiParserUtilsTemplate<unsigned short>;