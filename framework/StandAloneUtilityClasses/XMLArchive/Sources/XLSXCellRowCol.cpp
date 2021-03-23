#include "XLSXCellRowCol.h"
#include "XLSXSheet.h"


size_t	XLSXCol::size() const
{
	if (mSheet && (mIndex >= 0))
	{
		mSheet->getRowCount();
	}
	return std::string::npos;
}