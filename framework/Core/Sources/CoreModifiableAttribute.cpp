#include "PrecompiledHeaders.h"

#include "CoreModifiableAttribute.h"
#include "CoreModifiable.h"
#include "DoNothingObject.h"

#include "maReference.h"

#include "Core.h"

#include <algorithm>

using namespace Kigs::Core;

//! convert enum to readable string
std::string CoreModifiableAttribute::typeToString(CoreModifiable::ATTRIBUTE_TYPE typ)
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

CoreModifiableAttribute::~CoreModifiableAttribute()
{
	CoreModifiable* owner = getOwner();
	if (owner)
	{
		unordered_map<KigsID, CoreModifiableAttribute*>::const_iterator it = owner->mAttributes.find(mID);
		if (it != owner->mAttributes.end())
		{
			owner->mAttributes.erase(it);
		}
	}
};


//! convert string to type enum
CoreModifiable::ATTRIBUTE_TYPE CoreModifiableAttribute::stringToType(const std::string_view & typ_view)
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


void CoreModifiableAttribute::ParseAttributePath(const std::string &path, std::string & CoreModifiablePath, std::string & CoreModifiableAttributeLabel)
{
	CoreModifiablePath = "";
	CoreModifiableAttributeLabel = "";
	std::string::size_type posPoint = path.find_last_of("->");

	if (posPoint != std::string::npos)
	{
		posPoint -= 1;
		CoreModifiablePath = path.substr(0, posPoint);
		CoreModifiableAttributeLabel = path.substr(posPoint + 2, path.length() - posPoint - 2);
	}
}
