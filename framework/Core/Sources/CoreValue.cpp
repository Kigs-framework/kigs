#include "PrecompiledHeaders.h"
#include "CoreValue.h"

#include "CoreModifiableAttribute.h"
#include "maUSString.h"

using namespace Kigs;
using namespace Kigs::Core;

template<>
CoreModifiableAttribute* CoreValue<bool>::createAttribute()
{
	return new maBoolOrphan("Val", this->mValue);
}

template<>
CoreModifiableAttribute* CoreValue<f32>::createAttribute()
{
	return new maFloatOrphan("Val", this->mValue);
}

template<>
CoreModifiableAttribute* CoreValue<f64>::createAttribute()
{
	return new maFloatOrphan("Val", (float)this->mValue);
}

template<>
CoreModifiableAttribute* CoreValue<v2f>::createAttribute()
{
	return new maVect2DFOrphan("Val", &(this->mValue.x));
}

template<>
CoreModifiableAttribute* CoreValue<v3f>::createAttribute()
{
	return new maVect3DFOrphan("Val", &(this->mValue.x));
}

template<>
CoreModifiableAttribute* CoreValue<v4f>::createAttribute()
{
	return new maVect4DFOrphan("Val", &(this->mValue.x));
}

template<>
CoreModifiableAttribute* CoreValue<int>::createAttribute()
{
	return new maIntOrphan("Val", (this->mValue));
}
template<>
CoreModifiableAttribute* CoreValue<s64>::createAttribute()
{
	return new maLongOrphan("Val", (this->mValue));
}
template<>
CoreModifiableAttribute* CoreValue<unsigned int>::createAttribute()
{
	return new maUIntOrphan("Val", (this->mValue));
}
template<>
CoreModifiableAttribute* CoreValue<u64>::createAttribute()
{
	return new maULongOrphan("Val", (this->mValue));
}
template<>
CoreModifiableAttribute* CoreValue<std::string>::createAttribute()
{
	return new maStringOrphan("Val", (this->mValue));
}

template<>
CoreModifiableAttribute* CoreValue<usString>::createAttribute()
{
	return new maUSStringOrphan("Val", (this->mValue));
}


