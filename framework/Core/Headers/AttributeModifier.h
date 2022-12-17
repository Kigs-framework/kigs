#pragma once

#include "CoreItemOperator.h"
#include "CoreVector.h"

class CoreModifiableAttribute;

// modifier class on getValue/setValue (virtual base class)
class AttachedModifierBase : public CoreVector
{
public:

	AttachedModifierBase() : CoreVector(), mNextModifier(0), mIsGetter(true)
	{
	}

	virtual void	Init(CoreModifiableAttribute* caller, bool isGetter, const std::string& addParam)
	{

#ifdef   KEEP_NAME_AS_STRING
		mInitString = addParam;
#endif
		mIsGetter = isGetter;
	}

	virtual ~AttachedModifierBase()
	{
		if (mNextModifier & 1)
		{
			delete getNext();
			mNextModifier = 0;
		}
	}

	// numeric
	void	CallModifier(CoreModifiableAttribute* caller, bool& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, s8& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, s16& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, s32& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, s64& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, u8& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, u16& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, u32& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, u64& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, float& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};
	void	CallModifier(CoreModifiableAttribute* caller, double& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};

	// strings
	void	CallModifier(CoreModifiableAttribute* caller, std::string& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};

	void	CallModifier(CoreModifiableAttribute* caller, usString& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};

	// 2D or 3D points
	void	CallModifier(CoreModifiableAttribute* caller, Point2D& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};

	void	CallModifier(CoreModifiableAttribute* caller, Point3D& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};

	void	CallModifier(CoreModifiableAttribute* caller, Vector4D& value, bool isGetter)
	{
		if (isGetterModifier() == isGetter) { ProtectedCallModifier(caller, value); } if (mNextModifier & 1) { getNext()->CallModifier(caller, value, isGetter); }
	};

	inline bool	isGetterModifier() { return mIsGetter; }

	inline void setNextObject(uintptr_t nexttoset)
	{
		mNextModifier = nexttoset;
	}

	inline AttachedModifierBase* getNext()
	{
		AttachedModifierBase* realaddress = (AttachedModifierBase*)(mNextModifier & (((uintptr_t)-1) ^ (uintptr_t)3));
		return realaddress;
	}

	inline uintptr_t getNextObject()
	{
		return mNextModifier;
	}
#ifdef   KEEP_NAME_AS_STRING
	// for export
	virtual std::string	GetModifierType() = 0;
	std::string			GetModifierInitString()
	{
		return mInitString;
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
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, float& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, double& value) {};

	// strings
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, std::string& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, usString& value) {};

	// 2D or 3D points
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, Point2D& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, Point3D& value) {};
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, Vector4D& value) {};

	uintptr_t				mNextModifier;
	bool					mIsGetter;

#ifdef   KEEP_NAME_AS_STRING
	std::string			mInitString;
#endif
};

class CoreItemOperatorModifier : public AttachedModifierBase
{
public:
	CoreItemOperatorModifier() : AttachedModifierBase()
		, mCurrentItem(nullptr)
	{

	}

	virtual ~CoreItemOperatorModifier()
	{
	}

	virtual void	Init(CoreModifiableAttribute* caller, bool isGetter, const std::string& addParam);

	static std::unique_ptr<CoreVector> create()
	{
		return std::make_unique<CoreItemOperatorModifier>();
	}

#ifdef   KEEP_NAME_AS_STRING
	// for export
	virtual std::string	GetModifierType()
	{
		return "CoreItemOperatorModifier";
	}
#endif
protected:

	// for numeric only float supported, the other type use float
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, bool& value)
	{
		float tmpval = (float)value;
		ProtectedCallModifier(caller, tmpval);
		value = (tmpval != 0.0f);
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s8& value)
	{
		float tmpval = (float)value;
		ProtectedCallModifier(caller, tmpval);
		value = (char)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s16& value)
	{
		float tmpval = (float)value;
		ProtectedCallModifier(caller, tmpval);
		value = (s16)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s32& value)
	{
		float tmpval = (float)value;
		ProtectedCallModifier(caller, tmpval);
		value = (s32)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, s64& value)
	{
		float tmpval = (float)value;
		ProtectedCallModifier(caller, tmpval);
		value = (s64)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u8& value)
	{
		float tmpval = (float)value;
		ProtectedCallModifier(caller, tmpval);
		value = (u8)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u16& value)
	{
		float tmpval = (float)value;
		ProtectedCallModifier(caller, tmpval);
		value = (u16)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u32& value)
	{
		float tmpval = (float)value;
		ProtectedCallModifier(caller, tmpval);
		value = (u32)tmpval;
	}
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, u64& value)
	{
		float tmpval = (float)value;
		ProtectedCallModifier(caller, tmpval);
		value = (u64)tmpval;
	}

	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, double& value)
	{
		float tmpval = (float)value;
		ProtectedCallModifier(caller, tmpval);
		value = (double)tmpval;
	}

	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, float& value);

	// strings
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, std::string& value);
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, usString& value);

	// 2D or 3D points
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, Point2D& value);
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, Point3D& value);
	virtual void	ProtectedCallModifier(CoreModifiableAttribute* caller, Vector4D& value);

	CoreItemSP					mCurrentItem;
	CoreItemEvaluationContext	mContext;
};