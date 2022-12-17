#include "PrecompiledHeaders.h"
#include "CoreValue.h"

#include "CoreModifiableAttribute.h"
#include "maUSString.h"


template<>
CoreModifiableAttribute* CoreValue<bool>::createAttribute(CoreModifiable* target)
{
	return new maBool(*target, false, "Val", this->mValue);
}

template<>
CoreModifiableAttribute* CoreValue<float>::createAttribute(CoreModifiable* target)
{
	return new maFloat(*target, false, "Val", this->mValue);
}

template<>
CoreModifiableAttribute* CoreValue<double>::createAttribute(CoreModifiable* target)
{
	return new maFloat(*target, false, "Val", (float)this->mValue);
}

template<>
CoreModifiableAttribute* CoreValue<Point2D>::createAttribute(CoreModifiable* target)
{
	return new maVect2DF(*target, false, "Val", &(this->mValue.x));
}

template<>
CoreModifiableAttribute* CoreValue<Point3D>::createAttribute(CoreModifiable* target)
{
	return new maVect3DF(*target, false, "Val", &(this->mValue.x));
}

template<>
CoreModifiableAttribute* CoreValue<Vector4D>::createAttribute(CoreModifiable* target)
{
	return new maVect4DF(*target, false, "Val", &(this->mValue.x));
}

template<>
CoreModifiableAttribute* CoreValue<int>::createAttribute(CoreModifiable* target)
{
	return new maInt(*target, false, "Val", (this->mValue));
}
template<>
CoreModifiableAttribute* CoreValue<s64>::createAttribute(CoreModifiable* target)
{
	return new maLong(*target, false, "Val", (this->mValue));
}
template<>
CoreModifiableAttribute* CoreValue<unsigned int>::createAttribute(CoreModifiable* target)
{
	return new maUInt(*target, false, "Val", (this->mValue));
}
template<>
CoreModifiableAttribute* CoreValue<u64>::createAttribute(CoreModifiable* target)
{
	return new maULong(*target, false, "Val", (this->mValue));
}
template<>
CoreModifiableAttribute* CoreValue<std::string>::createAttribute(CoreModifiable* target)
{
	return new maString(*target, false, "Val", (this->mValue));
}

template<>
CoreModifiableAttribute* CoreValue<usString>::createAttribute(CoreModifiable* target)
{
	return new maUSString(*target, false, "Val", (this->mValue));
}


