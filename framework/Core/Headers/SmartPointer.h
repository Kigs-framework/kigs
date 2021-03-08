#ifndef _SMARTPOINTER_H_
#define _SMARTPOINTER_H_

/*! smart pointer on GenericRefCountedBaseClass mecanism
useful when you want to auto manage retain / release on elements that you don't want to "addItem"
*/

// Tag Dispatch
struct GetRefTag {};
struct StealRefTag {};


template<typename smartPointOn>
class SmartPointer
{
public:
	using ValueType = smartPointOn;
	using IsSP = std::true_type;
	
	SmartPointer() : mPointer(nullptr) {};
	SmartPointer(std::nullptr_t) : SmartPointer() {};

	// DOES NOT GET REF!
	// SmartPointer(smartPointOn* point) : mPointer(point) {};
	
	SmartPointer(smartPointOn* point, StealRefTag stealref) : mPointer(point) {};
	SmartPointer(smartPointOn* point, GetRefTag getref) : mPointer(point) { if(mPointer) mPointer->GetRef(); };

	~SmartPointer()
	{
		if (mPointer)
		{
			mPointer->Destroy();
			mPointer = nullptr;
		}
	}

	template<typename smartPointOnOther>
	SmartPointer(const SmartPointer<smartPointOnOther>& smcopy) : mPointer(nullptr)
	{
		*this = smcopy;
	}

	SmartPointer(const SmartPointer& other) : mPointer(nullptr)
	{
		*this = other;
	}

	// Move constructor, steal other ref
	SmartPointer(SmartPointer&& other) noexcept
	{
		mPointer = other.mPointer;
		other.mPointer = nullptr;
	}


	smartPointOn*	operator->() const {
		return mPointer;
	}

	bool	isNil() const
	{
		return mPointer == nullptr;
	}

	/*
	SmartPointer& operator=(smartPointOn* copy)
	{
		if (mPointer != copy)
		{
			if (mPointer)
			{
				mPointer->Destroy();
			}
			mPointer = copy;
			if (mPointer)
			{
				mPointer->GetRef();
			}
		}
		return *this;
	}
	*/

	SmartPointer& operator=(std::nullptr_t)
	{
		Reset();
		return *this;
	}

	// Steal other's ref
	SmartPointer& operator=(SmartPointer&& smmove)
	{
		if (mPointer) mPointer->Destroy();
		mPointer = smmove.mPointer;
		smmove.mPointer = nullptr;
		return *this;
	}


	SmartPointer& operator=(const SmartPointer& smcopy)
	{
		if (mPointer != smcopy.get())
		{
			if (mPointer)
			{
				mPointer->Destroy();
			}
			//TODO(antoine) FIX const_cast
			mPointer = (smartPointOn*)smcopy.get();
			if (mPointer)
			{
				mPointer->GetRef();
			}
		}
		return *this;
	}

	template<typename smartPointOnOther>
	SmartPointer& operator=(const SmartPointer<smartPointOnOther>& smcopy)
	{
		smartPointOn* copy = (smartPointOn*)smcopy.get();
		if (mPointer != copy)
		{
			if (mPointer)
			{
				mPointer->Destroy();
			}
			mPointer = copy;
			if (mPointer)
			{
				mPointer->GetRef();
			}
		}
		return *this;
	}

	//operator smartPointOn*() const { return mPointer; }
	operator bool() const { return mPointer != nullptr; }

	bool	operator == (const smartPointOn* other) const
	{
		return mPointer == other;
	}

	bool	operator != (const smartPointOn* other) const
	{
		return mPointer != other;
	}

	template<typename smartPointOnOther>
	bool	operator == (const SmartPointer<smartPointOnOther>& other) const
	{
		return mPointer == other.get();
	}

	template<typename smartPointOnOther>
	bool	operator != (const SmartPointer<smartPointOnOther>& other) const
	{
		return mPointer != other.get();
	}

	void Reset()
	{
		if (mPointer) mPointer->Destroy();
		mPointer = nullptr;
	}
	
	smartPointOn* Pointer() const { return mPointer; }
	smartPointOn* get() const { return mPointer; }

	template<typename othertype>
	operator SmartPointer<othertype>() {

		SmartPointer<othertype> result((othertype*)mPointer, GetRefTag{}); // create a new ref here
		return result;
	}


protected:

	smartPointOn*	mPointer;

};

// The smart pointer will own the current ref on the object
/*template<typename smartPointOn>
SmartPointer<smartPointOn> MakeSmartPointer(smartPointOn* object)
{
	return SmartPointer<smartPointOn>{object};
}*/

template<typename smartPointOn>
SmartPointer<smartPointOn> OwningRawPtrToSmartPtr(smartPointOn* object)
{
	return SmartPointer<smartPointOn>{object, StealRefTag{}};
}

template<typename smartPointOn>
SmartPointer<smartPointOn> NonOwningRawPtrToSmartPtr(smartPointOn* object)
{
	return SmartPointer<smartPointOn>{object, GetRefTag{}};
}

template<typename smartPointOn, typename ... Args>
SmartPointer<smartPointOn> MakeRefCounted(Args&& ... args)
{
	return OwningRawPtrToSmartPtr(new smartPointOn(std::forward<decltype(args)>(args)...));
}

template<typename smartPointOn>
using SP=SmartPointer<smartPointOn>;


#endif //_SMARTPOINTER_H_