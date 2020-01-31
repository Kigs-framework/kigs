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
	
	SmartPointer() : myPointer(nullptr) {};
	SmartPointer(std::nullptr_t) : SmartPointer() {};

	// DOES NOT GET REF!
	// SmartPointer(smartPointOn* point) : myPointer(point) {};
	
	SmartPointer(smartPointOn* point, StealRefTag stealref) : myPointer(point) {};
	SmartPointer(smartPointOn* point, GetRefTag getref) : myPointer(point) { if(myPointer) myPointer->GetRef(); };

	~SmartPointer()
	{
		if (myPointer)
		{
			myPointer->Destroy();
			myPointer = nullptr;
		}
	}

	template<typename smartPointOnOther>
	SmartPointer(const SmartPointer<smartPointOnOther>& smcopy) : myPointer(nullptr)
	{
		*this = smcopy;
	}

	SmartPointer(const SmartPointer& other) : myPointer(nullptr)
	{
		*this = other;
	}

	// Move constructor, steal other ref
	SmartPointer(SmartPointer&& other) noexcept
	{
		myPointer = other.myPointer;
		other.myPointer = nullptr;
	}


	smartPointOn*	operator->() const {
		return myPointer;
	}

	bool	isNil() const
	{
		return myPointer == nullptr;
	}

	/*
	SmartPointer& operator=(smartPointOn* copy)
	{
		if (myPointer != copy)
		{
			if (myPointer)
			{
				myPointer->Destroy();
			}
			myPointer = copy;
			if (myPointer)
			{
				myPointer->GetRef();
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
		if (myPointer) myPointer->Destroy();
		myPointer = smmove.myPointer;
		smmove.myPointer = nullptr;
		return *this;
	}


	SmartPointer& operator=(const SmartPointer& smcopy)
	{
		if (myPointer != smcopy.get())
		{
			if (myPointer)
			{
				myPointer->Destroy();
			}
			//TODO(antoine) FIX const_cast
			myPointer = (smartPointOn*)smcopy.get();
			if (myPointer)
			{
				myPointer->GetRef();
			}
		}
		return *this;
	}

	template<typename smartPointOnOther>
	SmartPointer& operator=(const SmartPointer<smartPointOnOther>& smcopy)
	{
		smartPointOn* copy = (smartPointOn*)smcopy.get();
		if (myPointer != copy)
		{
			if (myPointer)
			{
				myPointer->Destroy();
			}
			myPointer = copy;
			if (myPointer)
			{
				myPointer->GetRef();
			}
		}
		return *this;
	}

	//operator smartPointOn*() const { return myPointer; }
	operator bool() const { return myPointer != nullptr; }

	bool	operator == (const smartPointOn* other) const
	{
		return myPointer == other;
	}

	bool	operator != (const smartPointOn* other) const
	{
		return myPointer != other;
	}

	template<typename smartPointOnOther>
	bool	operator == (const SmartPointer<smartPointOnOther>& other) const
	{
		return myPointer == other.get();
	}

	template<typename smartPointOnOther>
	bool	operator != (const SmartPointer<smartPointOnOther>& other) const
	{
		return myPointer != other.get();
	}

	void Reset()
	{
		if (myPointer) myPointer->Destroy();
		myPointer = nullptr;
	}
	
	smartPointOn* Pointer() const { return myPointer; }
	smartPointOn* get() const { return myPointer; }

	template<typename othertype>
	operator SmartPointer<othertype>() {

		SmartPointer<othertype> result((othertype*)myPointer, GetRefTag{}); // create a new ref here
		return result;
	}


protected:

	smartPointOn*	myPointer;

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