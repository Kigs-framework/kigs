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
* \ingroup Core
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
		CoreMapIterator*	result = new CoreMapIterator(mAttachedCoreItem, mPos);
		result->myMapIterator = myMapIterator;
		return result;
	}

	virtual CoreItemSP operator*() const;

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

	CoreMapIterator(const CoreItemSP& item, unsigned int pos) : CoreItemIteratorBase(item,pos)
	{

	}

	typedef kstl::map<map_key, CoreItemSP>	CoreMapMap;

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


	typedef kstl::map<map_key, CoreItemSP>	CoreMapMap;

	virtual void set(int key, const CoreItemSP& toinsert) override
	{
		KIGS_ERROR("set called on CoreMapBase", 2);
	}
	virtual void set(const kstl::string& key, const CoreItemSP& toinsert) override
	{
		KIGS_ERROR("set called on CoreMapBase", 2);
	}
	virtual void set(const usString& key, const CoreItemSP& toinsert) override
	{
		KIGS_ERROR("set called on CoreMapBase", 2);
	}

	virtual void erase(int key) override
	{
		KIGS_ERROR("erase called on CoreMapBase", 2);
	}
	virtual void erase(const kstl::string& key) override
	{
		KIGS_ERROR("erase called on CoreMapBase", 2);
	}
	virtual void erase(const usString& key) override
	{
		KIGS_ERROR("erase called on CoreMapBase", 2);
	}

	
public:

	CoreItemSP GetItem(map_key aKey)
	{
		typename kstl::map<map_key, CoreItemSP>::iterator it = myMap.find(aKey);
		if (it != myMap.end())
		{
			return it->second;
		}
		else
		{
			return CoreItemSP(nullptr);
		}
	}
	
	friend class CoreMapIterator<map_key>;
	
	// wrapper on member vector
	CoreItemIterator	 begin() override
	{
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(CoreItemSP(this, GetRefTag{}), 0);
		iter->myMapIterator = myMap.begin();
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}
	

	CoreItemIterator end() override
	{
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(CoreItemSP(this, GetRefTag{}), 0);
		iter->myMapIterator = myMap.end();
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}
	
	CoreItemIterator find(const map_key& k)
	{
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(CoreItemSP(this, GetRefTag{}), 0);
		iter->myMapIterator = myMap.find(k);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	CoreItem::size_type size() const  override
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

	virtual bool empty() const  override
	{
		return myMap.empty();
	}


	const CoreItemSP at (const map_key& n) const
	{
		return myMap.at(n);
	}


	CoreItemIterator erase(CoreItemIterator position)
	{
		CoreMapIterator<map_key>& pos = *(CoreMapIterator<map_key>*)position.Pointer();
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(this, 0);
		iter->myMapIterator = myMap.erase(pos.myMapIterator);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	CoreItemIterator erase(CoreItemIterator first, CoreItemIterator last)
	{
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(this, 0);
		iter->myMapIterator = myMap.erase(((CoreMapIterator<map_key>*)first.mPointer)->myMapIterator, ((CoreMapIterator<map_key>*)last.mPointer)->myMapIterator);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	void clear()
	{
		myMap.clear();
	}



	CoreMapBase& operator= (const CoreMapBase& x)
	{
		myMap.clear();
		for(auto& el : x)
		{
			push_back(el);
		}
		return *this;
	}
	
	virtual void*	getContainerStruct()  override
	{
		return &myMap;
	}

protected:
	CoreMapMap myMap;
};


template<class map_key>
class CoreNamedMap : public CoreMapBase<map_key,CoreNamedItem>
{
public:
	CoreNamedMap(const kstl::string& _name) : CoreMapBase<map_key,CoreNamedItem>(CoreItem::CORENAMEDMAP)
	{
		CoreNamedItem::mName=_name;
	}
	inline CoreItemSP operator[](const char* key) const
	{
		return CoreItem::operator[](key);
	}
	virtual inline CoreItemSP operator[](int i) const  override;

	virtual inline CoreItemSP operator[](const kstl::string& key) const  override;

	virtual inline CoreItemSP operator[](const usString& key) const  override;

	virtual inline void set(int key, const CoreItemSP& toinsert) override;

	virtual inline void set(const kstl::string& key, const CoreItemSP& toinsert) override;

	virtual inline void set(const usString& key, const CoreItemSP& toinsert) override;

	virtual inline void erase(int key) override;

	virtual inline void erase(const kstl::string& key) override;

	virtual inline void erase(const usString& key) override;


protected:

};

// specialized kstl::string



template<>
inline CoreItemSP CoreNamedMap<kstl::string>::operator[](const kstl::string& key) const
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator it = myMap.find(key);
	if (it != myMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreNamedMap<kstl::string>::operator[](const usString& key) const
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator it = myMap.find(key.ToString());
	if (it != myMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreNamedMap<kstl::string>::operator[](int i) const
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator it = myMap.begin();
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
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}


// specialized usString

template<>
inline CoreItemSP CoreNamedMap<usString>::operator[](const kstl::string& key) const
{
	usString usKey(key);
	kstl::map<usString, CoreItemSP>::const_iterator it = myMap.find(usKey);
	if (it != myMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreNamedMap<usString>::operator[](const usString& key) const
{
	kstl::map<usString, CoreItemSP>::const_iterator it = myMap.find(key);
	if (it != myMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreNamedMap<usString>::operator[](int i) const
{
	kstl::map<usString, CoreItemSP>::const_iterator it = myMap.begin();
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
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}


// specialized int
template<>
inline CoreItemSP CoreNamedMap<int>::operator[](const kstl::string& key) const
{
	kstl::map<int, CoreItemSP>::const_iterator it = myMap.begin();
	
	while (it != myMap.end())
	{
		if (((*it).second)->GetType()&(unsigned int)CORENAMEDITEMMASK)
		{
			if (((CoreNamedItem*)((*it).second).get())->getName() == key)
			{
				return (*it).second;
			}
		}
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreNamedMap<int>::operator[](const usString& key) const
{
	kstl::map<int, CoreItemSP>::const_iterator it = myMap.begin();

	while (it != myMap.end())
	{
		if (((*it).second)->GetType()&(unsigned int)CORENAMEDITEMMASK)
		{
			if (((CoreNamedItem*)(*it).second.get())->getName() == key.ToString())
			{
				return (*it).second;
			}
		}
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreNamedMap<int>::operator[](int i) const
{

	kstl::map<int, CoreItemSP>::const_iterator it = myMap.find(i);
	if (it != myMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

// specialised set
template<>
inline void CoreNamedMap<kstl::string>::set(int key, const CoreItemSP& toinsert)
{
	kstl::string goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = intstr;

	myMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<usString>::set(int key, const CoreItemSP& toinsert)
{
	usString goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = usString(intstr);

	myMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<int>::set(int key, const CoreItemSP& toinsert)
{
	int goodType = key;
	myMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<kstl::string>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	kstl::string goodType=key;
	myMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<usString>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	usString goodType=key;
	myMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<int>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	int goodType = 0;
	sscanf(key.c_str(), "%d", &goodType);
	myMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<kstl::string>::set(const usString& key, const CoreItemSP& toinsert)
{
	kstl::string goodType = key.ToString();
	myMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<usString>::set(const usString& key, const CoreItemSP& toinsert)
{
	usString goodType = key;
	myMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<int>::set(const usString& key, const CoreItemSP& toinsert)
{
	kstl::string strkey = key.ToString();
	int goodType = 0;
	sscanf(strkey.c_str(), "%d", &goodType);
	myMap[goodType] = toinsert;
}

// specialised erase
template<>
inline void CoreNamedMap<kstl::string>::erase(int key)
{
	kstl::string goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = intstr;

	auto it=myMap.find(goodType);
	if(it!= myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreNamedMap<usString>::erase(int key)
{
	usString goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = usString(intstr);

	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreNamedMap<int>::erase(int key)
{
	int goodType = key;
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreNamedMap<kstl::string>::erase(const kstl::string& key)
{
	kstl::string goodType = key;
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreNamedMap<usString>::erase(const kstl::string& key)
{
	usString goodType = key;
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreNamedMap<int>::erase(const kstl::string& key)
{
	int goodType = 0;
	sscanf(key.c_str(), "%d", &goodType);
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreNamedMap<kstl::string>::erase(const usString& key)
{
	kstl::string goodType = key.ToString();
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreNamedMap<usString>::erase(const usString& key)
{
	usString goodType = key;
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreNamedMap<int>::erase(const usString& key)
{
	kstl::string strkey = key.ToString();
	int goodType = 0;
	sscanf(strkey.c_str(), "%d", &goodType);
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}


template<class map_key>
class CoreMap : public CoreMapBase<map_key,CoreItem>
{
public:
	CoreMap():
	CoreMapBase<map_key,CoreItem>(CoreItem::COREMAP)
	{
	}
	inline CoreItemSP operator[](const char* key) const
	{
		return CoreItem::operator[](key);
	}
	// Specialised
	virtual inline CoreItemSP operator[](int i) const override;
	virtual inline CoreItemSP operator[](const kstl::string& key) const override;
	virtual inline CoreItemSP operator[](const usString& key) const override;

	virtual inline void set(int key, const CoreItemSP& toinsert) override;

	virtual inline void set(const kstl::string& key, const CoreItemSP& toinsert) override;

	virtual inline void set(const usString& key, const CoreItemSP& toinsert) override;
	
	virtual inline void erase(int key) override;

	virtual inline void erase(const kstl::string& key) override;

	virtual inline void erase(const usString& key) override;
};

// specialized kstl::string

template<>
inline CoreItemSP CoreMapIterator<kstl::string>::operator*() const
{

	kstl::map<kstl::string, CoreItemSP>&	mapstruct = *(kstl::map<kstl::string, CoreItemSP>*)mAttachedCoreItem->getContainerStruct();

	if (myMapIterator != mapstruct.end())
	{
		return (*myMapIterator).second;
	}

	return CoreItemSP(nullptr);
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
inline CoreItemSP CoreMap<kstl::string>::operator[](const kstl::string& key)  const
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator it = myMap.find(key);
	if (it != myMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreMap<kstl::string>::operator[](const usString& key)  const
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator  it = myMap.find(key.ToString());
	if (it != myMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreMap<kstl::string>::operator[](int i)  const
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator  it = myMap.begin();
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
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}


// specialized usString

template<>
inline CoreItemSP CoreMapIterator<usString>::operator*() const
{
	if (myMapIterator != ((CoreMap<usString>*)mAttachedCoreItem.get())->myMap.end())
	{
		return ((*myMapIterator).second);
	}

	return CoreItemSP(nullptr);
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
inline CoreItemSP CoreMap<usString>::operator[](const kstl::string& key)  const
{
	usString usKey(key);
	kstl::map<usString, CoreItemSP>::const_iterator it = myMap.find(usKey);
	if (it != myMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreMap<usString>::operator[](const usString& key)  const
{
	kstl::map<usString, CoreItemSP>::const_iterator it = myMap.find(key);
	if (it != myMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreMap<usString>::operator[](int i)  const
{
	kstl::map<usString, CoreItemSP>::const_iterator it = myMap.begin();
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
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}




// specialized int
template<>
inline CoreItemSP CoreMap<int>::operator[](const kstl::string& key)  const
{
	kstl::map<int, CoreItemSP>::const_iterator it = myMap.begin();

	while (it != myMap.end())
	{
		if (((*it).second)->GetType()&(unsigned int)CORENAMEDITEMMASK)
		{
			if (((CoreNamedItem*)(*it).second.get())->getName() == key)
			{
				return (*it).second;
			}
		}
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreMap<int>::operator[](const usString& key)  const
{
	kstl::map<int, CoreItemSP>::const_iterator it = myMap.begin();

	while (it != myMap.end())
	{
		if (((*it).second)->GetType()&(unsigned int)CORENAMEDITEMMASK)
		{
			if (((CoreNamedItem*)(*it).second.get())->getName() == key.ToString())
			{
				return (*it).second;
			}
		}
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreMap<int>::operator[](int i)  const
{

	kstl::map<int, CoreItemSP>::const_iterator it = myMap.find(i);
	if (it != myMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}


// specialised set
template<>
inline void CoreMap<kstl::string>::set(int key, const CoreItemSP& toinsert)
{
	kstl::string goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = intstr;

	myMap[goodType] = toinsert;
}

template<>
inline void CoreMap<usString>::set(int key, const CoreItemSP& toinsert)
{
	usString goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = usString(intstr);

	myMap[goodType] = toinsert;
}

template<>
inline void CoreMap<int>::set(int key, const CoreItemSP& toinsert)
{
	int goodType = key;
	myMap[goodType] = toinsert;
}

template<>
inline void CoreMap<kstl::string>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	kstl::string goodType = key;
	myMap[goodType] = toinsert;
}

template<>
inline void CoreMap<usString>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	usString goodType = key;
	myMap[goodType] = toinsert;
}

template<>
inline void CoreMap<int>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	int goodType = 0;
	sscanf(key.c_str(), "%d", &goodType);
	myMap[goodType] = toinsert;
}

template<>
inline void CoreMap<kstl::string>::set(const usString& key, const CoreItemSP& toinsert)
{
	kstl::string goodType = key.ToString();
	myMap[goodType] = toinsert;
}

template<>
inline void CoreMap<usString>::set(const usString& key, const CoreItemSP& toinsert)
{
	usString goodType = key;
	myMap[goodType] = toinsert;
}

template<>
inline void CoreMap<int>::set(const usString& key, const CoreItemSP& toinsert)
{
	kstl::string strkey = key.ToString();
	int goodType = 0;
	sscanf(strkey.c_str(), "%d", &goodType);
	myMap[goodType] = toinsert;
}

// specialised erase
template<>
inline void CoreMap<kstl::string>::erase(int key)
{
	kstl::string goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = intstr;

	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreMap<usString>::erase(int key)
{
	usString goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = usString(intstr);

	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreMap<int>::erase(int key)
{
	int goodType = key;
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreMap<kstl::string>::erase(const kstl::string& key)
{
	kstl::string goodType = key;
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreMap<usString>::erase(const kstl::string& key)
{
	usString goodType = key;
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreMap<int>::erase(const kstl::string& key)
{
	int goodType = 0;
	sscanf(key.c_str(), "%d", &goodType);
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreMap<kstl::string>::erase(const usString& key)
{
	kstl::string goodType = key.ToString();
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreMap<usString>::erase(const usString& key)
{
	usString goodType = key;
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}

template<>
inline void CoreMap<int>::erase(const usString& key)
{
	kstl::string strkey = key.ToString();
	int goodType = 0;
	sscanf(strkey.c_str(), "%d", &goodType);
	auto it = myMap.find(goodType);
	if (it != myMap.end())
		myMap.erase(it);
}


#endif // _COREMAP_H