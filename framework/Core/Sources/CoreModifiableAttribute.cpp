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
CoreModifiable::ATTRIBUTE_TYPE CoreModifiableAttribute::stringToType(const kstl::string_view & typ_view)
{
	std::string typ(typ_view.data(), typ_view.length());
	str_toupper(typ);
	KigsID testtyp(typ);

	

	switch (typ_view.length()) // avoid doing all tests... Check only the right length ones
	{
		case 3:
		{
			if (testtyp._id == KigsID("INT")._id)
			{
				return CoreModifiable::INT;
			}
		}
		break;
		case 4:
		{
			if (testtyp._id == KigsID("UINT")._id)
			{
				return CoreModifiable::UINT;
			}
			if (testtyp._id == KigsID("ENUM")._id)
			{
				return CoreModifiable::ENUM;
			}
			if (testtyp._id == KigsID("BOOL")._id)
			{
				return CoreModifiable::BOOL;
			}
			if (testtyp._id == KigsID("CHAR")._id)
			{
				return CoreModifiable::CHAR;
			}
			if (testtyp._id == KigsID("LONG")._id)
			{
				return CoreModifiable::LONG;
			}
		}
		break;
		case 5:
		{
			if (testtyp._id == KigsID("FLOAT")._id)
			{
				return CoreModifiable::FLOAT;
			}
			if (testtyp._id == KigsID("ARRAY")._id)
			{
				return CoreModifiable::ARRAY;
			}
			if (testtyp._id == KigsID("SHORT")._id)
			{
				return CoreModifiable::SHORT;
			}
			if (testtyp._id == KigsID("UCHAR")._id)
			{
				return CoreModifiable::UCHAR;
			}
			if (testtyp._id == KigsID("ULONG")._id)
			{
				return CoreModifiable::ULONG;
			}
		}
		break;
		case 6:
		{
			if (testtyp._id == KigsID("DOUBLE")._id)
			{
				return CoreModifiable::DOUBLE;
			}
			if (testtyp._id == KigsID("STRING")._id)
			{
				return CoreModifiable::STRING;
			}
			if (testtyp._id == KigsID("USHORT")._id)
			{
				return CoreModifiable::USHORT;
			}
		}
		break;
		case 8:
		{
			if (testtyp._id == KigsID("COREITEM")._id)
			{
				return CoreModifiable::COREITEM;
			}
			if (testtyp._id == KigsID("USSTRING")._id)
			{
				return CoreModifiable::USSTRING;
			}
		}
		break;
		default: // more than 8
		{
			if (testtyp._id == KigsID("REFERENCE")._id)
			{
				return CoreModifiable::REFERENCE;
			}
			if (testtyp._id == KigsID("COREBUFFER")._id)
			{
				return CoreModifiable::COREBUFFER;
			}
		}
		break;
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