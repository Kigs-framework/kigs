#include "XLSXElementRef.h"
#include "XLSXSheet.h"

XLSXCell XLSXElementRef::mBadCell;

void	CellName::interpretStringName()
{
	std::string col = getCol();
	std::string row = getRow();

	mIndexes[0] = XLSXSheet::getColIndex(col);
	mIndexes[1] = XLSXSheet::getRowIndex(row);
}

void	CellName::setIndex(const v2i& i)
{
	mIndexes = i;
	mName = XLSXSheet::getCellName(i[0], i[1]);
}

std::string CellName::getCol()
{
	return XLSXSheet::getColName(mName);
}
std::string	CellName::getRow()
{
	return XLSXSheet::getRowName(mName);
}

XLSXElementRef XLSXElementRef::operator[](const std::string& name) const
{
	XLSXElementRef result(mSheet);

	if (mSheet)
	{
		if (mRow) // if this ref a row, then name should contain a column
		{
			s32 colindex = XLSXSheet::getColIndex(name);
			if (colindex >= 0)
			{
				result.mCell = mSheet->getCell({ colindex,mRow->mIndex });
			}
		}
		else if (mCol.mIndex >= 0) // if this ref a col, then name should contain a row
		{
			s32 rowindex = XLSXSheet::getRowIndex(name);
			if (rowindex >= 0)
			{
				result.mCell = mSheet->getCell({ mCol.mIndex,rowindex });
			}
		}
		else if (mCell) // can't go further
		{
			result = *this;
		}
		else // search cell
		{
			result = XLSXElementRef(name, mSheet);
		}
	}
	return result;
}

XLSXElementRef::XLSXElementRef(const std::string& ref, XLSXSheet* sheetRef) : mSheet(sheetRef)
{
	v2i pos = XLSXSheet::getCellPos(ref);

	if (pos.x < 0) // no column
	{
		if (pos.y < 0) // no row
		{
			return;
		}

		mRow = mSheet->getRow(pos.y);
	}
	else if (pos.y < 0) // no row
	{
		mCol = mSheet->getCol(pos.x);
	}
	else
	{
		mCell = mSheet->getCell(pos);
	}
}


XLSXElementRef	XLSXElementRef::row() const
{
	XLSXElementRef result(mSheet);

	if (mSheet)
	{
		if (mCell)
		{
			result.mRow = mSheet->getRow(mCell->mName.mIndexes.y);
		}
		else if (mRow)
		{
			result = *this;
		}
	}
	return result;
}

XLSXElementRef	XLSXElementRef::col() const
{
	XLSXElementRef result(mSheet);
	if (mSheet)
	{
		if (mCell)
		{
			result.mCol = mSheet->getCol(mCell->mName.mIndexes.x);
		}
		else if (mCol.mIndex >= 0)
		{
			result = *this;
		}

	}
	return result;
}

XLSXCell&	XLSXElementRef::operator*() const
{
	XLSXCell* result = mCell;
	if ((mITPos >= 0) && !result)
	{
		if (mRow && (mITPos < mRow->size()))
			result = &((*mRow)[mITPos]);
		else if (mCol.mIndex >= 0)
		{
			result = mSheet->getCell({ mCol.mIndex ,mITPos });
		}
	}
	if (result)
		return *result;

	return mBadCell;
}

XLSXElementRef	XLSXElementRef::begin() const
{
	XLSXElementRef result(*this);
	result.mITPos = -1;
	result.msITPos = -1;
	if (mRow || (mCol.mIndex >= 0)) // can iterate cells on row or col
	{
		result.mITPos = 0;
	}
	else if (mSheet) // or can iterate row on sheet
	{
		result.msITPos = 0;
		result.mRow = mSheet->getRow(result.msITPos);
	}
	return result;
}

XLSXElementRef& XLSXElementRef::operator++()
{
	if (mITPos >= 0)
	{
		mITPos++;
		if (mRow)
		{
			if (mITPos >= mRow->size())
			{
				mITPos = -1;
			}
		}
		else if (mCol.mIndex >= 0)
		{
			if (mITPos >= mSheet->getColCount())
			{
				mITPos = -1;
			}
		}
	}
	else if (msITPos>=0)
	{
		msITPos++;
		if (msITPos >= mSheet->getRowCount())
		{
			msITPos = -1;
			mRow = nullptr;
		}
		else
		{
			mRow = mSheet->getRow(msITPos);
		}
	}
	return *this;
}

std::vector<XLSXElementRef>	XLSXElementRef::find(const std::string& content,bool exactmatch)
{
	std::vector<XLSXElementRef> result;
	if (mSheet)
	{
		if (mITPos==-1) // iterator
		{
			for (XLSXElementRef r=(*this).begin();r!=(*this).end();++r)
			{
				std::vector<XLSXElementRef> recurseresult = r.find(content, exactmatch);
				result.insert(result.end(), recurseresult.begin(), recurseresult.end());
			}
		}
		else 
		{
			if (exactmatch)
			{
				if (content == std::string(*(*this)))
				{
					result.push_back(*this);
				}
			}
			else
			{
				std::string val = *(*this);

				if (val.find(content) != std::string::npos) 
				{
					result.push_back(*this);
				}
			}
		}
	}
	return result;
}

std::vector<XLSXElementRef>	XLSXElementRef::find(int content)
{
	std::vector<XLSXElementRef> result;
	if (mSheet)
	{
		if (mITPos == -1) // iterator
		{
			for (XLSXElementRef r = (*this).begin(); r != (*this).end(); ++r)
			{
				std::vector<XLSXElementRef> recurseresult = r.find(content);
				result.insert(result.end(), recurseresult.begin(), recurseresult.end());
			}
		}
		else
		{
			if (content == int(*(*this)))
			{
				result.push_back(*this);
			}
		}
	}
	return result;
}
