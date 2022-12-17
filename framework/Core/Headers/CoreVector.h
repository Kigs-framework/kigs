#ifndef _COREVECTOR_H
#define _COREVECTOR_H

#include "Core.h"
#include "CoreItem.h"
#include "CoreValue.h"

template<class BaseClass>
class CoreVectorBase;

// ****************************************
// * CoreVectorIterator class
// * --------------------------------------
/**
* \class	CoreVectorIterator
* \file		CoreVector.h
* \ingroup Core
* \brief	Iterator for CoreVector
*/
// ****************************************

class CoreVectorIterator : public CoreItemIteratorBase
{
public:

	CoreVectorIterator(const CoreItemIteratorBase & other) : CoreItemIteratorBase(other)
	{
		mVectorIterator = ((const CoreVectorIterator*)&other)->mVectorIterator;
	}

	virtual CoreItemSP operator*() const;

	virtual CoreItemIteratorBase*	clone()
	{
		CoreVectorIterator*	result = new CoreVectorIterator(mAttachedCoreItem, mPos);
		result->mVectorIterator = mVectorIterator;
		return result;
	}

	virtual CoreItemIteratorBase& operator=(const CoreItemIteratorBase & other)
	{
		CoreItemIteratorBase::operator=(other);

		mVectorIterator = ((const CoreVectorIterator*)&other)->mVectorIterator;
		return *this;
	}

	virtual CoreItemIteratorBase& operator++()
	{
		mVectorIterator++;
		return *this;
	}

	virtual CoreItemIteratorBase& operator+(const int decal)
	{
		mVectorIterator+=decal;
		return *this;
	}

	virtual CoreItemIteratorBase operator++(int)
	{
		CoreVectorIterator	tmp(*this);
		operator++();
		return tmp;
	}

	virtual bool operator==(const CoreItemIteratorBase & other) const
	{
		if (mVectorIterator == ((CoreVectorIterator*)&other)->mVectorIterator)
		{
			return true;
		}
		return false;
	}

	virtual bool operator!=(const CoreItemIteratorBase & other) const
	{
		if (mVectorIterator != ((CoreVectorIterator*)&other)->mVectorIterator)
		{
			return true;
		}
		return false;
	}

	CoreVectorIterator(const CoreItemSP& item, unsigned int pos) : CoreItemIteratorBase(item, pos)
	{

	}

	std::vector<CoreItemSP>::iterator	mVectorIterator;
};

// ****************************************
// * CoreVectorBase class
// * --------------------------------------
/**
* \class	CoreVectorBase
* \file		CoreVector.h
* \ingroup Core
* \brief	Base class for CoreVector
*/
// ****************************************

template<class BaseClass>
class CoreVectorBase : public BaseClass
{
protected:

	CoreVectorBase(CoreItem::COREITEM_TYPE _type):
	BaseClass(_type)
	{
		mVector.clear();
	}

public:

	virtual void set(int key, const CoreItemSP& toinsert) override
	{
		mVector[key] = toinsert;
	}
	virtual void set(const std::string& key, const CoreItemSP& toinsert) override
	{
		if (key.size())
		{
			// TODO : check if key contains number ?
		}
		else
		{
			push_back(toinsert);
		}
	}
	virtual void set(const usString& key, const CoreItemSP& toinsert) override
	{
		if (key.length())
		{
			// TODO : check if key contains number ?
		}
		else
		{
			push_back(toinsert);
		}
	}

	virtual void erase(int key) override
	{
		auto it = mVector.begin();
		it += key;
		if(it!= mVector.end())
			mVector.erase(it);
	}
	virtual void erase(const std::string& key) override
	{
		if (key.size())
		{
			// TODO : check if key contains number ?
		}
		else
		{
			pop_back();
		}
	}
	virtual void erase(const usString& key) override
	{
		if (key.length())
		{
			// TODO : check if key contains number ?
		}
		else
		{
			pop_back();
		}
	}


	virtual ~CoreVectorBase()
	{
		clear();
	}

	friend class CoreVectorIterator;

	// wrapper on member vector
	CoreItemIterator begin() override
	{
		CoreVectorIterator* iter = new CoreVectorIterator(this->SharedFromThis(), 0);
		iter->mVectorIterator = mVector.begin();
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}


	CoreItemIterator end() override
	{
		CoreVectorIterator* iter = new CoreVectorIterator(this->SharedFromThis(), 0);
		iter->mVectorIterator = mVector.end();
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	CoreItem::size_type size() const  override
	{
		return mVector.size();
	}

	CoreItem::size_type max_size() const
	{
		return mVector.max_size();
	}

	void resize(CoreItem::size_type n)
	{
		mVector.resize(n,0);
	}

	virtual bool empty() const  override
	{
		return mVector.empty();
	}

	const CoreItemSP back() const
	{
		return mVector.back();
	}

	const CoreItemSP front() const
	{
		return mVector.front();
	}

	const CoreItemSP at(CoreItem::size_type n) const
	{
		return mVector.at(n);
	}

	void push_back (const CoreItemSP& val)
	{
		mVector.push_back(val);
	}

	void pop_back()
	{
		mVector.pop_back();
	}

	CoreItemIterator erase(CoreItemIterator position)
	{
		CoreVectorIterator& pos = *(CoreVectorIterator*)position.get();
		CoreVectorIterator* iter = new CoreVectorIterator(this->SharedFromThis(), 0);
		iter->mVectorIterator = mVector.erase(pos.mVectorIterator);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}
	CoreItemIterator erase(CoreItemIterator first, CoreItemIterator last)
	{
		CoreVectorIterator* iter = new CoreVectorIterator(this->SharedFromThis(), 0);
		iter->mVectorIterator = mVector.erase(((CoreVectorIterator*)first.get())->mVectorIterator, ((CoreVectorIterator*)last.get())->mVectorIterator);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	void clear()
	{
		mVector.clear();
	}

	void insert(CoreItemIterator position, const CoreItemSP& toinsert)
	{
		mVector.insert(((CoreVectorIterator*)&position)->mVectorIterator, toinsert);
	}

	void insert(CoreItem::size_type position, const CoreItemSP& toinsert)
	{
		mVector.insert(mVector.begin()+position,toinsert);
	}

	void set(CoreItem::size_type position, const CoreItemSP& toinsert)
	{
		mVector[position]=toinsert;
	}

	CoreVectorBase& operator= (const CoreVectorBase& x)
	{
		mVector.clear();
		for(auto& el : x)
		{
			mVector.push_back(el);
		}
		return *this;
	}

	virtual inline CoreItemSP operator[](int i) override
	{
		if ((i >= 0) && (i < (int)mVector.size()))
		{
			return mVector[i];
		}
		return CoreItemSP(nullptr);
	}

	virtual void*	getContainerStruct()  override
	{
		return &mVector;
	}

	virtual operator Point2D() const override
	{
		Point2D result;

		mVector[0]->getValue(result.x);
		mVector[1]->getValue(result.y);

		return result;
	}

	virtual operator Point3D() const override
	{
		Point3D result;

		mVector[0]->getValue(result.x);
		mVector[1]->getValue(result.y);
		mVector[2]->getValue(result.z);

		return result;
	}

	virtual operator Vector4D() const override
	{
		Vector4D result;

		mVector[0]->getValue(result.x);
		mVector[1]->getValue(result.y);
		mVector[2]->getValue(result.z);
		mVector[3]->getValue(result.w);

		return result;
	}

protected:
	std::vector<CoreItemSP>	mVector;
};


// ****************************************
// * CoreVector class
// * --------------------------------------
/**
* \class	CoreVector
* \file		CoreVector.h
* \ingroup Core
* \brief	CoreItem managing a vector of CoreItemSP
*/
// ****************************************

class CoreVector : public CoreVectorBase<CoreItem>
{
public:
	CoreVector():
	CoreVectorBase<CoreItem>(COREVECTOR)
	{
	}
};

#endif // _COREVECTOR_H