#pragma once

#include "CoreItemOperator.h"
#include "CoreVector.h"

class CoreModifiableAttribute;

// modifier class on getValue/setValue (virtual base class)
class AttachedModifierBase : public CoreVector
{
public:

	AttachedModifierBase() : CoreVector(), _nextModifier(0), myIsGetter(true)
	{
	}

	virtual void	Init(CoreModifiableAttribute* caller, bool isGetter, const kstl::string& addParam)
	{

#ifdef   KEEP_NAME_AS_STRING
		myInitString = addParam;
#endif
		myIsGetter = isGetter;
	}

	virtual ~AttachedModifierBase()
	{
		if (_nextModifier)
		{
			delete _nextModifier;
		}
		_nextModifier = 0;
	}

	// numeric
	void	CallModifier(CoreModifiableAttribute* caller, bool& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, s8& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, s16& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, s32& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, s64& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, u8& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, u16& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, u32& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, u64& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, kfloat& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, kdouble& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};

	// strings
	void	CallModifier(CoreModifiableAttribute* caller, kstl::string& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};

	void	CallModifier(CoreModifiableAttribute* caller, usString& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};

	// 2D or 3D points
	void	CallModifier(CoreModifiableAttribute* caller, Point2D& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};

	void	CallModifier(CoreModifiableAttribute* caller, Point3D& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (_nextModifier) { _nextModifier->CallModifier(caller, value, isGetter); }
	};

	inline bool	isGetterModifier() { return myIsGetter; }

	inline void setNext(AttachedModifierBase* nexttoset)
	{
		_nextModifier = nexttoset;
	}

	inline AttachedModifierBase* getNext()
	{
		return _nextModifier;
	}

#ifdef   KEEP_NAME_AS_STRING
	// for export
	virtual kstl::string	GetModifierType() = 0;
	kstl::string			GetModifierInitString()
	{
		return myInitString;
	}
#endif

protected:

	// numeric
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, bool& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s8& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s16& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s32& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s64& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u8& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u16& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u32& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u64& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, kfloat& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, kdouble& value) {};

	// strings
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, kstl::string& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, usString& value) {};

	// 2D or 3D points
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, Point2D& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, Point3D& value) {};

	AttachedModifierBase*	_nextModifier;

	bool					myIsGetter;

#ifdef   KEEP_NAME_AS_STRING
	kstl::string			myInitString;
#endif
};

class CoreItemOperatorModifier : public AttachedModifierBase
{
public:
	CoreItemOperatorModifier() : AttachedModifierBase()
		, myCurrentItem(0)
	{

	}

	virtual ~CoreItemOperatorModifier()
	{
		myCurrentItem->Destroy();
		myCurrentItem = 0;

		auto itcurrent = myContext.myVariableList.begin();
		auto itend = myContext.myVariableList.end();
		while (itcurrent != itend)
		{
			if ((*itcurrent).second)
			{
				(*itcurrent).second->Destroy();
			}
			++itcurrent;
		}

	}

	virtual void	Init(CoreModifiableAttribute* caller, bool isGetter, const kstl::string& addParam);

	static CoreVector* create()
	{
		return new CoreItemOperatorModifier();
	}

#ifdef   KEEP_NAME_AS_STRING
	// for export
	virtual kstl::string	GetModifierType()
	{
		return "CoreItemOperatorModifier";
	}
#endif
protected:

	// for numeric only kfloat supported, the other type use kfloat
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, bool& value)
	{
		kfloat tmpval = (kfloat)value;
		ProtectedCallModifier(caller, tmpval);
		value = (tmpval != 0.0f);
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s8& value)
	{
		kfloat tmpval = (kfloat)value;
		ProtectedCallModifier(caller, tmpval);
		value = (char)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s16& value)
	{
		kfloat tmpval = (kfloat)value;
		ProtectedCallModifier(caller, tmpval);
		value = (s16)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s32& value)
	{
		kfloat tmpval = (kfloat)value;
		ProtectedCallModifier(caller, tmpval);
		value = (s32)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s64& value)
	{
		kfloat tmpval = (kfloat)value;
		ProtectedCallModifier(caller, tmpval);
		value = (s64)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u8& value)
	{
		kfloat tmpval = (kfloat)value;
		ProtectedCallModifier(caller, tmpval);
		value = (u8)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u16& value)
	{
		kfloat tmpval = (kfloat)value;
		ProtectedCallModifier(caller, tmpval);
		value = (u16)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u32& value)
	{
		kfloat tmpval = (kfloat)value;
		ProtectedCallModifier(caller, tmpval);
		value = (u32)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u64& value)
	{
		kfloat tmpval = (kfloat)value;
		ProtectedCallModifier(caller, tmpval);
		value = (u64)tmpval;
	}

	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, kdouble& value)
	{
		kfloat tmpval = (kfloat)value;
		ProtectedCallModifier(caller, tmpval);
		value = (kdouble)tmpval;
	}

	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, kfloat& value);

	// strings
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, kstl::string& value);
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, usString& value);

	// 2D or 3D points
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, Point2D& value);
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, Point3D& value);

	CoreItem*					myCurrentItem;
	CoreItemEvaluationContext	myContext;
};