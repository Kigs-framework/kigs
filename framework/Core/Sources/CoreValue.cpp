#include "PrecompiledHeaders.h"
#include "CoreValue.h"

#include "CoreModifiableAttribute.h"
#include "maUSString.h"


template<>
CoreModifiableAttribute* CoreValue<bool>::createAttribute(CoreModifiable* target)
{
	return new maBool(*target, false, LABEL_AND_ID(Val), this->m_Value);
}

template<>
CoreModifiableAttribute* CoreValue<kfloat>::createAttribute(CoreModifiable* target)
{
	return new maFloat(*target, false, LABEL_AND_ID(Val), this->m_Value);
}

template<>
CoreModifiableAttribute* CoreValue<kdouble>::createAttribute(CoreModifiable* target)
{
	return new maFloat(*target, false, LABEL_AND_ID(Val), (kfloat)this->m_Value);
}

template<>
CoreModifiableAttribute* CoreValue<Point2D>::createAttribute(CoreModifiable* target)
{
	return new maVect2DF(*target, false, LABEL_AND_ID(Val), &(this->m_Value.x));
}

template<>
CoreModifiableAttribute* CoreValue<Point3D>::createAttribute(CoreModifiable* target)
{
	return new maVect3DF(*target, false, LABEL_AND_ID(Val), &(this->m_Value.x));
}

template<>
CoreModifiableAttribute* CoreValue<Vector4D>::createAttribute(CoreModifiable* target)
{
	return new maVect4DF(*target, false, LABEL_AND_ID(Val), &(this->m_Value.x));
}

template<>
CoreModifiableAttribute* CoreValue<int>::createAttribute(CoreModifiable* target)
{
	return new maInt(*target, false, LABEL_AND_ID(Val), (this->m_Value));
}

template<>
CoreModifiableAttribute* CoreValue<unsigned int>::createAttribute(CoreModifiable* target)
{
	return new maUInt(*target, false, LABEL_AND_ID(Val), (this->m_Value));
}

template<>
CoreModifiableAttribute* CoreValue<kstl::string>::createAttribute(CoreModifiable* target)
{
	return new maString(*target, false, LABEL_AND_ID(Val), (this->m_Value));
}

template<>
CoreModifiableAttribute* CoreValue<usString>::createAttribute(CoreModifiable* target)
{
	return new maUSString(*target, false, LABEL_AND_ID(Val), (this->m_Value));
}


