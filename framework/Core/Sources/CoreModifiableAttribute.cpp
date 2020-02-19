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
	case CoreModifiable::ATTRIBUTE_TYPE::BOOL:
	{
		return ("bool");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::CHAR:
	{
		return ("char");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::SHORT:
	{
		return ("short");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::INT:
	{
		return ("int");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::LONG:
	{
		return ("long");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::UCHAR:
	{
		return ("uchar");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::USHORT:
	{
		return ("ushort");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::UINT:
	{
		return ("uint");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::ULONG:
	{
		return ("ulong");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::FLOAT:
	{
		return ("float");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::DOUBLE:
	{
		return ("double");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::STRING:
	{
		return ("string");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::ARRAY:
	{
		return ("array");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::ENUM:
	{
		return ("enum");
	}
	case CoreModifiable::ATTRIBUTE_TYPE::REFERENCE:
	{
		return ("reference");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::COREITEM:
	{
		return ("coreitem");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::USSTRING:
	{
		return ("usstring");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::COREBUFFER:
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
				return CoreModifiable::ATTRIBUTE_TYPE::INT;
			}
		}
		break;
		case 4:
		{
			if (testtyp._id == KigsID("UINT")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::UINT;
			}
			if (testtyp._id == KigsID("ENUM")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::ENUM;
			}
			if (testtyp._id == KigsID("BOOL")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::BOOL;
			}
			if (testtyp._id == KigsID("CHAR")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::CHAR;
			}
			if (testtyp._id == KigsID("LONG")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::LONG;
			}
		}
		break;
		case 5:
		{
			if (testtyp._id == KigsID("FLOAT")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::FLOAT;
			}
			if (testtyp._id == KigsID("ARRAY")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::ARRAY;
			}
			if (testtyp._id == KigsID("SHORT")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::SHORT;
			}
			if (testtyp._id == KigsID("UCHAR")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::UCHAR;
			}
			if (testtyp._id == KigsID("ULONG")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::ULONG;
			}
		}
		break;
		case 6:
		{
			if (testtyp._id == KigsID("DOUBLE")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::DOUBLE;
			}
			if (testtyp._id == KigsID("STRING")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::STRING;
			}
			if (testtyp._id == KigsID("USHORT")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::USHORT;
			}
		}
		break;
		case 8:
		{
			if (testtyp._id == KigsID("COREITEM")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::COREITEM;
			}
			if (testtyp._id == KigsID("USSTRING")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::USSTRING;
			}
		}
		break;
		default: // more than 8
		{
			if (testtyp._id == KigsID("REFERENCE")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::REFERENCE;
			}
			if (testtyp._id == KigsID("COREBUFFER")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::COREBUFFER;
			}
		}
		break;
	}

	return CoreModifiable::ATTRIBUTE_TYPE::UNKNOWN;
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
	case CoreModifiable::ATTRIBUTE_TYPE::BOOL:
	case CoreModifiable::ATTRIBUTE_TYPE::CHAR:
	case CoreModifiable::ATTRIBUTE_TYPE::SHORT:
	case CoreModifiable::ATTRIBUTE_TYPE::INT:
	case CoreModifiable::ATTRIBUTE_TYPE::LONG:
	case CoreModifiable::ATTRIBUTE_TYPE::UCHAR:
	case CoreModifiable::ATTRIBUTE_TYPE::USHORT:
	case CoreModifiable::ATTRIBUTE_TYPE::UINT:
	case CoreModifiable::ATTRIBUTE_TYPE::ULONG:
	case CoreModifiable::ATTRIBUTE_TYPE::FLOAT:
	case CoreModifiable::ATTRIBUTE_TYPE::DOUBLE:
	{
		myContext.myVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<kfloat>(0.0f);
		myCurrentItem = CoreItemOperator<kfloat>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::STRING:
	case CoreModifiable::ATTRIBUTE_TYPE::USSTRING:
	{
		myContext.myVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<kstl::string>("");
		myCurrentItem = CoreItemOperator<kstl::string>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::ARRAY:
	{
		// only Point2D & Point3D
		unsigned int asize = caller->getNbArrayElements();
		if (asize == 2)
		{
			myContext.myVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<Point2D>();
			myCurrentItem = CoreItemOperator<Point2D>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
		}
		else if (asize == 3)
		{
			myContext.myVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<Point3D>();
			myCurrentItem = CoreItemOperator<Point3D>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
		}
		else if (asize == 4)
		{
			myContext.myVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<Vector4D>();
			myCurrentItem = CoreItemOperator<Vector4D>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());

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
	value = *myCurrentItem.get();
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