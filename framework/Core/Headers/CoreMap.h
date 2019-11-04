#ifndef _COREMAP_H
#define _COREMAP_H

#include "CoreItem.h"
#include "Core.h"
#include "kstlmap.h"

// ****************************************
// * CoreMap class
// * --------------------------------------
/**
* \class	CoreMap
* \ingroup KigsCore
* \brief	a map of RefCountedBaseClass, maintain ref count of instances in map
* \author	ukn
* \version ukn
* \date	ukn
*/
// ****************************************


template<class map_key, class baseClass>
class CoreMapBase;

template<class map_key>
class CoreMapIterator : public CoreItemIteratorBase
{
public:

	CoreMapIterator(const CoreItemIteratorBase & other) : CoreItemIteratorBase(other)
	{
		myMapIterator = ((const CoreMapIterator*)&other)->myMapIterator;
	}

	virtual CoreItemIteratorBase*	clone()
	{
		CoreMapIterator*	result = new CoreMapIterator(myAttachedCoreItem, myPos);
		result->myMapIterator = myMapIterator;
		return result;
	}

	virtual CoreItem* operator*() const;

	virtual CoreItemIteratorBase& operator=(const CoreItemIteratorBase & other)
	{
		CoreItemIteratorBase::operator=(other);

		myMapIterator = ((CoreMapIterator*)&other)->myMapIterator;

		return *this;
	}

	virtual CoreItemIteratorBase& operator++()
	{
		myMapIterator++;
		return *this;
	}

	virtual CoreItemIteratorBase& operator+(const int decal)
	{
		// ERROR ?
		return *this;
	}

	virtual CoreItemIteratorBase operator++(int)
	{
		CoreMapIterator	tmp(*this);
		operator++();
		return tmp;
	}

	virtual bool operator==(const CoreItemIteratorBase & other) const
	{
		if (myMapIterator == ((CoreMapIterator*)&other)->myMapIterator)
		{
			return true;
		}
		return false;
	}

	virtual bool operator!=(const CoreItemIteratorBase & other) const
	{
		if (myMapIterator != ((CoreMapIterator*)&other)->myMapIterator)
		{
			return true;
		}
		return false;
	}

	// to specialize
	virtual bool	getKey(kstl::string& returnedkey);
	virtual bool	getKey(usString& returnedkey);

	CoreMapIterator(CoreItem* item, unsigned int pos) : CoreItemIteratorBase(item,pos)
	{

	}

	typedef kstl::map<map_key, RefCountedBaseClass*>	CoreMapMap;

	typename CoreMapMap::iterator	myMapIterator;
};



template<class map_key,class baseClass>
class CoreMapBase : public baseClass
{
protected:
	//typedef kstl::pair<map_key, RefCountedBaseClass*> CoreMapPair;
	virtual ~CoreMapBase()
	{
		clear();
	}

	explicit CoreMapBase():
	baseClass(CoreItem::COREMAP)
	{
		myMap.clear();
	}

	// protected, only son classes can access it
	CoreMapBase(CoreItem::COREITEM_TYPE _type):
	baseClass(_type)
	{
		myMap.clear();
	}


	typedef kstl::map<map_key, RefCountedBaseClass*>	CoreMapMap;

	
public:

	baseClass * GetItem(map_key aKey)
	{
		typename kstl::map<map_key, RefCountedBaseClass*>::iterator it = myMap.find(aKey);
		if (it != myMap.end())
		{
			return ((baseClass*)it->second);
		}
		else
		{
			return NULL;
		}
	}
	
	friend class CoreMapIterator<map_key>;
	
	// wrapper on member vector
	CoreItemIterator	 begin()
	{
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(this, 0);
		iter->myMapIterator = myMap.begin();
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}
	

	CoreItemIterator end()
	{
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(this, 0);
		iter->myMapIterator = myMap.end();
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}
	
	CoreItemIterator find(const map_key& k)
	{
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(this, 0);
		iter->myMapIterator = myMap.find(k);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	CoreItem::size_type size() const
	{
		return myMap.size();
	}

	CoreItem::size_type max_size() const
	{
		return myMap.max_size();
	}

	void resize(CoreItem::size_type n)
	{
		myMap.resize(n,0);
	}

	virtual bool empty() const
	{
		return myMap.empty();
	}


	const RefCountedBaseClass* at (const map_key& n) const
	{
		return myMap.at(n);
	}


	CoreItemIterator erase(CoreItemIterator position)
	{
		CoreMapIterator<map_key>& pos = *(CoreMapIterator<map_key>*)position.myPointer;
		if ((*(pos.myMapIterator)).second)
		{
			((*(pos.myMapIterator)).second)->Destroy();
		}

		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(this, 0);
		iter->myMapIterator = myMap.erase(pos.myMapIterator);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	CoreItemIterator erase(CoreItemIterator first, CoreItemIterator last)
	{
		CoreMapIterator<map_key>& start = *(CoreMapIterator<map_key>*)first.myPointer;
		
		while (start != last)
		{
			if ((*(start.myMapIterator)).second)
			{
				(*(start.myMapIterator)).second->Destroy();
			}
			++start;
		}

		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(this, 0);
		iter->myMapIterator = myMap.erase(((CoreMapIterator<map_key>*)first.myPointer)->myMapIterator, ((CoreMapIterator<map_key>*)last.myPointer)->myMapIterator);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	void clear()
	{
		typename kstl::map<map_key, RefCountedBaseClass*>::iterator	start = myMap.begin();
		typename kstl::map<map_key, RefCountedBaseClass*>::iterator	end = myMap.end();
		while(start!=end)
		{
			((*start).second)->Destroy();
			++start;
		}
		myMap.clear();
	}

	void set (map_key key,RefCountedBaseClass* toinsert )
	{
		RefCountedBaseClass* toDestroy=0;
		typename kstl::map<map_key, RefCountedBaseClass*>::iterator found = myMap.find(key);

		if(found != myMap.end())
		{
			toDestroy=(*found).second;
		}
		toinsert->GetRef();
		myMap[key]=toinsert;
		if(toDestroy)
		{
			toDestroy->Destroy();
		}
	}

	CoreMapBase& operator= (const CoreMapBase& x)
	{
		// copy current vector
		kstl::map<map_key,RefCountedBaseClass*>	todestroy=myMap;
		myMap.clear();
		typename kstl::map<map_key, RefCountedBaseClass*>::iterator itstart;
		typename kstl::map<map_key, RefCountedBaseClass*>::iterator itend = x.end();

		for(itstart=x.begin();itstart!=itend;itstart++)
		{
			push_back(*itstart);
		}

		itend=todestroy.end();
		for(itstart=todestroy.begin();itstart!=itend;itstart++)
		{
			((*itstart).second)->Destroy();
		}
		return *this;
	}
	
	virtual void*	getContainerStruct()
	{
		return &myMap;
	}

protected:
	CoreMapMap myMap;
	virtual void    ProtectedDestroy()
	{
		clear();
		CoreItem::ProtectedDestroy();
	}
};

template<class map_key>
class CoreNamedMap : public CoreMapBase<map_key,CoreNamedItem>
{
public:
	CoreNamedMap(const kstl::string& _name) : CoreMapBase<map_key,CoreNamedItem>(CoreItem::CORENAMEDMAP)
	{
		CoreNamedItem::m_Name=_name;
	}

	virtual inline CoreItem& operator[](int i) const;

	virtual inline CoreItem& operator[](const kstl::string& key) const;

	virtual inline CoreItem& operator[](const usString& key) const;

protected:

};

// specialized kstl::string



template<>
inline CoreItem& CoreNamedMap<kstl::string>::operator[](const kstl::string& key) const
{
	kstl::map<kstl::string, RefCountedBaseClass*>::const_iterator it = myMap.find(key);
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreNamedMap<kstl::string>::operator[](const usString& key) const
{
	kstl::map<kstl::string, RefCountedBaseClass*>::const_iterator it = myMap.find(key.ToString());
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreNamedMap<kstl::string>::operator[](int i) const
{
	kstl::map<kstl::string, RefCountedBaseClass*>::const_iterator it = myMap.begin();
	int current_index = 0;
	while (current_index < i)
	{
		++it;
		++current_index;
		if (it == myMap.end())
		{
			break;
		}
	}
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}


// specialized usString

template<>
inline CoreItem& CoreNamedMap<usString>::operator[](const kstl::string& key) const 
{
	usString usKey(key);
	kstl::map<usString, RefCountedBaseClass*>::const_iterator it = myMap.find(usKey);
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreNamedMap<usString>::operator[](const usString& key) const
{
	kstl::map<usString, RefCountedBaseClass*>::const_iterator it = myMap.find(key);
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreNamedMap<usString>::operator[](int i) const
{
	kstl::map<usString, RefCountedBaseClass*>::const_iterator it = myMap.begin();
	int current_index = 0;
	while (current_index < i)
	{
		++it;
		++current_index;
		if (it == myMap.end())
		{
			break;
		}
	}
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}


// specialized int
template<>
inline CoreItem& CoreNamedMap<int>::operator[](const kstl::string& key) const
{
	kstl::map<int, RefCountedBaseClass*>::const_iterator it = myMap.begin();
	
	while (it != myMap.end())
	{
		if (((CoreItem*)(*it).second)->GetType()&(unsigned int)CORENAMEDITEMMASK)
		{
			if (((CoreNamedItem*)(*it).second)->getName() == key)
			{
				return *((CoreItem*)(*it).second);
			}
		}
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreNamedMap<int>::operator[](const usString& key) const
{
	kstl::map<int, RefCountedBaseClass*>::const_iterator it = myMap.begin();

	while (it != myMap.end())
	{
		if (((CoreItem*)(*it).second)->GetType()&(unsigned int)CORENAMEDITEMMASK)
		{
			if (((CoreNamedItem*)(*it).second)->getName() == key.ToString())
			{
				return *((CoreItem*)(*it).second);
			}
		}
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreNamedMap<int>::operator[](int i) const
{

	kstl::map<int, RefCountedBaseClass*>::const_iterator it = myMap.find(i);
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<class map_key>
class CoreMap : public CoreMapBase<map_key,CoreItem>
{
public:
	CoreMap():
	CoreMapBase<map_key,CoreItem>(CoreItem::COREMAP)
	{
	}

	// Specialised
	virtual inline CoreItem& operator[](int i) const;
	virtual inline CoreItem& operator[](const kstl::string& key) const;
	virtual inline CoreItem& operator[](const usString& key) const;
	
};

// specialized kstl::string

template<>
inline CoreItem* CoreMapIterator<kstl::string>::operator*() const
{

	kstl::map<kstl::string, RefCountedBaseClass*>&	mapstruct = *(kstl::map<kstl::string, RefCountedBaseClass*>*)myAttachedCoreItem->getContainerStruct();

	if (myMapIterator != mapstruct.end())
	{
		return ((CoreItem*)(*myMapIterator).second);
	}

	return KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline bool	 CoreMapIterator<kstl::string>::getKey(kstl::string& returnedkey)
{
	returnedkey = (*myMapIterator).first;
	return true;
}

template<>
inline bool	 CoreMapIterator<kstl::string>::getKey(usString& returnedkey)
{
	returnedkey = (*myMapIterator).first;
	return true;
}


template<>
inline CoreItem& CoreMap<kstl::string>::operator[](const kstl::string& key)  const
{
	kstl::map<kstl::string, RefCountedBaseClass*>::const_iterator it = myMap.find(key);
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreMap<kstl::string>::operator[](const usString& key)  const
{
	kstl::map<kstl::string, RefCountedBaseClass*>::const_iterator  it = myMap.find(key.ToString());
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreMap<kstl::string>::operator[](int i)  const
{
	kstl::map<kstl::string, RefCountedBaseClass*>::const_iterator  it = myMap.begin();
	int current_index = 0;
	while (current_index < i)
	{
		++it;
		++current_index;
		if (it == myMap.end())
		{
			break;
		}
	}
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}


// specialized usString

template<>
inline CoreItem* CoreMapIterator<usString>::operator*() const
{
	if (myMapIterator != ((CoreMap<usString>*)myAttachedCoreItem)->myMap.end())
	{
		return ((CoreItem*)(*myMapIterator).second);
	}

	return KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline bool	 CoreMapIterator<usString>::getKey(kstl::string& returnedkey)
{
	returnedkey = (*myMapIterator).first.ToString();
	return true;
}

template<>
inline bool	 CoreMapIterator<usString>::getKey(usString& returnedkey)
{
	returnedkey = (*myMapIterator).first;
	return true;
}



template<>
inline CoreItem& CoreMap<usString>::operator[](const kstl::string& key)  const
{
	usString usKey(key);
	kstl::map<usString, RefCountedBaseClass*>::const_iterator it = myMap.find(usKey);
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreMap<usString>::operator[](const usString& key)  const
{
	kstl::map<usString, RefCountedBaseClass*>::const_iterator it = myMap.find(key);
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreMap<usString>::operator[](int i)  const
{
	kstl::map<usString, RefCountedBaseClass*>::const_iterator it = myMap.begin();
	int current_index = 0;
	while (current_index < i)
	{
		++it;
		++current_index;
		if (it == myMap.end())
		{
			break;
		}
	}
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}




// specialized int
template<>
inline CoreItem& CoreMap<int>::operator[](const kstl::string& key)  const
{
	kstl::map<int, RefCountedBaseClass*>::const_iterator it = myMap.begin();

	while (it != myMap.end())
	{
		if (((CoreItem*)(*it).second)->GetType()&(unsigned int)CORENAMEDITEMMASK)
		{
			if (((CoreNamedItem*)(*it).second)->getName() == key)
			{
				return *((CoreItem*)(*it).second);
			}
		}
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreMap<int>::operator[](const usString& key)  const
{
	kstl::map<int, RefCountedBaseClass*>::const_iterator it = myMap.begin();

	while (it != myMap.end())
	{
		if (((CoreItem*)(*it).second)->GetType()&(unsigned int)CORENAMEDITEMMASK)
		{
			if (((CoreNamedItem*)(*it).second)->getName() == key.ToString())
			{
				return *((CoreItem*)(*it).second);
			}
		}
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}

template<>
inline CoreItem& CoreMap<int>::operator[](int i)  const
{

	kstl::map<int, RefCountedBaseClass*>::const_iterator it = myMap.find(i);
	if (it != myMap.end())
	{
		return *((CoreItem*)(*it).second);
	}

	return *KigsCore::Instance()->NotFoundCoreItem();
}


#endif // _COREMAP_H