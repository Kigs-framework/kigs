#pragma once

#include <string>
#include "XML.h"
#include "CoreItem.h"

class CellName
{
protected:
	std::string	mName;
	v2i			mIndexes;

	void	interpretStringName();
	friend class XLSXElementRef;
public:
	CellName() {};
	CellName(const std::string& n) : mName(n)
	{
		interpretStringName();
	};
	~CellName() {};

	void	setName(const std::string& n)
	{
		mName = n;
		interpretStringName();
	}

	void	setIndex(const v2i& i);

	std::string getCol();
	std::string	getRow();

	// index starting at 0
	u32			getColIndex()
	{
		return mIndexes[0];
	}


	// index starting at 0
	u32			getRowIndex()
	{
		return mIndexes[1];
	}

	const std::string& getName()
	{
		return mName;
	}

private:

};


class XLSXCell : public CoreItemSP
{
protected:
	CellName	mName;
	friend class XLSXSheet;
	friend class XLSXElementRef;
public:

	XLSXCell() : CoreItemSP(), mName() {};

	XLSXCell& operator=(const CoreItemSP& other)
	{
		CoreItemSP::operator=(other);
		return *this;
	}

	const std::string& getName()
	{
		return mName.getName();
	}

	inline operator float() const
	{
		if (mPointer)
			return mPointer->operator kfloat();
		return 0.0f;
	}

	inline operator double() const
	{
		if (mPointer)
			return mPointer->operator double();
		return 0.0;
	}

	inline operator int() const
	{
		if (mPointer)
			return mPointer->operator int();
		return 0;
	}

	inline operator unsigned int() const
	{
		if (mPointer)
			return mPointer->operator unsigned int();
		return 0;
	}

	inline operator s64() const
	{
		if (mPointer)
			return mPointer->operator s64();
		return 0;
	}

	inline operator u64() const
	{
		if (mPointer)
			return mPointer->operator u64();
		return 0;
	}

	inline operator kstl::string() const
	{
		if (mPointer)
			return mPointer->operator kstl::string();
		return "";
	}

	inline operator usString() const
	{
		if (mPointer)
			return mPointer->operator usString();
		return usString("");
	}

};

class XLSXSheet;

class XLSXRow : public std::vector<XLSXCell>
{
protected:
	s32			mIndex = 0;
	XLSXSheet* mSheet = nullptr;

	friend class XLSXSheet;
	friend class XLSXElementRef;
public:

	XLSXRow() : std::vector<XLSXCell>() {};


};

class XLSXSharedStrings;
// just a reference to the good column in sheet
class XLSXCol
{
protected:

	s32			mIndex = -1;
	XLSXSheet* mSheet = nullptr;
	friend class XLSXSheet;
	friend class XLSXElementRef;
	XLSXCol(XLSXSheet* s, s32 i) : mSheet(s), mIndex(i)
	{

	}
public:
	XLSXCol()
	{

	}

	size_t	size() const;

};



