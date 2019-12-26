#ifndef _COREMODIFIABLEATTRIBUTE_H
#define _COREMODIFIABLEATTRIBUTE_H

#ifdef STRING
#undef STRING
#endif

#include "CoreModifiable.h"
#include "usString.h"

class AttachedModifierBase;
class CoreItem;

// ****************************************
// * AttributeNotificationLevel enum
// * --------------------------------------
/*!
* \enum	AttributeNotificationLevel
* CoreModifiableAttribute can now notify their owner/clones that they have changed
* Do do this, some dynamic type change is done with a placement new, changing the virtual
* table of the objects. But placement new can in some cases reinit members (ie :for strings).
* So placement new constructor must have some parameters as copy.
* The notification occurs only when parameter is changed with a setValue (or array equivalent) methods
*
* \ingroup KigsCore
*/
// ****************************************
enum	AttributeNotificationLevel
{
	None =	1,
	Owner =	2,
};

// Tag type for inheritance switch constructor
struct InheritanceSwitch {};;

#define INHERIT_LEVEL_SHIFT 3
#define INHERIT_LEVEL_MOD 3


#define IMPLEMENT_CHANGE_LEVEL \
void changeInheritance() override\
{\
	KigsID old_id = _id; \
	_id.~KigsID(); \
	CurrentAttributeType old_value = _value; \
	_value.~CurrentAttributeType(); \
	AttachedModifierBase* modifier = _attachedModifier; \
	u32 old_flags = _Flags; \
	u32 inheritlevel = (_Flags >> INHERIT_LEVEL_SHIFT) & INHERIT_LEVEL_MOD; \
	doPlacementNew(inheritlevel); \
	_value = old_value; \
	_Flags = old_flags; \
	_attachedModifier = modifier; \
}\
void changeNotificationLevel(AttributeNotificationLevel level) override\
{\
	CoreModifiableAttributeData::changeNotificationLevel(level);\
	changeInheritance();\
}

#define STATIC_ASSERT_NOTIF_LEVEL_SIZES(classname) static_assert(sizeof(classname<0>) == sizeof(classname<1>), "Size mismatch between notification levels");\
static_assert(sizeof(classname<0>) == sizeof(classname<2>), "Size mismatch between notification levels");\
static_assert(sizeof(classname<0>) == sizeof(classname<3>), "Size mismatch between notification levels");\
static_assert(sizeof(classname<2>) == sizeof(classname<3>), "Size mismatch between notification levels");


#define DECLARE_ATTRIBUTE_USINGS(underlying_type)\
protected:\
	using CoreModifiableAttributeData<underlying_type>::_value;\
public:\
	using CoreModifiableAttributeData<underlying_type>::isReadOnly;

#define DECLARE_ATTRIBUTE_HERITAGE_IMPL(name, template_name, underlying_type, enum_type)\
public:\
using CoreModifiableAttributeData<underlying_type>::CoreModifiableAttributeData;\
typedef name CurrentAttributeClass;\
typedef underlying_type CurrentAttributeType;\
static constexpr CoreModifiable::ATTRIBUTE_TYPE type = enum_type;\
CoreModifiable::ATTRIBUTE_TYPE getType() const override { return enum_type; }\
DECLARE_ATTRIBUTE_USINGS(underlying_type)\
auto& operator=(const template_name<notificationLevel>& attribute)\
{\
	this->CopyData(attribute);\
return *this; \
}\
protected:\
void doPlacementNew(u32 level) override\
{\
	switch (level)\
	{\
		case 0: new (this) template_name<0>(InheritanceSwitch{}); break; \
		case 1: new (this) template_name<1>(InheritanceSwitch{}); break; \
		case 2: new (this) template_name<2>(InheritanceSwitch{}); break; \
		case 3: new (this) template_name<3>(InheritanceSwitch{}); break; \
		default: assert(false); break; \
	}\
}

// Use this one if the underlying_type is not something used anywhere (like an internal structure)
#define DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(name, template_name, underlying_type, enum_type) DECLARE_ATTRIBUTE_HERITAGE_IMPL(name, template_name, underlying_type, enum_type)

#define DECLARE_ATTRIBUTE_HERITAGE(name, template_name, underlying_type, enum_type) DECLARE_ATTRIBUTE_HERITAGE_IMPL(name, template_name, underlying_type, enum_type)\
public:\
virtual operator CurrentAttributeType() const\
{\
	CurrentAttributeType tmpValue = _value;\
	CALL_GETMODIFIER(notificationLevel, tmpValue);\
	return tmpValue;\
};\
auto& operator=(const CurrentAttributeType& value)\
{\
	_value = value;\
	return *this;\
}






/*! \defgroup CoreModifiableAttibute CoreModifiableAttibute
*	\ingroup KigsCore
*  \brief group of coreModifiableAttribute
*
* A CoreModifiableAttribute defines a type as an object.<br>
* the management of attribute is transparent :<br>
* This allow to export and import a CoreModifiable with its attributes.<br>
* Each CoreModifiableAttribute can be used and modified by anyone using generic function as getValue() and setValue().<br>
* it IS possible to add and remove a dynamic attribute to a CoreModifiable.<br>
* a CoreModifiableAttribute is accessed by an unique id. see the class documentation for the list of available CoreModifiableAttribute.<br>
*
* <dl class="exemple"><dt><b>Exemple:</b></dt><dd>
* <span class="comment"> using setValue() on a CoreModifiable :</span><br>
* <span class="code">
* CoreModifiable* SkinObject=(CoreModifiable*)(KigsCore::GetInstanceOf(<i>instance_name</i>,"SkinningResource"));<br>
* SkinObject->setValue(LABEL_TO_ID(SkeletonFileName),<i>file_name.skel</i>);<span class="comment"> // get the CoreModifiableAttribute ID from its name</span><br>
* SkinObject->setValue(LABEL_TO_ID(SkinFileName),<i>file_name.skin</i>);<br>
* SkinObject->Init();<br>
* </span>
* <span class="comment"> initialization in constructor :</span><br>
* <span class="code">
* SkinningResource::SkinningResource(),<br>
* mySkeletonFileName(*this,true,LABEL_AND_ID(SkeletonFileName),""),<br>
* mySkinFileName(*this,true,LABEL_AND_ID(SkinFileName),""),<br>
* {}<br>
* <span class="comment"> // first param  : link to the owner</span><br>
* <span class="comment"> // second param : TRUE if the param is needed for the init</span><br>
* <span class="comment"> // third param  : Id of the CoreModifiableAttribute</span><br>
* <span class="comment"> // fourth param : default value</span><br>
* </span>
* </dd></dl>
*/


// ****************************************
// * CoreModifiableAttribute class
// * --------------------------------------
/**
* \file	CoreModifiableAttribute.h
* \class	CoreModifiableAttribute
* \ingroup KigsCore
* \brief	 base class for CoreModifiableAttributeData . This class is just composed of virtual methods
* \author	ukn
* \version ukn
* \date	ukn
*/
// ****************************************
class CoreModifiableAttribute
{
	CoreModifiableAttribute(const CoreModifiableAttribute& attribute) = delete;

protected:
	explicit CoreModifiableAttribute(InheritanceSwitch tag) {}


	CoreModifiableAttribute(CoreModifiable* owner, KigsID ID) :
		_owner(owner)
		, _attachedModifier(nullptr)
		, _Flags(0)
		, _id(ID)
	{
		if(_owner)
			_owner->_attributes[ID] = this;
	}


public:


	virtual ~CoreModifiableAttribute();
	
	

	CoreModifiableAttribute& operator=(const CoreModifiableAttribute& other)
	{
		CopyAttribute(other);
		return *this;
	}
	

	

	//! retreive attribute label and coremodifiable path in a full path. Format is corepath->attributelabel
	static void ParseAttributePath(const kstl::string &path, kstl::string & CoreModifiablePath, kstl::string & CoreModifiableAttributeLabel);
	static kstl::string typeToString(CoreModifiable::ATTRIBUTE_TYPE typ);
	static CoreModifiable::ATTRIBUTE_TYPE stringToType(const kstl::string_view &typ);
	

	virtual CoreModifiable::ATTRIBUTE_TYPE getType() const = 0;
	virtual CoreModifiable::ATTRIBUTE_TYPE getArrayElementType() const { return CoreModifiable::UNKNOWN; }

	virtual u32 getNbArrayElements() const { return 0; }
	virtual u32 getNbArrayColumns() const { return 0; }
	virtual u32 getNbArrayLines() const { return 0; }
	

	virtual kstl::vector<kstl::string> getEnumElements() const { return {}; }
	
	KigsID getID() const { return _id; };

	// For Backward compatibility:
	KigsID getLabel() const { return _id;};
	KigsID getLabelID() const { return _id; };
	
	
	enum ModifiableAttributeFlags
	{
		isReadOnlyFlag = 1,
		isInitFlag = 2,
		isDynamicFlag = 4,
		notifyOwnerFlag = 8,
		haveAttachedModifier = 16
	};

	


	AttachedModifierBase*	getFirstAttachedModifier() { return _attachedModifier; }
	void	attachModifier(AttachedModifierBase* toAttach);
	void	detachModifier(AttachedModifierBase* toDetach);
	

	virtual void changeNotificationLevel(AttributeNotificationLevel level);

	
#define DECLARE_SET(type)	virtual bool setValue(type value){ return false; }
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SET);
	DECLARE_SET(const char*);
	DECLARE_SET(const kstl::string&);
	DECLARE_SET(const unsigned short*);
	DECLARE_SET(const usString&);
	DECLARE_SET(const CheckUniqueObject&);
	DECLARE_SET(CoreItem*);
	DECLARE_SET(CoreModifiable*);
	DECLARE_SET(void*);
	DECLARE_SET(const UTF8Char*); // Only for usstring

#define DECLARE_GET(type) virtual bool getValue(type value) const { return false; }
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GET);
	DECLARE_GET(kstl::string&);
	DECLARE_GET(usString&);
	DECLARE_GET(CheckUniqueObject&);
	DECLARE_GET(CoreItem*&);
	DECLARE_GET(CoreModifiable*&);
	DECLARE_GET(void*&);

#define DECLARE_SETARRAYVALUE(type)	virtual bool setArrayValue(type /*value*/, u32 /* nbElements */){return false;};
	EXPAND_MACRO_FOR_BASE_TYPES(const, *, DECLARE_SETARRAYVALUE);

#define DECLARE_GETARRAYVALUE(type) virtual bool getArrayValue(type * const /* value */ , u32 /* nbElements */) const {return false;};
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_GETARRAYVALUE);
	
#define DECLARE_SETARRAYELEMENTVALUE(type)	virtual bool setArrayElementValue(type /*value*/, u32 /* line */, u32 /* column */){ return false; };
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SETARRAYELEMENTVALUE);
	DECLARE_SETARRAYELEMENTVALUE(const kstl::string&);
		
#define DECLARE_GETARRAYELEMENTVALUE(type) virtual bool getArrayElementValue(type /* value */, u32 /* line */, u32 /* column */) const {return false;};
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GETARRAYELEMENTVALUE);
	DECLARE_GETARRAYELEMENTVALUE(kstl::string&);


#undef DECLARE_SET
#undef DECLARE_GET
#undef DECLARE_SETARRAYVALUE
#undef DECLARE_GETARRAYVALUE
#undef DECLARE_SETARRAYELEMENTVALUE
#undef DECLARE_GETARRAYELEMENTVALUE

	CoreModifiable& getOwner() const {return *_owner;}
	
	virtual bool	isReadOnly() = 0;
	virtual void	setReadOnly(bool val) = 0;
	
	virtual bool	isDynamic() = 0;
	virtual void	setDynamic(bool dyn) = 0;
	
	virtual void	setIsInitParam(bool init) = 0;
	virtual bool	isInitParam() = 0;

	virtual void*	getRawValue() = 0;
	
	// return element number for array or one for other attributes
	virtual int		size() const { return 1; };
	virtual size_t 	MemorySize() const { return 0; };
	
	virtual void	changeInheritance() = 0;
	virtual void	doPlacementNew(u32 level) = 0;

	virtual bool	IsAClone() const = 0;

	// Return true if the copy was done correctly
	virtual bool	CopyAttribute(const CoreModifiableAttribute& other) = 0;

protected:

	CoreModifiable*   _owner;
	AttachedModifierBase*	_attachedModifier;
	u32 _Flags;
	KigsID _id;
	
};

// ****************************************
// * CoreModifiableAttributeData class
// * --------------------------------------
/**
* \class	CoreModifiableAttributeData
* \ingroup CoreModifiableAttibute
* \brief	base class for CoreModifiable attributes
*/
// ****************************************

template<typename T>
class CoreModifiableAttributeData : public CoreModifiableAttribute
{

public:
	CoreModifiableAttributeData(CoreModifiable& owner, bool isInitParam, KigsID ID, const T& value) : CoreModifiableAttribute(&owner, ID), _value(value)
	{
		setIsInitParam(isInitParam);
	}
	CoreModifiableAttributeData(CoreModifiable& owner, bool isInitParam, KigsID ID) : CoreModifiableAttribute(&owner, ID), _value{}
	{
		setIsInitParam(isInitParam);
	}
	CoreModifiableAttributeData(KigsID ID, const T& value) : CoreModifiableAttribute(nullptr, ID), _value{value}
	{
	}

	explicit CoreModifiableAttributeData(InheritanceSwitch tag) : CoreModifiableAttribute(tag) {}



	virtual void changeNotificationLevel(AttributeNotificationLevel level) final
	{
		CoreModifiableAttribute::changeNotificationLevel(level);
		changeInheritance();
	}

	T& ref() { return _value; }
	const T& const_ref() const { return _value; }

	virtual void* getRawValue() final { return static_cast<void*>(&_value); }


	virtual bool IsAClone() const final {return false;}
	
	friend class CoreModifiable;
	
	//! Read only attributes cannot be modified with setValue
	virtual bool isReadOnly() final {return (bool)((((u32)isReadOnlyFlag)&this->_Flags)!=0);}
	//! \brief  return true if attribute is an init attribute (necessary for the CoreModifiable Init to be done)
	virtual bool isInitParam() final {return (bool)((((u32)isInitFlag)&this->_Flags)!=0);}
	virtual bool isDynamic() final {return (bool)((((u32)isDynamicFlag)&this->_Flags)!=0);}
	virtual void setReadOnly(bool val) final
	{
		if(val)
		{
			this->_Flags|=(u32)isReadOnlyFlag;
		}
		else
		{
			this->_Flags&=~(u32)isReadOnlyFlag;
		}
	}
	virtual void setDynamic(bool dyn) final
	{
		if(dyn)
		{
			this->_Flags|=(u32)isDynamicFlag;
		}
		else
		{
			this->_Flags&=~(u32)isDynamicFlag;
		}
	}
	virtual void setIsInitParam(bool init) final
	{
		if(init)
		{
			this->_Flags|=(u32)isInitFlag;
		}
		else
		{
			this->_Flags&=~(u32)isInitFlag;
		}
	}

	size_t MemorySize() const final { return sizeof(T); };
	
	CoreModifiableAttributeData<T>& operator=(const CoreModifiableAttributeData<T>& other)
	{
		CopyData(other);
		return *this;
	}
	
protected:

	//! Underlying value
	T _value;

	virtual void changeInheritance() final
	{
		KigsID old_id = _id;
		_id.~KigsID();
		T old_value = _value;
		_value.~T();
		AttachedModifierBase* modifier = _attachedModifier;
		u32 old_flags = _Flags;
		u32 inheritlevel = (_Flags >> INHERIT_LEVEL_SHIFT) & INHERIT_LEVEL_MOD;
		doPlacementNew(inheritlevel);
		_id = old_id;
		_value = old_value;
		_Flags = old_flags;
		_attachedModifier = modifier;
	}
	

	// Uses RTTI dynamic cast to check for compatibility by default
	// If we ever add attributes that are not CoreModifiableAttributeData this would need to be changed/overloaded for each type
	bool CopyAttribute(const CoreModifiableAttribute& other) override
	{
#if 0
	 	const CoreModifiableAttributeData<T>* casted = dynamic_cast<const CoreModifiableAttributeData<T>*>(&other);
		if (casted)
		{
			CopyData(*casted);
			return true;
		}
		return false;
#else
		if (getType() == other.getType() && 
			getArrayElementType() == other.getArrayElementType() && 
			getNbArrayColumns() == other.getNbArrayColumns() &&
			getNbArrayLines() == getNbArrayLines()
			&& !other.IsAClone())
		{
			CopyData(static_cast<const CoreModifiableAttributeData<T>&>(other));
			return true;
		}
		return false;
#endif
	}

	virtual void CopyData(const CoreModifiableAttributeData<T>& other)
	{
		_value = other._value;
	}
};


#define CALL_GETMODIFIER(level,value) if(level&2){this->_attachedModifier->CallModifier((CoreModifiableAttribute *)this,value,true);}
#define CALL_SETMODIFIER(level,value) if(level&2){this->_attachedModifier->CallModifier((CoreModifiableAttribute *)this,value,false);}

#define DO_NOTIFICATION(level)	if(level&1){this->_owner->NotifyUpdate(CoreModifiableAttribute::getLabel().toUInt());}




template<int notificationLevel>
class maRawPtrHeritage : public CoreModifiableAttributeData<void*>
{
	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maRawPtrHeritage, maRawPtrHeritage, void*, CoreModifiable::RAWPTR);
public:
	
	bool getValue(CoreModifiable*& value) const override
	{
		value = static_cast<CoreModifiable*>(_value);
		return true;
	}
	bool getValue(void*& value) const override { value = _value; return true; }
	bool setValue(void* value) override 
	{ 
		if (isReadOnly()) return false;
		_value = value; 
		DO_NOTIFICATION(notificationLevel);
		return true; 
	}
	bool setValue(CoreModifiable* value) override
	{
		if (isReadOnly()) return false;
		_value = static_cast<void*>(value);
		DO_NOTIFICATION(notificationLevel);
		return true;
	}

};
using maRawPtr = maRawPtrHeritage<0>;

// Template specializations defined in CoreModifiable.cpp
//! \brief methods to convert a string to a value (numeric, boolean...)
template<typename T> bool CoreConvertString2Value(const kstl::string &stringValue,T& value);
//! \brief methods to convert a value (numeric, boolean...) to a string
template<typename T> bool CoreConvertValue2String(kstl::string& stringValue,T value);


//@REFACTOR: Ideally those includes would be removed and each .cpp only includes the ma****.h that it needs
#include "maString.h"
#include "maNumeric.h"
#include "maBool.h"
#include "maEnum.h"
#include "maArray.h"



#endif //_COREMODIFIABLEATTRIBUTE_H
