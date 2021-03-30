#pragma once

#include <string>
#include "XML.h"
#include "CoreItem.h"
#include "XLSXCellRowCol.h"
#include "XLSXElementRef.h"

class XLSXSheet
{
protected:
	std::string						mName;
	s32								mSheedID=0;
	std::vector<XLSXRow>			mRows;
	XLSXSharedStrings*				mSharedStrings=nullptr;
	XMLBase*						mBaseXML = nullptr;
	friend class XLSXDocument;

	XLSXSheet() {};

public:

	XLSXSheet(const std::string& name, s32 id,XLSXSharedStrings* sharedstrings) : mName(name), mSheedID(id), mSharedStrings(sharedstrings) {};
	~XLSXSheet() {};

	const std::string& getName()
	{
		return mName;
	}

	void initFromXML(XMLBase* xml);
	void updateXML(XMLBase* xml);

	v2i	getDimension()
	{
		if (mRows.size())
		{
			return v2i((s32)mRows[0].size(),(s32) mRows.size());
		}
		return v2i(0, 0);
	}

	size_t	getRowCount()
	{
		return mRows.size();
	}
	size_t	getColCount()
	{
		if (mRows.size())
		{
			return mRows[0].size();
		}
		return 0;
	}

	XLSXRow* getRow(u32 i)
	{
		if (i < mRows.size())
		{
			return &mRows[i];
		}
		return nullptr;
	}

	XLSXCol getCol(u32 i)
	{
		XLSXCol result(this,-1);
		if (mRows.size())
		{
			if (i < mRows[0].size())
			{
				result.mIndex = i;
			}
		}
		return result;
	}

	XLSXCell* getCell(v2i pos)
	{
		if ((pos.y >= 0) && (pos.y < mRows.size()))
		{
			if ((pos.x >= 0) && (pos.x < mRows[0].size()))
			{
				return &mRows[pos.y][pos.x];
			}
		}
		return nullptr;
	}

	XLSXElementRef operator[](const std::string& name)
	{
		XLSXElementRef result(name, this);
		return result;
	}

	void	setRange(const std::string& range);

	static	std::string	getCellName(u32 col, u32 row);
	static	s32	getColIndex(const std::string& colname);
	static	std::string	getColName(const std::string& cellname);
	static	s32	getRowIndex(const std::string& rowname);
	static	std::string	getRowName(const std::string& cellname);

	static  v2i	getCellPos(const std::string& cellname);

	static  std::pair<v2i, v2i>	getRange(const std::string& rangename);
};

