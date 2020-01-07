#ifndef _COREVECTOR_H
#define _COREVECTOR_H

#include "Core.h"
#include "CoreItem.h"
#include "CoreValue.h"

// ****************************************
// * CoreVector class
// * --------------------------------------
/**
* \class	CoreVector
* \ingroup KigsCore
* \brief	a vector of RefCountedBaseClass, maintain ref count of instances in vector
* \author	ukn
* \version ukn
* \date	ukn
*/
// ****************************************



template<class BaseClass>
class CoreVectorBase;

class CoreVectorIterator : public CoreItemIteratorBase
{
public:

	CoreVectorIterator(const CoreItemIteratorBase & other) : CoreItemIteratorBase(other)
	{
		myVectorIterator = ((const CoreVectorIterator*)&other)->myVectorIterator;
	}

	virtual CoreItemSP operator*() const;

	virtual CoreItemIteratorBase*	clone()
	{
		CoreVectorIterator*	result = new CoreVectorIterator(myAttachedCoreItem, myPos);
		result->myVectorIterator = myVectorIterator;
		return result;
	}

	virtual CoreItemIteratorBase& operator=(const CoreItemIteratorBase & other)
	{
		CoreItemIteratorBase::operator=(other);

		myVectorIterator = ((const CoreVectorIterator*)&other)->myVectorIterator;
		return *this;
	}

	virtual CoreItemIteratorBase& operator++()
	{
		myVectorIterator++;
		return *this;
	}

	virtual CoreItemIteratorBase& operator+(const int decal)
	{
		myVectorIterator+=decal;
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
		if (myVectorIterator == ((CoreVectorIterator*)&other)->myVectorIterator)
		{
			return true;
		}
		return false;
	}

	virtual bool operator!=(const CoreItemIteratorBase & other) const
	{
		if (myVectorIterator != ((CoreVectorIterator*)&other)->myVectorIterator)
		{
			return true;
		}
		return false;
	}

	CoreVectorIterator(const CoreItemSP& item, unsigned int pos) : CoreItemIteratorBase(item, pos)
	{

	}

	kstl::vector<CoreItemSP>::iterator	myVectorIterator;
};



template<class BaseClass>
class CoreVectorBase : public BaseClass
{
protected:

	CoreVectorBase(CoreItem::COREITEM_TYPE _type):
	BaseClass(_type)
	{
		myVector.clear();
	}

public:

	virtual void set(int key, const CoreItemSP& toinsert)
	{
		myVector[key] = toinsert;
	}
	virtual void set(const kstl::string& key, const CoreItemSP& toinsert)
	{
		// TODO : check if key contains number ?
		push_back(toinsert);
	}
	virtual void set(const usString& key, const CoreItemSP& toinsert)
	{
		// TODO : check if key contains number ?
		push_back(toinsert);
	}


	virtual ~CoreVectorBase()
	{
		clear();
	}

	friend class CoreVectorIterator;

	// wrapper on member vector
	CoreItemIterator begin() override
	{
		CoreVectorIterator* iter = new CoreVectorIterator(CoreItemSP(this, GetRefTag{}), 0);
		iter->myVectorIterator = myVector.begin();
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}
	

	CoreItemIterator end() override
	{
		CoreVectorIterator* iter = new CoreVectorIterator(CoreItemSP(this, GetRefTag{}), 0);
		iter->myVectorIterator = myVector.end();
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	CoreItem::size_type size() const
	{
		return myVector.size();
	}

	CoreItem::size_type max_size() const
	{
		return myVector.max_size();
	}

	void resize(CoreItem::size_type n)
	{
		myVector.resize(n,0);
	}

	virtual bool empty() const
	{
		return myVector.empty();
	}

	const CoreItemSP back() const
	{
		return myVector.back();
	}

	const CoreItemSP front() const
	{
		return myVector.front();
	}

	const CoreItemSP at(CoreItem::size_type n) const
	{
		return myVector.at(n);
	}

	void push_back (const CoreItemSP& val)
	{
		myVector.push_back(val);
	}

	void pop_back()
	{
		myVector.pop_back();
	}

	CoreItemIterator erase(CoreItemIterator position)
	{
		CoreVectorIterator& pos = *(CoreVectorIterator*)position.Pointer();
		CoreVectorIterator* iter = new CoreVectorIterator(CoreItemSP(this), 0);
		iter->myVectorIterator = myVector.erase(pos.myVectorIterator);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}
	CoreItemIterator erase(CoreItemIterator first, CoreItemIterator last)
	{
		CoreVectorIterator* iter = new CoreVectorIterator(CoreItemSP(this), 0);
		iter->myVectorIterator = myVector.erase(((CoreVectorIterator*)first.myPointer)->myVectorIterator, ((CoreVectorIterator*)last.myPointer)->myVectorIterator);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	void clear()
	{
		myVector.clear();
	}

	void insert(CoreItemIterator position, const CoreItemSP& toinsert)
	{
		myVector.insert(((CoreVectorIterator*)&position)->myVectorIterator, toinsert);
	}

	void insert(CoreItem::size_type position, const CoreItemSP& toinsert)
	{
		myVector.insert(myVector.begin()+position,toinsert);
	}

	void set(CoreItem::size_type position, const CoreItemSP& toinsert)
	{
		myVector[position]=toinsert;
	}

	CoreVectorBase& operator= (const CoreVectorBase& x)
	{
		myVector.clear();
		kstl::vector<RefCountedBaseClass*>::const_iterator itstart;
		kstl::vector<RefCountedBaseClass*>::const_iterator itend=x.end();

		for(itstart=x.begin();itstart!=itend;itstart++)
		{
			myVector.push_back(*itstart);
		}

		return *this;
	}

	// some utility
	inline void	getPoint2D(Point2D& result)
	{
		myVector[0]->getValue(result.x);
		myVector[1]->getValue(result.y);
	}
	inline void	getPoint3D(Point3D& result)
	{
		myVector[0]->getValue(result.x);
		myVector[1]->getValue(result.y);
		myVector[2]->getValue(result.z);
	}
	inline void	getPoint4D(Quaternion& result)
	{
		myVector[0]->getValue(result.V.x);
		myVector[1]->getValue(result.V.y);
		myVector[2]->getValue(result.V.z);
		myVector[3]->getValue(result.w);
	}


	virtual inline CoreItemSP operator[](int i) const
	{
		if ((i >= 0) && (i < (int)myVector.size()))
		{
			return myVector[i];
		}
		return CoreItemSP(nullptr);
	}

	virtual inline CoreItemSP operator[](const kstl::string& key) const
	{
		kstl::vector<CoreItemSP>::const_iterator it = myVector.begin();

		while (it != myVector.end())
		{
			if ((*it)->GetType()&(unsigned int)CoreItem::CORENAMEDITEMMASK)
			{
				if (((CoreNamedItem*)(*it).get())->getName() == key)
				{
					return (*it);
				}
			}
		}

		return CoreItemSP(nullptr);
	}

	virtual inline CoreItemSP operator[](const usString& key) const
	{
		kstl::vector<CoreItemSP>::const_iterator it = myVector.begin();

		while (it != myVector.end())
		{
			if ((*it)->GetType()&(unsigned int)CoreItem::CORENAMEDITEMMASK)
			{
				if (((CoreNamedItem*)(*it).get())->getName() == key.ToString())
				{
					return (*it);
				}
			}
		}

		return CoreItemSP(nullptr);
	}

	virtual void*	getContainerStruct()
	{
		return &myVector;
	}

protected:
	kstl::vector<CoreItemSP>	myVector;
	virtual void    ProtectedDestroy()
	{
		clear();
		CoreItem::ProtectedDestroy();
	}
};

class CoreNamedVector : public CoreVectorBase<CoreNamedItem>
{
public:
	CoreNamedVector(const kstl::string& _name) : CoreVectorBase<CoreNamedItem>(CORENAMEDVECTOR)
	{
		m_Name=_name;
	}
		
	CoreNamedVector() : CoreVectorBase<CoreNamedItem>(CORENAMEDVECTOR)
	{
		m_Name="";
	}


protected:

};

class CoreVector : public CoreVectorBase<CoreItem>
{
public:
	CoreVector():
	CoreVectorBase<CoreItem>(COREVECTOR)
	{
	}


};

#endif // _COREVECTOR_H