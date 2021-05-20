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
	case CoreModifiable::ATTRIBUTE_TYPE::WEAK_REFERENCE:
	{
		return ("reference");
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::STRONG_REFERENCE:
	{
		return ("strong_reference");
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

CoreModifiable* CoreModifiableAttribute::getOwner() const
{
	return mOwner;
}

void	CoreModifiableAttribute::attachModifier(SP<AttachedModifierBase> toAttach)
{
	if (!toAttach)
	{
		return;
	}
	SP<AttachedModifierBase> modifier = getFirstAttachedModifier();

	toAttach->setNextObject(mAttachedModifier);
	mAttachedModifier = toAttach;

	if (!modifier) // first one
	{
		this->mFlags |= (unsigned int)haveAttachedModifier;
		changeInheritance();
	}
	
}

void	CoreModifiableAttribute::detachModifier(SP<AttachedModifierBase> toDetach)
{
	SP<AttachedModifierBase> modifier = getFirstAttachedModifier();
	if (!modifier)
		return;

	SP<AttachedModifierBase> prev;
	SP<AttachedModifierBase> current = modifier;
	while (current)
	{
		if (current == toDetach)
		{
			if (prev)
			{
				prev->setNextObject(current);
			}
			else
			{
				mAttachedModifier = current;
			}
			toDetach->setNextObject(nullptr);
			break;
		}
		prev = current;
		current = current->getNext();
	}

	modifier = getFirstAttachedModifier();
	// no more attached modifier
	if (!modifier)
	{
		this->mFlags &= ~((unsigned int)haveAttachedModifier);
		changeInheritance();
	}
}

CoreModifiableAttribute::~CoreModifiableAttribute()
{
	CoreModifiable* owner = getOwner();
	if (owner)
	{
		kigs::unordered_map<KigsID, CoreModifiableAttribute*>::const_iterator it = owner->mAttributes.find(mID);
		if (it != owner->mAttributes.end())
		{
			owner->mAttributes.erase(it);
		}
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
				return CoreModifiable::ATTRIBUTE_TYPE::WEAK_REFERENCE;
			}
			if (testtyp._id == KigsID("STRONG_REFERENCE")._id)
			{
				return CoreModifiable::ATTRIBUTE_TYPE::STRONG_REFERENCE;
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
		mContext.mVariableList[LABEL_TO_ID(input).toUInt()].push_back(MakeCoreValue(0.0f));
		mCurrentItem = CoreItemOperator<kfloat>::Construct(addParam, caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::STRING:
	case CoreModifiable::ATTRIBUTE_TYPE::USSTRING:
	{
		mContext.mVariableList[LABEL_TO_ID(input).toUInt()].push_back(MakeCoreValue(std::string()));
		mCurrentItem = CoreItemOperator<kstl::string>::Construct(addParam, caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
	}
	break;
	case CoreModifiable::ATTRIBUTE_TYPE::ARRAY:
	{
		// only Point2D & Point3D
		unsigned int asize = caller->getNbArrayElements();
		if (asize == 2)
		{
			mContext.mVariableList[LABEL_TO_ID(input).toUInt()].push_back(MakeCoreValue(v2f{}));
			mCurrentItem = CoreItemOperator<Point2D>::Construct(addParam, caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
		}
		else if (asize == 3)
		{
			mContext.mVariableList[LABEL_TO_ID(input).toUInt()].push_back(MakeCoreValue(v3f{}));
			mCurrentItem = CoreItemOperator<Point3D>::Construct(addParam, caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
		}
		else if (asize == 4)
		{
			mContext.mVariableList[LABEL_TO_ID(input).toUInt()].push_back(MakeCoreValue(v4f{}));
			mCurrentItem = CoreItemOperator<Vector4D>::Construct(addParam, caller->getOwner(), KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());

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
	*((CoreValue<kfloat>*)mContext.mVariableList[LABEL_TO_ID(input).toUInt()].back().get()) = value;
	value = *mCurrentItem.get();
	CoreItemEvaluationContext::ReleaseContext();
};

// strings
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, kstl::string& value)
{
	CoreItemEvaluationContext::SetContext(&mContext);
	*((CoreValue<kstl::string>*)mContext.mVariableList[LABEL_TO_ID(input).toUInt()].back().get()) = value;
	mCurrentItem->getValue(value);
	CoreItemEvaluationContext::ReleaseContext();
}
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, usString& value)
{
	CoreItemEvaluationContext::SetContext(&mContext);
	*((CoreValue<usString>*)mContext.mVariableList[LABEL_TO_ID(input).toUInt()].back().get()) = value;
	mCurrentItem->getValue(value);
	CoreItemEvaluationContext::ReleaseContext();
}

// 2D or 3D points
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, Point2D& value)
{
	CoreItemEvaluationContext::SetContext(&mContext);
	*((CoreValue<Point2D>*)mContext.mVariableList[LABEL_TO_ID(input).toUInt()].back().get()) = value;
	mCurrentItem->getValue(value);
	CoreItemEvaluationContext::ReleaseContext();
}
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, Point3D& value)
{
	CoreItemEvaluationContext::SetContext(&mContext);
	*((CoreValue<Point3D>*)mContext.mVariableList[LABEL_TO_ID(input).toUInt()].back().get()) = value;
	mCurrentItem->getValue(value);
	CoreItemEvaluationContext::ReleaseContext();
}
void	CoreItemOperatorModifier::ProtectedCallModifier(CoreModifiableAttribute* caller, Vector4D& value)
{
	CoreItemEvaluationContext::SetContext(&mContext);
	*((CoreValue<Vector4D>*)mContext.mVariableList[LABEL_TO_ID(input).toUInt()].back().get()) = value;
	mCurrentItem->getValue(value);
	CoreItemEvaluationContext::ReleaseContext();
}