#pragma once
#include <string>
#include "XML.h"
#include "CoreItem.h"
#include "XLSXCellRowCol.h"

class XLSXSheet;
class XLSXCell;
class XLSXRow;

class XLSXElementRef
{
protected:
	XLSXSheet*	mSheet = nullptr;
	XLSXCell*	mCell = nullptr;
	XLSXCol		mCol;
	XLSXRow*	mRow = nullptr;

	// when used as a col or row iterator
	s32			mITPos = -1;
	// when used as a sheet iterator
	s32			msITPos = -1;

	XLSXElementRef() {};
	XLSXElementRef(XLSXSheet* sheetRef) :mSheet(sheetRef) {};

	friend class XLSXDocument;

public:
	XLSXElementRef(const std::string& ref, XLSXSheet* sheetRef);

	XLSXElementRef operator[](const std::string& name)const;

	XLSXCell&		operator*() const;

	// if referencing a cell or a row, return row containing the cell (or row itself)
	XLSXElementRef	row() const;
	// if referencing a cell or a col, return col containing the cell (or col itself)
	XLSXElementRef	col() const;
	// get sheet reference
	XLSXElementRef sheet() const
	{
		return XLSXElementRef(mSheet);
	}

	operator XLSXSheet* ()
	{
		return mSheet;
	}

	operator XLSXCell*()
	{
		return mCell;
	}

	// if this ref a row or column, 
	XLSXElementRef	begin() const;

	XLSXElementRef	end() const
	{
		XLSXElementRef result(*this);
		result.mITPos = -1;
		result.msITPos = -1;
		return result;
	}

	XLSXElementRef& operator++();

	XLSXElementRef operator++(int)
	{
		XLSXElementRef result = *this;
		operator++();
		return result;
	}

	bool operator == (const XLSXElementRef& other) const
	{
		if ((mSheet == other.mSheet) && (mCell == other.mCell) && (mRow == other.mRow))
		{
			if ((mITPos == other.mITPos) && (msITPos == other.msITPos) && (mCol.mIndex == other.mCol.mIndex))
			{
				return true;
			}
		}
		return false;
	}

	bool operator != (const XLSXElementRef& other) const
	{
		return !((*this) == other);
	}

	std::vector<XLSXElementRef>	find(const std::string& content,bool exactmatch=false);
	std::vector<XLSXElementRef>	find(int content);

	static XLSXCell mBadCell;
};
