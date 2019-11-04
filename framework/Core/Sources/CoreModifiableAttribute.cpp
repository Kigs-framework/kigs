#include "PrecompiledHeaders.h"

#include "CoreModifiableAttribute.h"
#include "CoreModifiable.h"
#include "DoNothingObject.h"

#include "maReference.h"

#include "Core.h"

#include <algorithm>

#include "AttributeModifier.h"

//! convert enum to readable string
kstl::string CoreModifiableAttribute::typeToString(CoreModifiable::ATTRIBUTE_TYPE typ)
{
	switch (typ)
	{
	case CoreModifiable::BOOL:
	{
		return ("bool");
	}
	break;
	case CoreModifiable::CHAR:
	{
		return ("char");
	}
	break;
	case CoreModifiable::SHORT:
	{
		return ("short");
	}
	break;
	case CoreModifiable::INT:
	{
		return ("int");
	}
	break;
	case CoreModifiable::LONG:
	{
		return ("long");
	}
	break;
	case CoreModifiable::UCHAR:
	{
		return ("uchar");
	}
	break;
	case CoreModifiable::USHORT:
	{
		return ("ushort");
	}
	break;
	case CoreModifiable::UINT:
	{
		return ("uint");
	}
	break;
	case CoreModifiable::ULONG:
	{
		return ("ulong");
	}
	break;
	case CoreModifiable::FLOAT:
	{
		return ("float");
	}
	break;
	case CoreModifiable::DOUBLE:
	{
		return ("double");
	}
	break;
	case CoreModifiable::STRING:
	{
		return ("string");
	}
	break;
	case CoreModifiable::ARRAY:
	{
		return ("array");
	}
	break;
	case CoreModifiable::ENUM:
	{
		return ("enum");
	}
	case CoreModifiable::REFERENCE:
	{
		return ("reference");
	}
	break;
	case CoreModifiable::COREITEM:
	{
		return ("coreitem");
	}
	break;
	case CoreModifiable::USSTRING:
	{
		return ("usstring");
	}
	break;
	case CoreModifiable::COREBUFFER:
	{
		return ("corebuffer");
	}
	break;

	default:
	{
		return ("unknown");
	}
	break;
	}

	return ("unknown");
}


void CoreModifiableAttribute::changeNotificationLevel(AttributeNotificationLevel level)
{
	// Clear bit
	_Flags &= ~((u32)notifyOwnerFlag);
	switch (level)
	{
	case None:

		break;
	case Owner:
		this->_Flags |= (u32)notifyOwnerFlag;
		break;
	}
}

void	CoreModifiableAttribute::attachModifier(AttachedModifierBase* toAttach)
{
	if (!_attachedModifier)
	{
		this->_Flags |= (unsigned int)haveAttachedModifier;
		_attachedModifier = toAttach;
		changeInheritance();
	}
	else
	{
		toAttach->setNext(_attachedModifier);
		_attachedModifier = toAttach;
	}
}

void	CoreModifiableAttribute::detachModifier(AttachedModifierBase* toDetach)
{
	if (!_attachedModifier)
		return;

	AttachedModifierBase* prev = 0;
	AttachedModifierBase* current = _attachedModifier;
	while (current)
	{
		if (current == toDetach)
		{
			if (prev)
			{
				prev->setNext(current->getNext());
			}
			else
			{
				_attachedModifier = current->getNext();
			}

			toDetach->setNext(0);
			break;
		}
		prev = current;
		current = current->getNext();
	}

	// no more attached modifier
	if (!_attachedModifier)
	{
		this->_Flags &= ~((unsigned int)haveAttachedModifier);
		changeInheritance();
	}
}

CoreModifiableAttribute::~CoreModifiableAttribute()
{
	if (_owner)
	{
		robin_hood::unordered_map<KigsID, CoreModifiableAttribute*, KigsIDHash>::const_iterator it = _owner->_attributes.find(_id);
		if (it != _owner->_attributes.end())
		{
			_owner->_attributes.erase(it);
		}
	}
	if (_attachedModifier)
	{
		delete _attachedModifier;
	}
};


//! convert string to type enum
CoreModifiable::ATTRIBUTE_TYPE CoreModifiableAttribute::stringToType(const kstl::string &typ)
{
	if (typ == "BOOL" || typ == "bool")
	{
		return CoreModifiable::BOOL;
	}

	if (typ == "CHAR" || typ == "char")
	{
		return CoreModifiable::CHAR;
	}

	if (typ == "SHORT" || typ == "short")
	{
		return CoreModifiable::SHORT;
	}

	if (typ == "INT" || typ == "int")
	{
		return CoreModifiable::INT;
	}

	if (typ == "LONG" || typ == "long")
	{
		return CoreModifiable::LONG;
	}

	if (typ == "UCHAR" || typ == "uchar")
	{
		return CoreModifiable::UCHAR;
	}

	if (typ == "USHORT" || typ == "ushort")
	{
		return CoreModifiable::USHORT;
	}

	if (typ == "UINT" || typ == "uint")
	{
		return CoreModifiable::UINT;
	}

	if (typ == "ULONG" || typ == "ulong")
	{
		return CoreModifiable::ULONG;
	}

	if (typ == "FLOAT" || typ == "float")
	{
		return CoreModifiable::FLOAT;
	}

	if (typ == "DOUBLE" || typ == "double")
	{
		return CoreModifiable::DOUBLE;
	}

	if (typ == "STRING" || typ == "string")
	{
		return CoreModifiable::STRING;
	}

	if (typ == "ARRAY" || typ == "array")
	{
		return CoreModifiable::ARRAY;
	}

	if (typ == "ENUM" || typ == "enum")
	{
		return CoreModifiable::ENUM;
	}

	if (typ == "REFERENCE" || typ == "reference")
	{
		return CoreModifiable::REFERENCE;
	}

	if (typ == "COREITEM" || typ == "coreitem")
	{
		return CoreModifiable::COREITEM;
	}

	if (typ == "USSTRING" || typ == "usstring")
	{
		return CoreModifiable::USSTRING;
	}

	if (typ == "COREBUFFER" || typ == "corebuffer")
	{
		return CoreModifiable::COREBUFFER;
	}



	return CoreModifiable::UNKNOWN;
}


void CoreModifiableAttribute::ParseAttributePath(const kstl::string &path, kstl::string & CoreModifiablePath, kstl::string & CoreModifiableAttributeLabel)
{
	CoreModifiablePath = "";
	CoreModifiableAttributeLabel = "";
	kstl::string::size_type posPoint = path.find_last_of("->");

	if (posPoint != kstl::string::npos)
	{
		posPoint -= 1;
		CoreModifiablePath = path.substr(0, posPoint);
		CoreModifiableAttributeLabel = path.substr(posPoint + 2, path.length() - posPoint - 2);
	}
}



void	CoreItemOperatorModifier::Init(CoreModifiableAttribute* caller, bool isGetter, const kstl::string& addParam)
{
	AttachedModifierBase::Init(caller, isGetter, addParam);

	SetCoreItemOperatorContext(&myContext);

	CoreModifiable::ATTRIBUTE_TYPE type = caller->getType();

	switch (type)
	{
	case (int)CoreModifiable::BOOL:
	case (int)CoreModifiable::CHAR:
	case (int)CoreModifiable::SHORT:
	case (int)CoreModifiable::INT:
	case (int)CoreModifiable::LONG:
	case (int)CoreModifiable::UCHAR:
	case (int)CoreModifiable::USHORT:
	case (int)CoreModifiable::UINT:
	case (int)CoreModifiable::ULONG:
	case (int)CoreModifiable::FLOAT:
	case (int)CoreModifiable::DOUBLE:
	{
		myContext.myVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<kfloat>(0.0f);
		myCurrentItem = &CoreItemOperator<kfloat>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
	}
	break;
	case (int)CoreModifiable::STRING:
	case (int)CoreModifiable::USSTRING:
	{
		myContext.myVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<kstl::string>("");
		myCurrentItem = &CoreItemOperator<kstl::string>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
	}
	break;
	case (int)CoreModifiable::ARRAY:
	{
		// only Point2D & Point3D
		unsigned int asize = caller->getNbArrayElements();
		if (asize == 2)
		{
			myContext.myVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<Point2D>();
			myCurrentItem = &CoreItemOperator<Point2D>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
		}
		else if (asize == 3)
		{
			myContext.myVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<Point3D>();
			myCurrentItem = &CoreItemOperator<Point3D>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());

		}
	}
	break;
	default:
		break;
	}

	ReleaseCoreItemOperatorContext();
}

void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, kfloat& value)
{
	SetCoreItemOperatorContext(&myContext);
	*((CoreValue<kfloat>*)myContext.myVariableList[LABEL_TO_ID(input).toUInt()]) = value;
	value = *myCurrentItem;
	ReleaseCoreItemOperatorContext();
};

// strings
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, kstl::string& value)
{
	SetCoreItemOperatorContext(&myContext);
	*((CoreValue<kstl::string>*)myContext.myVariableList[LABEL_TO_ID(input).toUInt()]) = value;
	myCurrentItem->getValue(value);
	ReleaseCoreItemOperatorContext();
}
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, usString& value)
{
	SetCoreItemOperatorContext(&myContext);
	*((CoreValue<usString>*)myContext.myVariableList[LABEL_TO_ID(input).toUInt()]) = value;
	myCurrentItem->getValue(value);
	ReleaseCoreItemOperatorContext();
}

// 2D or 3D points
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, Point2D& value)
{
	SetCoreItemOperatorContext(&myContext);
	*((CoreValue<Point2D>*)myContext.myVariableList[LABEL_TO_ID(input).toUInt()]) = value;
	myCurrentItem->getValue(value);
	ReleaseCoreItemOperatorContext();
}
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, Point3D& value)
{
	SetCoreItemOperatorContext(&myContext);
	*((CoreValue<Point3D>*)myContext.myVariableList[LABEL_TO_ID(input).toUInt()]) = value;
	myCurrentItem->getValue(value);
	ReleaseCoreItemOperatorContext();
}