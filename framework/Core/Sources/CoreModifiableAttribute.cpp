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
	mFlags &= ~((u32)notifyOwnerFlag);
	switch (level)
	{
	case None:

		break;
	case Owner:
		this->mFlags |= (u32)notifyOwnerFlag;
		break;
	}
}

void	CoreModifiableAttribute::attachModifier(AttachedModifierBase* toAttach)
{
	if (!mAttachedModifier)
	{
		this->mFlags |= (unsigned int)haveAttachedModifier;
		mAttachedModifier = toAttach;
		changeInheritance();
	}
	else
	{
		toAttach->setNext(mAttachedModifier);
		mAttachedModifier = toAttach;
	}
}

void	CoreModifiableAttribute::detachModifier(AttachedModifierBase* toDetach)
{
	if (!mAttachedModifier)
		return;

	AttachedModifierBase* prev = 0;
	AttachedModifierBase* current = mAttachedModifier;
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
				mAttachedModifier = current->getNext();
			}

			toDetach->setNext(0);
			break;
		}
		prev = current;
		current = current->getNext();
	}

	// no more attached modifier
	if (!mAttachedModifier)
	{
		this->mFlags &= ~((unsigned int)haveAttachedModifier);
		changeInheritance();
	}
}

CoreModifiableAttribute::~CoreModifiableAttribute()
{
	if (mOwner)
	{
		kigs::unordered_map<KigsID, CoreModifiableAttribute*>::const_iterator it = mOwner->mAttributes.find(mID);
		if (it != mOwner->mAttributes.end())
		{
			mOwner->mAttributes.erase(it);
		}
	}
	if (mAttachedModifier)
	{
		delete mAttachedModifier;
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

	CoreItemEvaluationContext::SetContext(&mContext);

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
		mContext.mVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<kfloat>(0.0f);
		mCurrentItem = CoreItemOperator<kfloat>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::STRING:
	case CoreModifiable::ATTRIBUTE_TYPE::USSTRING:
	{
		mContext.mVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<kstl::string>("");
		mCurrentItem = CoreItemOperator<kstl::string>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::ARRAY:
	{
		// only Point2D & Point3D
		unsigned int asize = caller->getNbArrayElements();
		if (asize == 2)
		{
			mContext.mVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<Point2D>();
			mCurrentItem = CoreItemOperator<Point2D>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
		}
		else if (asize == 3)
		{
			mContext.mVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<Point3D>();
			mCurrentItem = CoreItemOperator<Point3D>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
		}
		else if (asize == 4)
		{
			mContext.mVariableList[LABEL_TO_ID(input).toUInt()] = new CoreValue<Vector4D>();
			mCurrentItem = CoreItemOperator<Vector4D>::Construct(addParam, &caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());

		}
	}
	break;
	default:
		break;
	}

	CoreItemEvaluationContext::ReleaseContext();
}

void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, kfloat& value)
{
	CoreItemEvaluationContext::SetContext(&mContext);
	*((CoreValue<kfloat>*)mContext.mVariableList[LABEL_TO_ID(input).toUInt()]) = value;
	value = *mCurrentItem.get();
	CoreItemEvaluationContext::ReleaseContext();
};

// strings
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, kstl::string& value)
{
	CoreItemEvaluationContext::SetContext(&mContext);
	*((CoreValue<kstl::string>*)mContext.mVariableList[LABEL_TO_ID(input).toUInt()]) = value;
	mCurrentItem->getValue(value);
	CoreItemEvaluationContext::ReleaseContext();
}
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, usString& value)
{
	CoreItemEvaluationContext::SetContext(&mContext);
	*((CoreValue<usString>*)mContext.mVariableList[LABEL_TO_ID(input).toUInt()]) = value;
	mCurrentItem->getValue(value);
	CoreItemEvaluationContext::ReleaseContext();
}

// 2D or 3D points
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, Point2D& value)
{
	CoreItemEvaluationContext::SetContext(&mContext);
	*((CoreValue<Point2D>*)mContext.mVariableList[LABEL_TO_ID(input).toUInt()]) = value;
	mCurrentItem->getValue(value);
	CoreItemEvaluationContext::ReleaseContext();
}
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, Point3D& value)
{
	CoreItemEvaluationContext::SetContext(&mContext);
	*((CoreValue<Point3D>*)mContext.mVariableList[LABEL_TO_ID(input).toUInt()]) = value;
	mCurrentItem->getValue(value);
	CoreItemEvaluationContext::ReleaseContext();
}