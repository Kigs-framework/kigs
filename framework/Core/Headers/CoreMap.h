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

	CoreMapIterator(const CoreItemIteratorBase& other) : CoreItemIteratorBase(other)
	{
		mMapIterator = ((const CoreMapIterator*)&other)->mMapIterator;
	}

	virtual CoreItemIteratorBase* clone()
	{
		CoreMapIterator* result = new CoreMapIterator(mAttachedCoreItem, mPos);
		result->mMapIterator = mMapIterator;
		return result;
	}

	virtual CoreItemSP operator*() const;

	virtual CoreItemIteratorBase& operator=(const CoreItemIteratorBase& other)
	{
		CoreItemIteratorBase::operator=(other);

		mMapIterator = ((CoreMapIterator*)&other)->mMapIterator;

		return *this;
	}

	virtual CoreItemIteratorBase& operator++()
	{
		mMapIterator++;
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

	virtual bool operator==(const CoreItemIteratorBase& other) const
	{
		if (mMapIterator == ((CoreMapIterator*)&other)->mMapIterator)
		{
			return true;
		}
		return false;
	}

	virtual bool operator!=(const CoreItemIteratorBase& other) const
	{
		if (mMapIterator != ((CoreMapIterator*)&other)->mMapIterator)
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

	typename CoreMapMap::iterator	mMapIterator;
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
		mMap.clear();
	}

	// protected, only son classes can access it
	CoreMapBase(CoreItem::COREITEM_TYPE _type):
	baseClass(_type)
	{
		mMap.clear();
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
		typename kstl::map<map_key, CoreItemSP>::iterator it = mMap.find(aKey);
		if (it != mMap.end())
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
		
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(this->SharedFromThis(), 0);
		iter->mMapIterator = mMap.begin();
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}
	

	CoreItemIterator end() override
	{
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(this->SharedFromThis(), 0);
		iter->mMapIterator = mMap.end();
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}
	
	CoreItemIterator find(const map_key& k)
	{
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(this->SharedFromThis(), 0);
		iter->mMapIterator = mMap.find(k);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	CoreItem::size_type size() const  override
	{
		return mMap.size();
	}

	CoreItem::size_type max_size() const
	{
		return mMap.max_size();
	}

	void resize(CoreItem::size_type n)
	{
		mMap.resize(n,0);
	}

	virtual bool empty() const  override
	{
		return mMap.empty();
	}


	const CoreItemSP at (const map_key& n) const
	{
		return mMap.at(n);
	}


	CoreItemIterator erase(CoreItemIterator position)
	{
		CoreMapIterator<map_key>& pos = *(CoreMapIterator<map_key>*)position.get();
		CoreMapIterator<map_key>* iter = new CoreMapIterator<map_key>(this, 0);
		iter->mMapIterator = mMap.erase(pos.mMapIterator);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	CoreItemIterator erase(CoreItemIterator first, CoreItemIterator last)
	{
		CoreMapIterator<map_key> iter = CoreMapIterator<map_key>(this, 0);
		iter.mMapIterator = mMap.erase(((CoreMapIterator<map_key>*)first.get())->mMapIterator, ((CoreMapIterator<map_key>*)last.get())->mMapIterator);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	void clear()
	{
		mMap.clear();
	}



	CoreMapBase& operator= (const CoreMapBase& x)
	{
		mMap.clear();
		for(auto& el : x)
		{
			push_back(el);
		}
		return *this;
	}
	
	virtual void*	getContainerStruct()  override
	{
		return &mMap;
	}

protected:
	CoreMapMap mMap;
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
	virtual inline CoreItemSP operator[](int i) override;

	virtual inline CoreItemSP operator[](const kstl::string& key) override;

	virtual inline CoreItemSP operator[](const usString& key) override;

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
inline CoreItemSP CoreNamedMap<kstl::string>::operator[](const kstl::string& key)
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator it = mMap.find(key);
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreNamedMap<kstl::string>::operator[](const usString& key)
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator it = mMap.find(key.ToString());
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreNamedMap<kstl::string>::operator[](int i)
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator it = mMap.begin();
	int current_index = 0;
	while (current_index < i)
	{
		++it;
		++current_index;
		if (it == mMap.end())
		{
			break;
		}
	}
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}


// specialized usString

template<>
inline CoreItemSP CoreNamedMap<usString>::operator[](const kstl::string& key)
{
	usString usKey(key);
	kstl::map<usString, CoreItemSP>::const_iterator it = mMap.find(usKey);
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreNamedMap<usString>::operator[](const usString& key)
{
	kstl::map<usString, CoreItemSP>::const_iterator it = mMap.find(key);
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreNamedMap<usString>::operator[](int i)
{
	kstl::map<usString, CoreItemSP>::const_iterator it = mMap.begin();
	int current_index = 0;
	while (current_index < i)
	{
		++it;
		++current_index;
		if (it == mMap.end())
		{
			break;
		}
	}
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}


// specialized int
template<>
inline CoreItemSP CoreNamedMap<int>::operator[](const kstl::string& key)
{
	kstl::map<int, CoreItemSP>::const_iterator it = mMap.begin();
	
	while (it != mMap.end())
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
inline CoreItemSP CoreNamedMap<int>::operator[](const usString& key)
{
	kstl::map<int, CoreItemSP>::const_iterator it = mMap.begin();

	while (it != mMap.end())
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
inline CoreItemSP CoreNamedMap<int>::operator[](int i)
{

	kstl::map<int, CoreItemSP>::const_iterator it = mMap.find(i);
	if (it != mMap.end())
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

	mMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<usString>::set(int key, const CoreItemSP& toinsert)
{
	usString goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = usString(intstr);

	mMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<int>::set(int key, const CoreItemSP& toinsert)
{
	int goodType = key;
	mMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<kstl::string>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	kstl::string goodType=key;
	mMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<usString>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	usString goodType=key;
	mMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<int>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	int goodType = 0;
	sscanf(key.c_str(), "%d", &goodType);
	mMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<kstl::string>::set(const usString& key, const CoreItemSP& toinsert)
{
	kstl::string goodType = key.ToString();
	mMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<usString>::set(const usString& key, const CoreItemSP& toinsert)
{
	usString goodType = key;
	mMap[goodType] = toinsert;
}

template<>
inline void CoreNamedMap<int>::set(const usString& key, const CoreItemSP& toinsert)
{
	kstl::string strkey = key.ToString();
	int goodType = 0;
	sscanf(strkey.c_str(), "%d", &goodType);
	mMap[goodType] = toinsert;
}

// specialised erase
template<>
inline void CoreNamedMap<kstl::string>::erase(int key)
{
	kstl::string goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = intstr;

	auto it=mMap.find(goodType);
	if(it!= mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreNamedMap<usString>::erase(int key)
{
	usString goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = usString(intstr);

	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreNamedMap<int>::erase(int key)
{
	int goodType = key;
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreNamedMap<kstl::string>::erase(const kstl::string& key)
{
	kstl::string goodType = key;
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreNamedMap<usString>::erase(const kstl::string& key)
{
	usString goodType = key;
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreNamedMap<int>::erase(const kstl::string& key)
{
	int goodType = 0;
	sscanf(key.c_str(), "%d", &goodType);
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreNamedMap<kstl::string>::erase(const usString& key)
{
	kstl::string goodType = key.ToString();
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreNamedMap<usString>::erase(const usString& key)
{
	usString goodType = key;
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreNamedMap<int>::erase(const usString& key)
{
	kstl::string strkey = key.ToString();
	int goodType = 0;
	sscanf(strkey.c_str(), "%d", &goodType);
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
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
	virtual inline CoreItemSP operator[](int i) override;
	virtual inline CoreItemSP operator[](const kstl::string& key) override;
	virtual inline CoreItemSP operator[](const usString& key) override;

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

	if (mMapIterator != mapstruct.end())
	{
		return (*mMapIterator).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline bool	 CoreMapIterator<kstl::string>::getKey(kstl::string& returnedkey)
{
	returnedkey = (*mMapIterator).first;
	return true;
}

template<>
inline bool	 CoreMapIterator<kstl::string>::getKey(usString& returnedkey)
{
	returnedkey = (*mMapIterator).first;
	return true;
}


template<>
inline CoreItemSP CoreMap<kstl::string>::operator[](const kstl::string& key)
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator it = mMap.find(key);
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreMap<kstl::string>::operator[](const usString& key)
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator  it = mMap.find(key.ToString());
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreMap<kstl::string>::operator[](int i)
{
	kstl::map<kstl::string, CoreItemSP>::const_iterator  it = mMap.begin();
	int current_index = 0;
	while (current_index < i)
	{
		++it;
		++current_index;
		if (it == mMap.end())
		{
			break;
		}
	}
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}


// specialized usString

template<>
inline CoreItemSP CoreMapIterator<usString>::operator*() const
{
	if (mMapIterator != ((CoreMap<usString>*)mAttachedCoreItem.get())->mMap.end())
	{
		return ((*mMapIterator).second);
	}

	return CoreItemSP(nullptr);
}

template<>
inline bool	 CoreMapIterator<usString>::getKey(kstl::string& returnedkey)
{
	returnedkey = (*mMapIterator).first.ToString();
	return true;
}

template<>
inline bool	 CoreMapIterator<usString>::getKey(usString& returnedkey)
{
	returnedkey = (*mMapIterator).first;
	return true;
}



template<>
inline CoreItemSP CoreMap<usString>::operator[](const kstl::string& key)
{
	usString usKey(key);
	kstl::map<usString, CoreItemSP>::const_iterator it = mMap.find(usKey);
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreMap<usString>::operator[](const usString& key)
{
	kstl::map<usString, CoreItemSP>::const_iterator it = mMap.find(key);
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}

template<>
inline CoreItemSP CoreMap<usString>::operator[](int i)
{
	kstl::map<usString, CoreItemSP>::const_iterator it = mMap.begin();
	int current_index = 0;
	while (current_index < i)
	{
		++it;
		++current_index;
		if (it == mMap.end())
		{
			break;
		}
	}
	if (it != mMap.end())
	{
		return (*it).second;
	}

	return CoreItemSP(nullptr);
}




// specialized int
template<>
inline CoreItemSP CoreMap<int>::operator[](const kstl::string& key)
{
	kstl::map<int, CoreItemSP>::const_iterator it = mMap.begin();

	while (it != mMap.end())
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
inline CoreItemSP CoreMap<int>::operator[](const usString& key)
{
	kstl::map<int, CoreItemSP>::const_iterator it = mMap.begin();

	while (it != mMap.end())
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
inline CoreItemSP CoreMap<int>::operator[](int i)
{

	kstl::map<int, CoreItemSP>::const_iterator it = mMap.find(i);
	if (it != mMap.end())
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

	mMap[goodType] = toinsert;
}

template<>
inline void CoreMap<usString>::set(int key, const CoreItemSP& toinsert)
{
	usString goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = usString(intstr);

	mMap[goodType] = toinsert;
}

template<>
inline void CoreMap<int>::set(int key, const CoreItemSP& toinsert)
{
	int goodType = key;
	mMap[goodType] = toinsert;
}

template<>
inline void CoreMap<kstl::string>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	kstl::string goodType = key;
	mMap[goodType] = toinsert;
}

template<>
inline void CoreMap<usString>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	usString goodType = key;
	mMap[goodType] = toinsert;
}

template<>
inline void CoreMap<int>::set(const kstl::string& key, const CoreItemSP& toinsert)
{
	int goodType = 0;
	sscanf(key.c_str(), "%d", &goodType);
	mMap[goodType] = toinsert;
}

template<>
inline void CoreMap<kstl::string>::set(const usString& key, const CoreItemSP& toinsert)
{
	kstl::string goodType = key.ToString();
	mMap[goodType] = toinsert;
}

template<>
inline void CoreMap<usString>::set(const usString& key, const CoreItemSP& toinsert)
{
	usString goodType = key;
	mMap[goodType] = toinsert;
}

template<>
inline void CoreMap<int>::set(const usString& key, const CoreItemSP& toinsert)
{
	kstl::string strkey = key.ToString();
	int goodType = 0;
	sscanf(strkey.c_str(), "%d", &goodType);
	mMap[goodType] = toinsert;
}

// specialised erase
template<>
inline void CoreMap<kstl::string>::erase(int key)
{
	kstl::string goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = intstr;

	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreMap<usString>::erase(int key)
{
	usString goodType;

	char intstr[64];
	sprintf(intstr, "%d", key);
	goodType = usString(intstr);

	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreMap<int>::erase(int key)
{
	int goodType = key;
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreMap<kstl::string>::erase(const kstl::string& key)
{
	kstl::string goodType = key;
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreMap<usString>::erase(const kstl::string& key)
{
	usString goodType = key;
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreMap<int>::erase(const kstl::string& key)
{
	int goodType = 0;
	sscanf(key.c_str(), "%d", &goodType);
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreMap<kstl::string>::erase(const usString& key)
{
	kstl::string goodType = key.ToString();
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreMap<usString>::erase(const usString& key)
{
	usString goodType = key;
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}

template<>
inline void CoreMap<int>::erase(const usString& key)
{
	kstl::string strkey = key.ToString();
	int goodType = 0;
	sscanf(strkey.c_str(), "%d", &goodType);
	auto it = mMap.find(goodType);
	if (it != mMap.end())
		mMap.erase(it);
}


#endif // _COREMAP_H