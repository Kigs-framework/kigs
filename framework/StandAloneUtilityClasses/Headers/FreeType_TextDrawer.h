#ifndef _FREETYPE_TEXTDRAWER_H_
#define _FREETYPE_TEXTDRAWER_H_


// as generic is a keyword when compiling to UWP, redifine it before including freetype 
#define generic GenericFromFreeTypeLibrary
#include "ft2build.h"
#include FT_FREETYPE_H
#undef generic

#include <vector>
#include <string>
#include <map>

/**
	Utility class to draw text using freetype library in pixel buffer		
	Can also compute text size and draw effects (shadow strokes...) directly in buffer
*/

#define SWAPWORD(a) ((unsigned short)(((a&0xFF)<<8) | ((a&0xFF00)>>8)))
#define SWAPLONG(a) ((unsigned int)(((a&0xFF)<<24) | ((a&0xFF00)<<8) | ((a&0xFF0000)>>8) | ((a&0xFF000000)>>24)))


enum TextAlignment
{
	TextAlignmentLeft = 0,
	TextAlignmentCenter,
	TextAlignmentRight,
	TextAlignmentJustified,
};

typedef struct tagTTF_OFFSET_TABLE{
	unsigned short	uMajorVersion;
	unsigned short	uMinorVersion;
	unsigned short	uNumOfTables;
	unsigned short	uSearchRange;
	unsigned short	uEntrySelector;
	unsigned short	uRangeShift;
}TTF_OFFSET_TABLE;

typedef struct tagTTF_TABLE_DIRECTORY{
	char	szTag[4];
	unsigned long	uCheckSum;
	unsigned long	uOffset;
	unsigned long	uLength;
}TTF_TABLE_DIRECTORY;

typedef struct tagTTF_NAME_TABLE_HEADER{
	unsigned short	uFSelector;
	unsigned short	uNRCount;
	unsigned short	uStorageOffset;
}TTF_NAME_TABLE_HEADER;

typedef struct tagTTF_NAME_RECORD{
	unsigned short	uPlatformID;
	unsigned short	uEncodingID;
	unsigned short	uLanguageID;
	unsigned short	uNameID;
	unsigned short	uStringLength;
	unsigned short	uStringOffset;
}TTF_NAME_RECORD;


// ****************************************
// * FreeType_TextDrawer class
// * --------------------------------------
/**
 * \file	FreeType_TextDrawer.h
 * \class	FreeType_TextDrawer
 * \ingroup StandAlone
 * \brief	Draw freetype texts in texture buffers.
 *
 */
 // ****************************************
class FreeType_TextDrawer
{
protected:

	static unsigned int mUseCount;
	static unsigned int mInterline;

public:
	virtual ~FreeType_TextDrawer();
	FreeType_TextDrawer();

	static void SetDefaultInterline(unsigned int interline)
	{
		mInterline = interline;
	}

	static unsigned int GetDefaultInterline()
	{
		return mInterline;
	}

	bool IsInCache(const char* fontName);
	bool SetFont(const char* fontName, const char* fullPath,unsigned int nSize);
	bool SetFont(const char* fontName, unsigned char * data, unsigned int fileSize, unsigned int nSize);

	unsigned char* DrawTextToImage(const char*           text, unsigned int tl, int& sizex, int& sizey, TextAlignment alignment, bool onlySize=false,unsigned int a_maxLineNumber=0,int a_maxSize=0, int a_drawingLimit=-1, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
	unsigned char* DrawTextToImage(const unsigned short* text, unsigned int tl, int& sizex, int& sizey, TextAlignment alignment, bool onlySize=false,unsigned int a_maxLineNumber=0,int a_maxSize=0, int a_drawingLimit=-1, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
	unsigned char* DrawTextWithInterlineToImage(const char*           text, unsigned int il, unsigned int tl, int& sizex, int& sizey, TextAlignment alignment, bool onlySize = false, unsigned int a_maxLineNumber = 0, int a_maxSize = 0, int a_drawingLimit = -1);
	unsigned char* DrawTextWithInterlineToImage(const unsigned short* text, unsigned int il, unsigned int tl, int& sizex, int& sizey, TextAlignment alignment, bool onlySize = false, unsigned int a_maxLineNumber = 0, int a_maxSize = 0, int a_drawingLimit = -1);

	void	startBuildFonts();

	struct LineStruct
	{
		unsigned int startIndex;
		unsigned int endIndex;
		unsigned int len;
		bool hasLineFeed;
	};

	struct CharacterData
	{
		int	width;
		int advance;
		int bearing;
	};

	void manageLineEnd(LineStruct& currentLine, const unsigned short* text, CharacterData* chardata);

	std::vector<LineStruct> splitString(const unsigned short* text, unsigned int tl,unsigned int maxHeight,unsigned int maxWidth,unsigned int& foundw,unsigned int& foundh,unsigned int& baseline,unsigned int a_maxLineNumber);

private:
	class	FontFaceStruct
	{
	public:
		
		FT_Face					mFontFace;
		std::string				mFontName;
		unsigned int			mFontSize;
		unsigned char *			mFaceBuffer;
		unsigned int			mUsedCount;

		FontFaceStruct()
		{
			init();
		}
		~FontFaceStruct()
		{	
			release();
		}

		void	init();
		void	release();
	};

	FT_Library				mLibraryContext;

	void			initFontFaceCache();
	void			freeFontFaceCache();
	FontFaceStruct*	searchOlderFontFaceInCache();
	FontFaceStruct* getFontStruct(const char* fontname);

	std::vector<FontFaceStruct*>	mFontFaceCache;
	unsigned int					mCurrentFontFace;

	std::string GetFontNameFromFile(unsigned char* data,unsigned int fileLen);
	
private:

	void Log_FT_Error(FT_Error error);
};

#endif //_FREETYPE_TEXTDRAWER_H_