#include "PrecompiledHeaders.h"
#include "FreeType_TextDrawer.h"

const unsigned short	wSpace=(unsigned short)(' ');
const unsigned short* WhiteSpace=&wSpace;

const char TAG_NAME_A[] = "name";

struct LastValidSplit
{
	int charPos;
	int nextLinePos;
	int lineWidth;
};

#define MAX_FONT_CACHE_SIZE		2
#define PIXEL_WIDTH				4
#define BORDER_PIXEL_MARGIN		4
#define BORDER_PIXEL_MARGIN_64	256

unsigned int FreeType_TextDrawer::mUseCount  = 0;
unsigned int FreeType_TextDrawer::mInterline = 0;

FreeType_TextDrawer::FreeType_TextDrawer() :
mLibraryContext(0)
{
	if(0 != FT_Init_FreeType(&mLibraryContext)) mLibraryContext = 0;
	initFontFaceCache();	
}

FreeType_TextDrawer::~FreeType_TextDrawer()
{
	freeFontFaceCache();
	if(mLibraryContext != 0) FT_Done_FreeType(mLibraryContext);
}	

unsigned char* FreeType_TextDrawer::DrawTextWithInterlineToImage(const char*  text, unsigned int il, unsigned int tl, int& sizex, int& sizey, TextAlignment alignment, bool onlySize, unsigned int a_maxLineNumber, int a_maxSize, int a_drawingLimit)
{
	unsigned short* tmp = new unsigned short[tl + 1];
	for (unsigned int i = 0; i<tl; i++)
		tmp[i] = text[i];

	tmp[tl] = 0;

	unsigned char* ret = DrawTextWithInterlineToImage(tmp, il, tl, sizex, sizey, alignment, onlySize, a_maxLineNumber, a_maxSize, a_drawingLimit);
	delete[] tmp;
	return ret;

}

unsigned char* FreeType_TextDrawer::DrawTextWithInterlineToImage(const unsigned short* text, unsigned int il, unsigned int tl, int& sizex, int& sizey, TextAlignment alignment, bool onlySize, unsigned int a_maxLineNumber, int a_maxSize, int a_drawingLimit)
{
	mInterline = il;
	unsigned char* ret = DrawTextToImage(text, tl, sizex, sizey, alignment, onlySize, a_maxLineNumber, a_maxSize, a_drawingLimit);
	mInterline = 0;
	return ret;

}


unsigned char* FreeType_TextDrawer::DrawTextToImage(const char* text, unsigned int tl, int& sizex,int& sizey, TextAlignment alignment, bool onlySize,unsigned int a_maxLineNumber,int a_maxSize, int a_drawingLimit, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned short* tmp = new unsigned short[tl+1];
	for(unsigned int i=0;i<tl;i++)
		tmp[i] = text[i];

	tmp[tl]=0;

	unsigned char* ret = DrawTextToImage(tmp, tl, sizex,sizey, alignment, onlySize,a_maxLineNumber,a_maxSize, a_drawingLimit,r,g,b);
	delete[] tmp;
	return ret;
}

unsigned char* FreeType_TextDrawer::DrawTextToImage(const unsigned short* text, unsigned int tl, int& sizex,int& sizey, TextAlignment alignment, bool onlySize,unsigned int a_maxLineNumber,int a_maxSize, int a_drawingLimit, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned short* txt=(unsigned short*)text;

	if((tl == 0) || (text == 0))
	{
		txt=(unsigned short*)WhiteSpace;
		tl=1;
	}

	unsigned int foundw = 0;
	unsigned int foundh = 0;
	unsigned int baseline = 0;
	bool L_fixedSize = false;
	if(sizex)
		L_fixedSize = true;
	else if(a_maxSize)
		sizex = a_maxSize;

	std::vector<LineStruct> result=splitString(txt,tl,sizey,sizex,foundw,foundh,baseline,a_maxLineNumber);
	if (foundh == 0 || foundw == 0)
		return nullptr;


	if(L_fixedSize && ((int)foundw<sizex) )
	{
		foundw=(unsigned int)sizex;
	}

	FT_Error error = 0;
	FT_GlyphSlot slot = mFontFaceCache[mCurrentFontFace]->mFontFace->glyph;
	//int addHeight = 0;

	// estimate bitmap dimensions

	unsigned short*	  readchar=(unsigned short*)txt;

	if(foundh<2)
	{
		foundh=2;
	}

	if(foundw<2)
	{
		foundw=2;
	}

	int fontHeight=(mFontFaceCache[mCurrentFontFace]->mFontFace->size->metrics.height)>>6;
	if (fontHeight < 2)fontHeight = 2;

	unsigned int lineHeight=(foundh-2*BORDER_PIXEL_MARGIN)/(unsigned int)result.size();

	// center text around baseline - fontHeight/4

	int up=baseline-fontHeight/4;
	if(up<0)
	{
		up=0;
	}
	int bottom=lineHeight-up;

	if(up>bottom)
	{
		bottom=up-bottom;
		foundh+=bottom;
		up=0;
	}
	else
	{
		up=bottom-up;
		foundh+=up;
		bottom=0;
	}

	sizex = (int)foundw;
	sizey = (int)foundh;

	if(onlySize)
		return 0;

	unsigned int L_ImageSize = foundw * foundh * PIXEL_WIDTH;
	unsigned char* pixelBuffer = new unsigned char[L_ImageSize];
	memset(pixelBuffer,0,L_ImageSize);

	int buffRow = 0;
	int buffCol = 0;

	readchar=txt;

	// render each glyph to bitmap

	int buffRowStart = up+baseline+BORDER_PIXEL_MARGIN;

	std::vector<LineStruct>::iterator		itLine=result.begin();
	int L_Index = 0;	
	int deltaJustified;
	int line = 0;

	//printf("\n\n");
	//std::string buf;
	//char tmp[1024];
	while(itLine != result.end())
	{
		//buf = "";
		line++;
		deltaJustified = 0;
		LineStruct& currentLine=(*itLine);

		if(TextAlignmentCenter == alignment)
		{
			// center
			buffCol = (foundw-currentLine.len)/2;
		}
		else if(TextAlignmentLeft == alignment)
		{
			// left
			buffCol = BORDER_PIXEL_MARGIN;
		}
		else if (TextAlignmentRight == alignment)
		{
			// right
			buffCol = foundw-currentLine.len-BORDER_PIXEL_MARGIN;
		}
		else if (TextAlignmentJustified == alignment)
		{
			buffCol = BORDER_PIXEL_MARGIN;
			// search how many space in line
			
			if (!currentLine.hasLineFeed)
			{
				int countSpace = 0;
				for (unsigned int n = currentLine.startIndex; n <= currentLine.endIndex; n++)
				{
					readchar = &txt[n];
					if (*readchar == 32 || *readchar == 160)
					{
						countSpace++;
					}
				}
				if (countSpace)
				{
					deltaJustified = foundw - currentLine.len - 2 * BORDER_PIXEL_MARGIN;
					deltaJustified <<= 6;
					deltaJustified /= countSpace;
				}
				//sprintf(tmp, "\tspace %2d >> %5d", countSpace, deltaJustified);
				//buf.append(tmp);
			}
		}
		else
		{
			printf("bad alignment");
			buffCol = BORDER_PIXEL_MARGIN;
		}
		
		int L_w64 = buffCol<<6;

		//sprintf(tmp, "\t%d >> ", L_w64);
		//buf.append(tmp);
		//int old_w64 = L_w64;

		FT_UInt       previous=0;

		for (unsigned int n = currentLine.startIndex; n <= currentLine.endIndex; n++ )
		{
			readchar = &txt[n];
			if(*readchar == 10)
			{
			//	++readchar;
				previous = 0;
				continue;
			}
			error = FT_Load_Char(mFontFaceCache[mCurrentFontFace]->mFontFace, *readchar, FT_LOAD_RENDER);
			if (error || (*readchar == 13))
			{
			
				continue;
			}

			FT_Bitmap& glyphBmp = slot->bitmap;
			FT_Glyph_Metrics& check_metrics=slot->metrics;


			int w=check_metrics.width;

			int Bearing = 0;

			if (!previous)
			{
				Bearing = check_metrics.horiBearingX;
			}			

			buffCol = ((L_w64 + check_metrics.horiBearingX - Bearing) >> 6);

			L_w64 += check_metrics.horiAdvance - Bearing;
			
			if (*readchar == 32 || *readchar == 160)
			{
				L_w64 += deltaJustified;
			}

			//sprintf(tmp, "%c:%+4d %d ", reinterpret_cast<char*>(readchar)[0], L_w64 - old_w64, L_w64);
			//buf.append(tmp);
			//old_w64 = L_w64;

			w=w>>6;

			unsigned int h=check_metrics.height>>6;
			unsigned int topb=check_metrics.horiBearingY>>6;

			if(w > 0)
			{
				unsigned int rowcount=h;
				buffRow = buffRowStart - topb;
				buffRow = (buffRow < 0)? 0 : buffRow;

				unsigned char* readpix=glyphBmp.buffer;
				unsigned char* writepix = pixelBuffer + (buffRow * foundw + buffCol )* PIXEL_WIDTH;

				if(a_drawingLimit == -1 || L_Index < a_drawingLimit)
				{
					for(unsigned int row = 0; row < rowcount; row++)
					{
						for(int col = 0; col < glyphBmp.width; col++)
						{			
							*writepix++ = r;
							*writepix++ = g;
							*writepix++ = b;

							*writepix++ |=*readpix++;
						}

						writepix+=(foundw-glyphBmp.width)*PIXEL_WIDTH;
					}
				}
					
			}

			previous = *readchar;

			buffCol = (L_w64>>6);
			L_Index++;
			
		}

		itLine++;
		buffRowStart+=lineHeight;

		//printf("%02d line size = %8d (%4d)   %s\n", line, L_w64,(int)(L_w64>>6), buf.c_str());
	}
	return pixelBuffer;
}

std::vector<FreeType_TextDrawer::LineStruct> FreeType_TextDrawer::splitString(const unsigned short* text,unsigned int tl,unsigned int maxHeight,unsigned int maxWidth,unsigned int& foundw,unsigned int& foundh,unsigned int& baseline,unsigned int a_maxLineNumber)
{
	//std::string debug = "";
	std::vector<FreeType_TextDrawer::LineStruct> returned;

	// convert to 64 point / pixel
	maxWidth  = maxWidth  <<6;
	maxHeight = maxHeight <<6;

	if (maxWidth != 0)
	{
		if (maxWidth < 2 * BORDER_PIXEL_MARGIN_64)
		{
			foundw = 0;
			return returned;
		}


		maxWidth -= 2 * BORDER_PIXEL_MARGIN_64;
	}

	LineStruct currentLine;
	currentLine.hasLineFeed = false;
	currentLine.len = 0;
	currentLine.startIndex = 0;
	currentLine.endIndex = 0;

	std::map<unsigned short, FT_Glyph_Metrics>	 metrics; 
	FT_Glyph_Metrics * check_metrics = NULL;

	FT_GlyphSlot slot = mFontFaceCache[mCurrentFontFace]->mFontFace->glyph;
	int addHeight = 0;

	// estimate bitmap dimensions
	int topb                = 0;	
	int computedH           = 0;
	int lineWidth			= 0;

	/*unsigned int lastValidSplit = 0;
	unsigned int lastLineStart  = 0;
	unsigned int lastLineLen    = 0;*/
	unsigned int currentW       = 0; 

	LastValidSplit lastSplit;
	lastSplit.charPos             = -1;
	lastSplit.nextLinePos         = -1;
	lastSplit.lineWidth           = 0;
	
	int previousEndBearing = 0;
	int endBearing = 0;

	bool canSplit = false;
	bool doSplit  = false; 

	unsigned short readchar;
	unsigned int n = 0;
	while (n < tl)
	{
		readchar = text[n];
		if (readchar == 10)// line feed (0x0A) 
		{
			doSplit  = true;
			canSplit = false;
			currentW=0;
			currentLine.hasLineFeed = true;

			lastSplit.charPos = n - 1;
			lastSplit.nextLinePos = n+1;
			lastSplit.lineWidth = lineWidth + previousEndBearing;
		}
		else
		{
			if (readchar == 13) //carriage return (0x0D) catch and zap
			{
				n++;
				continue;
			}


			if (metrics.find(readchar) != metrics.end())
				check_metrics = &metrics[readchar];
			else
			{
				if (FT_Load_Char(mFontFaceCache[mCurrentFontFace]->mFontFace, readchar, FT_LOAD_NO_BITMAP))
				{
					// char error (should never be there)
					continue;
				}
				metrics[readchar] = slot->metrics;
				check_metrics = &metrics[readchar];
			}

			if (readchar == 32) // conserve last valid split on space (0x20) 
			{
				if (canSplit)
				{
					lastSplit.charPos = n - 1;
					lastSplit.lineWidth = lineWidth + previousEndBearing;
				}

				lastSplit.nextLinePos = n + 1;
				canSplit = false;
			}
			else
			{
				canSplit = true;
			}

			topb = check_metrics->height - check_metrics->horiBearingY;

			endBearing = check_metrics->width - check_metrics->horiAdvance + check_metrics->horiBearingX;

			currentW = check_metrics->horiAdvance;

			computedH = computedH < check_metrics->horiBearingY ? check_metrics->horiBearingY : computedH;
			addHeight = addHeight < topb ? topb : addHeight;
		}

		// check if the 
		if (!doSplit && maxWidth)
		{
			if ((lineWidth + currentW + endBearing) >= maxWidth) // to wide, split the string
			{

				if (lastSplit.charPos != -1) // use the last valid split
				{

				}
				else // no last valid split
				{
					lastSplit.charPos = n - 1;
					lastSplit.lineWidth = lineWidth + previousEndBearing;
					lastSplit.nextLinePos = n;
				}

				doSplit = true;
			}
		}

		if (!doSplit)
		{
			if (lineWidth == 0) // keep first used bearing
				lineWidth -= check_metrics->horiBearingX;
			lineWidth += check_metrics->horiAdvance;
		}

		//debug.push_back((char)readchar);
		//printf("%c:%+5d  :  (%d / %d) '%s'\n", readchar, check_metrics->horiAdvance, lineWidth, maxWidth, debug.c_str());

		if (doSplit) // do the split
		{
			if (lastSplit.charPos > 0)
			{
				currentLine.endIndex = lastSplit.charPos;
			}
			else
			{
				if (currentLine.startIndex == currentLine.endIndex)
				{
					foundw = 0; 
					returned.clear();
					return returned;
				}
				currentLine.endIndex = 0;
			}
			currentLine.len = lastSplit.lineWidth >> 6;

			if ((a_maxLineNumber > 0) && (returned.size() >= a_maxLineNumber))
				break;

			//printf("split at '%s' pushline : '%s'\n", debug.c_str(), debug.substr(0, currentLine.endIndex - currentLine.startIndex+1).c_str());
			//debug = "";
			
			if (currentLine.startIndex >= currentLine.endIndex) //empty line
			{
				currentLine.startIndex = currentLine.endIndex;
				currentLine.len = 0;
				
			}
			returned.push_back(currentLine);
			currentLine.hasLineFeed = false;
			currentLine.startIndex = lastSplit.nextLinePos;
			n = currentLine.startIndex;

			lineWidth = 0;

			lastSplit.charPos = -1;
			lastSplit.nextLinePos = -1;
			lastSplit.lineWidth = 0;

			doSplit = false;
			continue;
		}

		// calcul end bearing
		previousEndBearing = endBearing;

		n++;
	}

	addHeight=(addHeight+63);
	addHeight=addHeight>>6;

	
	// add current line
	if (lineWidth)
	{
		currentLine.endIndex = tl - 1;
		currentLine.len = lineWidth >> 6;
	}	


	if (!((a_maxLineNumber > 0) && (returned.size() >= a_maxLineNumber)))
	{
		returned.push_back(currentLine);
	}

	// compute final foundw

	for (auto& l : returned)
	{
		if (l.len > foundw)
		{
			foundw = l.len;
		}
	}

	foundw += BORDER_PIXEL_MARGIN * 2;

	// multiline ?
	foundh =((computedH+63) >> 6);
	baseline = (computedH+63) >> 6;
	foundh += (addHeight);
	foundh*=(unsigned int)returned.size();

	foundh += mInterline*(unsigned int)(returned.size()-1);


	foundh+=BORDER_PIXEL_MARGIN*2;

	return returned;
}

void FreeType_TextDrawer::manageLineEnd(FreeType_TextDrawer::LineStruct& currentLine, const unsigned short* text, FreeType_TextDrawer::CharacterData* chardata)
{
	if (currentLine.endIndex - currentLine.startIndex)
	{

		// remove unwanted
		while ((chardata[currentLine.endIndex].advance == 0) && (currentLine.endIndex > currentLine.startIndex))
		{
			currentLine.endIndex--;
		}

		if (text[currentLine.endIndex] == 32)
		{
			currentLine.len -= chardata[currentLine.endIndex].advance;
			currentLine.endIndex--;
		}
		// reserve needed pixel for rendering
		if (currentLine.endIndex - currentLine.startIndex)
		{
			if (text[currentLine.startIndex] == 32)
			{
				currentLine.len -= chardata[currentLine.startIndex].advance;
				currentLine.startIndex++;
			}
		}
	}
	currentLine.len += -chardata[currentLine.startIndex].bearing;

	currentLine.len += chardata[currentLine.endIndex].width - (chardata[currentLine.endIndex].advance - chardata[currentLine.endIndex].bearing);
}


void FreeType_TextDrawer::Log_FT_Error(FT_Error error)
{
	switch(error)
	{
		case FT_Err_Unknown_File_Format:
		{
			printf("Unknown_File_Format error\n");
			break;
		}
		case FT_Err_Cannot_Open_Resource:
		{
			printf("Cannot_Open_Resource error\n");
			break;
		}
		case FT_Err_Invalid_Version:
		{
			printf("Invalid_Version error\n");
			break;
		}
		case FT_Err_Invalid_File_Format:
		{
			printf("Invalid_File_Format error\n");
			break;
		}
		case FT_Err_Lower_Module_Version:
		{
			printf("Lower_Module_Version error\n");
			break;
		}
		case FT_Err_Invalid_Argument:
		{
			printf("Invalid_Argument error\n");
			break;
		}
		case FT_Err_Unimplemented_Feature:
		{
			printf("Unimplemented_Feature error\n");
			break;
		}
		case FT_Err_Invalid_Table:
		{
			printf("Invalid_Table error\n");
			break;
		}
		case FT_Err_Invalid_Offset:
		{
			printf("Invalid_Offset error\n");
			break;
		}
		case FT_Err_Array_Too_Large:
		{
			printf("Array_Too_Large error\n");
			break;
		}
		case FT_Err_Missing_Module:
		{
			printf("Missing_Module error\n");
			break;
		}

		//Handle
		case FT_Err_Invalid_Handle:
		{
			printf("Handle error\n");
			break;
		}
		case FT_Err_Invalid_Library_Handle:
		{
			printf("Handle error\n");
			break;
		}
		case FT_Err_Invalid_Driver_Handle:
		{
			printf("Handle error\n");
			break;
		}
		case FT_Err_Invalid_Face_Handle:
		{
			printf("Handle error\n");
			break;
		}
		case FT_Err_Invalid_Size_Handle:
		{
			printf("Handle error\n");
			break;
		}
		case FT_Err_Invalid_Slot_Handle:
		{
			printf("Handle error\n");
			break;
		}
		case FT_Err_Invalid_CharMap_Handle:
		{
			printf("Handle error\n");
			break;
		}
		case FT_Err_Invalid_Cache_Handle:
		{
			printf("Handle error\n");
			break;
		}
		case FT_Err_Invalid_Stream_Handle:
		{
			printf("Handle error\n");
			break;
		}

		//Memory
		case FT_Err_Out_Of_Memory:
		{
			printf("Memory error\n");
			break;
		}
		case FT_Err_Unlisted_Object:
		{
			printf("Memory error\n");
			break;
		}

		//Cache
		case FT_Err_Too_Many_Caches:
		{
			printf("Cache error\n");
			break;
		}
	}
}

void	FreeType_TextDrawer::startBuildFonts()
{
	initFontFaceCache();
}
/*
bool FreeType_TextDrawer::NeedChangeFont(const char* fontName, unsigned int nSize)
{
	return (fontName != m_FontFaceStruct.mFontName || nSize != m_FontFaceStruct.mFontSize);
}

bool FreeType_TextDrawer::NeedChangeFontBuffer(const char* fontName)
{
	return (fontName != m_FontFaceStruct.mFontName);
}*/

bool FreeType_TextDrawer::SetFont(const char* fontName , const char* fullPath, unsigned int nSize)
{
	bool bRetVal = true;
	FT_Error error = 0;
	mUseCount++;

	FontFaceStruct* current=getFontStruct(fontName);
	if(current)
	{
		if(nSize > 0 && nSize != current->mFontSize)
		{
			current->mFontSize = nSize;
		}
		FT_Set_Char_Size(current->mFontFace, 0, current->mFontSize * 64, 96, 0);
	
		current->mUsedCount=mUseCount;
		return true;
	}

	current=searchOlderFontFaceInCache();
	current->mUsedCount=mUseCount;
	current->mFontName = fontName;
	if(nSize > 0 && nSize != current->mFontSize)
	{
		current->mFontSize = nSize;
	}

	error = FT_New_Face( mLibraryContext, fullPath, 0, &current->mFontFace );
	
	// set default font
	if(error)
	{
		return false;
	}

	if(!error)
	{
		error = FT_Set_Char_Size(current->mFontFace, 0, current->mFontSize * 64, 96, 0);
	}
	else
	{
		bRetVal = false;
	}

	return bRetVal;
}

bool FreeType_TextDrawer::SetFont(const char* fontName, unsigned char * data, unsigned int fileSize, unsigned int nSize)
{
	FT_Error error = 0;
	if(nSize <= 0)
		return false;

	mUseCount++;

	FontFaceStruct* current=getFontStruct(fontName);
	if(current)
	{
		if(current->mFontSize != nSize)
		{
			current->mFontSize = nSize;
			FT_Set_Char_Size(current->mFontFace, 0, current->mFontSize * 64, 96, 0);
		}
		current->mUsedCount=mUseCount;
		return true;
	}

	current=searchOlderFontFaceInCache();
	current->mUsedCount=mUseCount;
	current->mFontName = fontName;
	current->mFontSize = nSize;

	// create new fontFace
	FT_New_Memory_Face( mLibraryContext, data, fileSize, 0, &current->mFontFace );
	if(error)
	{
		current->release();
		if(data)
			delete [] data;
		data = 0;
		return false;
	}

	// keep ref on buffer
	current->mFaceBuffer = data;

	error = FT_Set_Char_Size(current->mFontFace, 0, current->mFontSize * 64, 96, 0);

	return true;
}

std::string FreeType_TextDrawer::GetFontNameFromFile(unsigned char* data,unsigned int fileLen)
{
	std::string fontName = "";

	unsigned char* readData=data;
	unsigned int	pos=0;

	if(data && (fileLen>0))
	{
		TTF_OFFSET_TABLE tabOffset = { 0 };

		memcpy(&tabOffset,readData, sizeof(TTF_OFFSET_TABLE));

		readData+=sizeof(TTF_OFFSET_TABLE);
		pos+=sizeof(TTF_OFFSET_TABLE);

		tabOffset.uNumOfTables = SWAPWORD(tabOffset.uNumOfTables);
		tabOffset.uMajorVersion = SWAPWORD(tabOffset.uMajorVersion);
		tabOffset.uMinorVersion = SWAPWORD(tabOffset.uMinorVersion);

		TTF_TABLE_DIRECTORY tblDir = { {0} };
		bool bValid = false;

		do
		{
			memcpy(&tblDir,readData, sizeof(TTF_TABLE_DIRECTORY));
			readData+=sizeof(TTF_TABLE_DIRECTORY);
			pos+=sizeof(TTF_TABLE_DIRECTORY);

			if(strncmp(tblDir.szTag, TAG_NAME_A, 4) == 0)
			{
				bValid = true;
				tblDir.uLength = SWAPLONG(tblDir.uLength);
				tblDir.uOffset = SWAPLONG(tblDir.uOffset);
				break;
			}
		}
		while (pos < fileLen);

		if(bValid)
		{
			unsigned int seek = 0;
			seek = tblDir.uOffset;
			readData=data+seek;
			pos=seek;

			TTF_NAME_TABLE_HEADER tabHeader = { 0 };
			memcpy(&tabHeader,readData, sizeof(TTF_NAME_TABLE_HEADER));
			readData+=sizeof(TTF_NAME_TABLE_HEADER);
			pos+=sizeof(TTF_NAME_TABLE_HEADER);

			tabHeader.uNRCount = SWAPWORD(tabHeader.uNRCount);
			tabHeader.uStorageOffset = SWAPWORD(tabHeader.uStorageOffset);

			TTF_NAME_RECORD tabRecord = { 0 };
			bValid = false;

			do
			{
				memcpy(&tabRecord,readData, sizeof(TTF_NAME_RECORD));
				readData+=sizeof(TTF_NAME_RECORD);
				pos+=sizeof(TTF_NAME_RECORD);

				tabRecord.uNameID = SWAPWORD(tabRecord.uNameID);

				if(tabRecord.uNameID == 1)
				{
					tabRecord.uStringLength = SWAPWORD(tabRecord.uStringLength);
					tabRecord.uStringOffset = SWAPWORD(tabRecord.uStringOffset);

					// get current pos
					unsigned int oldPointer = pos;

					seek = tblDir.uOffset + tabRecord.uStringOffset + tabHeader.uStorageOffset;
					readData=data+seek;
					pos=seek;

					unsigned char* lpszNameBuf = new unsigned char[tabRecord.uStringLength + 1];
					//TCHAR* lpszNameBuf = new TCHAR[tabRecord.uStringLength + 1];
					memset(lpszNameBuf,0, tabRecord.uStringLength + 1);

					memcpy(lpszNameBuf,readData, tabRecord.uStringLength);
					readData+=tabRecord.uStringLength;
					pos+=tabRecord.uStringLength;

					//ReadFile(hFile, lpszNameBuf, tabRecord.uStringLength * 2, &bytesRead, NULL);

					if(strlen((const char*)lpszNameBuf) > 0)
						//if(wcslen(lpszNameBuf) > 0)
					{
						fontName = (const char*)lpszNameBuf;
						//fontName = ref new Platform::String(lpszNameBuf);
						delete[] lpszNameBuf;
						bValid = true;
						break;
					}

					pos=oldPointer;
					seek=oldPointer;
					readData=data+pos;
				}
			}
			while (pos < fileLen);
		}
	}

	return fontName;
}

void	FreeType_TextDrawer::initFontFaceCache()
{
	mFontFaceCache.clear();
	mCurrentFontFace=-1;
}

void	FreeType_TextDrawer::freeFontFaceCache()
{
	std::vector<FontFaceStruct*>::iterator	it;
	for(it=mFontFaceCache.begin();it!=mFontFaceCache.end();it++)
	{
		delete (*it);
	}
	initFontFaceCache();
}

FreeType_TextDrawer::FontFaceStruct*	FreeType_TextDrawer::searchOlderFontFaceInCache()
{
	// if cache can have more font
	if(mFontFaceCache.size()<MAX_FONT_CACHE_SIZE)
	{
		mFontFaceCache.push_back(new FontFaceStruct());
		mCurrentFontFace=(unsigned int)(mFontFaceCache.size()-1);
	}
	else
	{
		int olderIndex=0;
		int index=0;
		unsigned int older=mUseCount;
		std::vector<FontFaceStruct*>::iterator	it;
		for(it=mFontFaceCache.begin();it!=mFontFaceCache.end();it++)
		{
			if((*it)->mUsedCount<older)
			{
				older=(*it)->mUsedCount;
				olderIndex=index;
			}
			index++;
		}
		mFontFaceCache[olderIndex]->release();
		mCurrentFontFace=olderIndex;
	}
	return mFontFaceCache[mCurrentFontFace];
}

bool FreeType_TextDrawer::IsInCache(const char* fontName)
{
	std::vector<FontFaceStruct*>::iterator	it;
	for(it=mFontFaceCache.begin();it!=mFontFaceCache.end();it++)
	{
		if((*it)->mFontName == fontName)
		{
			return true;
		}
	}
	return false;
}

FreeType_TextDrawer::FontFaceStruct* FreeType_TextDrawer::getFontStruct(const char* fontname)
{
	int index=0;
	std::vector<FontFaceStruct*>::iterator	it;
	for(it=mFontFaceCache.begin();it!=mFontFaceCache.end();it++)
	{
		if((*it)->mFontName == fontname)
		{
			mCurrentFontFace=index;
			return *it;
		}
		index++;
	}
	return 0;
}

void	FreeType_TextDrawer::FontFaceStruct::init()
{
	mFontSize=-1;
	mFontName="";
	mFontFace=0;
	mFaceBuffer=0;
	mUsedCount=0;
}

void	FreeType_TextDrawer::FontFaceStruct::release()
{
	if(mFontFace != 0) FT_Done_Face(mFontFace);
	if(mFaceBuffer != 0) delete[] mFaceBuffer;
	init();
}
