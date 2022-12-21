// **********************************************************************
// * FILE  : APRSStream.cpp
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : PRS animation for one bone
// * COMMENT : 
// *---------------------------------------------------------------------
// **********************************************************************
#include "PrecompiledHeaders.h"

#include "Bones/APRSStream.h"
#include "Bones/ABoneChannel.h"

//template class AStream<PRSKey>;

IMPLEMENT_CLASS_INFO(APRSStream)

APRSStream::APRSStream(const std::string& name,CLASS_NAME_TREE_ARG) : AStream<PRSKey>(name,PASS_CLASS_NAME_TREE_ARG)
{
}

APRSStream::~APRSStream()
{

}


// ******************************
// * NewStreamOutputDataInstance
// *-----------------------------
// * - return an instance of the stream output data  
// * -  from AStream used by the channel to work on output data ( mixing, offset ... )
// ******************************

LocalToGlobalBaseType*   APRSStream::NewStreamOutputDataInstance()
{
    return (LocalToGlobalBaseType*)new PRSKey;
};

// ******************************
// * DeleteStreamOutputDataInstance
// *-----------------------------
// * - destroy an output data  
// * - from AStream
// ******************************

void    APRSStream::DeleteStreamOutputDataInstance(LocalToGlobalBaseType* data)
{
	delete (PRSKey *)data;
};

// ******************************
// * SetAndModifyData
// *-----------------------------
// * - currentData is the the current PRSKey in the global coordinate space
// * - goalData is the global PRSKey where we would like the current PRSKey to move to 
// * - startAnimData is the PRSKey where the animation started at the end of the last loop
// * - the animation moved the root PRSKey from startAnimData to currentData
// * - this method computes a new startAnimData so that at the current time, the animation
// *   will move the PRSKey precisely to the goal PRSKey.
// * - as an example this method allows to rotate around the current root PRSKey instead
// *   of rotating around the startAnim Key.
// * - at the end the currentData is set to the given goadData
// ******************************

void	APRSStream::SetAndModifyData(LocalToGlobalBaseType* currentData, LocalToGlobalBaseType* goalData, LocalToGlobalBaseType* startAnimData)
{
	PRSKey*					key1=(PRSKey*)currentData;
	PRSKey*					key2=(PRSKey*)goalData;
	PRSKey*					modifiedKey=(PRSKey*)startAnimData;

	AMQuat				    quatDiffBetweenKey1AndKey2=Inv(key1->m_RotationKey)*key2->m_RotationKey;

	AMPoint3				relativePosOfModifiedKey;

	// Calculate distance between ModifiedKey and Key1
	relativePosOfModifiedKey = modifiedKey->m_PositionKey;
	relativePosOfModifiedKey -= key1->m_PositionKey;

	// Calculate rotation between Key1 and key2
	AMMatrix TmpMatrix(quatDiffBetweenKey1AndKey2);
	// Rotate the modifiedKey around Key1
    relativePosOfModifiedKey = TmpMatrix * relativePosOfModifiedKey;
	// Rotate the ModifiedKey
	modifiedKey->m_RotationKey = quatDiffBetweenKey1AndKey2 * modifiedKey->m_RotationKey; 
    modifiedKey->m_RotationKey.Normalize();

	// Adjust position of the ModifiedKey
	modifiedKey->m_PositionKey = relativePosOfModifiedKey + key2->m_PositionKey;

	#ifndef NO_SCALE
	// Adjust scale of the ModifiedKey
	modifiedKey->m_ScaleKey.x *= key2->m_ScaleKey.x / key1->m_ScaleKey.x;
	modifiedKey->m_ScaleKey.y *= key2->m_ScaleKey.y / key1->m_ScaleKey.y;
	modifiedKey->m_ScaleKey.z *= key2->m_ScaleKey.z / key1->m_ScaleKey.z;
    #endif

	// Copy data1 to data2
	(*key1) = (*key2);
}


void	PRSKey::set(const Matrix3x4& m)
{
	Vector3D	getScale(1.0f, 0.0f, 0.0f);
	m.TransformVector(&getScale);

	m_PositionKey = m.GetTranslation();
	m_RotationKey = (Quaternion)m;
	m_RotationKey.Normalize();
	m_ScaleKey = Norm(getScale);
}
void	PRSKey::get(Matrix3x4& m)
{
	m.Set(m_RotationKey);
	m.SetTranslation(m_PositionKey);
	m.PostScale(m_ScaleKey.x, m_ScaleKey.y, m_ScaleKey.z);
}




//  +-------------
//  | APRSStream methods      ------------------------------------------------------------------------------
//  +-------------

// ******************************
// * InitData
// *-----------------------------
// * - 
// * - 
// ******************************

void    APRSStream::InitData(LocalToGlobalBaseType* data)
{
	PRSKey *				prsKey = (PRSKey *)data;

	prsKey->m_PositionKey = m_CurrentPRSKey.m_PositionKey;
	prsKey->m_RotationKey = m_CurrentPRSKey.m_RotationKey;
#ifndef NO_SCALE
	prsKey->m_ScaleKey = m_CurrentPRSKey.m_ScaleKey;
#endif
};

// ******************************
// * LERPData
// *-----------------------------
// * - do : data = (this->data*t)+(data*(1-t))
// * - 
// ******************************

void	APRSStream::LERPData(LocalToGlobalBaseType* data, Float t)
{
	PRSKey *				prsKey = (PRSKey *)data;

	// Calculate new position
	prsKey->m_PositionKey *= (1.0f) - t;
	prsKey->m_PositionKey += m_CurrentPRSKey.m_PositionKey * t;
#ifndef NO_SCALE
	// Calculate new scale
	prsKey->m_ScaleKey *= (1.0f) - t;
	prsKey->m_ScaleKey += m_CurrentPRSKey.m_ScaleKey * t;
#endif
	// Calculate new rotation
	prsKey->m_RotationKey = Slerp(prsKey->m_RotationKey, m_CurrentPRSKey.m_RotationKey, t);
}

// ******************************
// * GetStreamLength
// *-----------------------------
// * - return the length of the stream
// * - virtual
// ******************************

ATimeValue    APRSStream::GetStreamLength()
{
	return m_Length;
}

// ******************************
// * GetSystemClassID
// *-----------------------------
// * - return the class id of the system to use with this stream
// * - from AStream
// ******************************

std::string    APRSStream::GetSystemType()
{
	return "ABoneSystem";
};






// ******************************
// * IdentityData
// *-----------------------------
// * - 
// * - 
// ******************************

void APRSStream::IdentityData(LocalToGlobalBaseType* data)
{
	PRSKey *				prsKey = (PRSKey *)data;

	prsKey->m_PositionKey.x = prsKey->m_PositionKey.y = prsKey->m_PositionKey.z = 0.0f;

	prsKey->m_RotationKey.SetIdentity();

#ifndef NO_SCALE
	prsKey->m_ScaleKey.x = prsKey->m_ScaleKey.y = prsKey->m_ScaleKey.z = 1.0f;
#endif
};

// ******************************
// * MulData
// *-----------------------------
// * - used to multiply two PRS keys
// * - when this method is used for PRSkey transformations, the arguments have
// * - the following meaning :
// * -     destination = MulData(origin, transformation)
// * - since transformations are not commutative, its important to respect this
// * - order so that Qr1 = Qr2*Qr1 (Qr = Quaternion rotation)
// * - the result is put in data1 which is overwritten.
// ******************************

void	APRSStream::MulData(LocalToGlobalBaseType* inout, LocalToGlobalBaseType* input)
{
	PRSKey *				prsKey1 = (PRSKey *)inout;
	PRSKey *				prsKey2 = (PRSKey *)input;
	/*AMPoint3				tmpPoint;

	#ifndef NO_SCALE
	prsKey1->m_ScaleKey.x *= prsKey2->m_ScaleKey.x;
	prsKey1->m_ScaleKey.y *= prsKey2->m_ScaleKey.y;
	prsKey1->m_ScaleKey.z *= prsKey2->m_ScaleKey.z;
	#endif

	prsKey1->m_RotationKey = prsKey1->m_RotationKey * prsKey2->m_RotationKey;
	//prsKey1->m_RotationKey = prsKey2->m_RotationKey * prsKey1->m_RotationKey;
	AMMatrix TmpMatrix(prsKey2->m_RotationKey);

	#ifdef NO_SCALE
	tmpPoint = TmpMatrix * prsKey1->m_PositionKey;
	#else
	tmpPoint.x = prsKey2->m_ScaleKey.x * prsKey1->m_PositionKey.x;
	tmpPoint.y = prsKey2->m_ScaleKey.y * prsKey1->m_PositionKey.y;
	tmpPoint.z = prsKey2->m_ScaleKey.z * prsKey1->m_PositionKey.z;
	tmpPoint = TmpMatrix * tmpPoint;
	#endif

	tmpPoint += prsKey2->m_PositionKey;
	prsKey1->m_PositionKey = tmpPoint;*/

	// local transform
	Matrix3x4 local_transform(prsKey1->m_RotationKey, prsKey1->m_PositionKey);
	local_transform.PostScale(prsKey1->m_ScaleKey.x, prsKey1->m_ScaleKey.y, prsKey1->m_ScaleKey.z);

	// parent transform
	Matrix3x4 parent_transform(prsKey2->m_RotationKey, prsKey2->m_PositionKey);
	parent_transform.PostScale(prsKey2->m_ScaleKey.x, prsKey2->m_ScaleKey.y, prsKey2->m_ScaleKey.z);

	// compute multiplication
	Matrix3x4 global_transform = parent_transform *local_transform;

	// set output
	prsKey1->set(global_transform);
}

// ******************************
// * MulInvData
// *-----------------------------
// * - data1 = data1*(1/data2)
// * - when this method is used for PRSkey transformations, it enables to
// * - recover the origin PRSKey given a destination key and a transformation key :
// * -   origin = MulInvData(dest,transformation);
// * - Calling MulData and MulInvData as following enables to find the origin key again :    
// *     dest = MulData(origin,transformation);
// *     origin = MulInvData(dest,transformation);
// * - Do not mix up MulInvData and MulInvData2 which have different definitions.
// ******************************

void	APRSStream::MulInvData(LocalToGlobalBaseType* inout, LocalToGlobalBaseType* input)
{
	PRSKey *				prsKey1 = (PRSKey *)inout;
	PRSKey *				prsKey2 = (PRSKey *)input;
	AMPoint3				tmpPoint;

	AMQuat				    invR2 = Inv(prsKey2->m_RotationKey);

	// scale
#ifndef NO_SCALE
	prsKey1->m_ScaleKey.x /= prsKey2->m_ScaleKey.x;
	prsKey1->m_ScaleKey.y /= prsKey2->m_ScaleKey.y;
	prsKey1->m_ScaleKey.z /= prsKey2->m_ScaleKey.z;
#endif

	// rotation
	prsKey1->m_RotationKey = invR2 * prsKey1->m_RotationKey;

	// A VERIFIER
	// translation
	AMMatrix TmpMatrix(invR2);

	tmpPoint.x = prsKey1->m_PositionKey.x - prsKey2->m_PositionKey.x;
	tmpPoint.y = prsKey1->m_PositionKey.y - prsKey2->m_PositionKey.y;
	tmpPoint.z = prsKey1->m_PositionKey.z - prsKey2->m_PositionKey.z;
	tmpPoint = TmpMatrix * tmpPoint;
#ifndef NO_SCALE
	tmpPoint.x /= prsKey2->m_ScaleKey.x;
	tmpPoint.y /= prsKey2->m_ScaleKey.y;
	tmpPoint.z /= prsKey2->m_ScaleKey.z;
#endif

	prsKey1->m_PositionKey = tmpPoint;
}

// ******************************
// * MulInvData2
// *-----------------------------
// * - data1 = data1*(1/data2)
// * - when this method is used for PRSkey transformations, it enables to
// * - recover the transformation PRSKey given a destination key and an origin key :
// * -   transformation = MulInvData2(dest,origin)
// * - Calling MulData and MulInvData2 as following enables to find the transformation key again :    
// *     dest = MulData(origin,transformation);
// *     transformation = MulInvData2(dest,origin);
// * - Do not mix up MulInvData and MulInvData2 which have different definitions.
// ******************************

void	APRSStream::MulInvData2(LocalToGlobalBaseType* data1, LocalToGlobalBaseType* data2)
{
	PRSKey *				prsKey1 = (PRSKey *)data1;
	PRSKey *				prsKey2 = (PRSKey *)data2;
	AMPoint3				tmpPoint;

	AMQuat				    invR2 = Inv(prsKey2->m_RotationKey);

	// scale
#ifndef NO_SCALE
	prsKey1->m_ScaleKey.x /= prsKey2->m_ScaleKey.x;
	prsKey1->m_ScaleKey.y /= prsKey2->m_ScaleKey.y;
	prsKey1->m_ScaleKey.z /= prsKey2->m_ScaleKey.z;
#endif

	// rotation
	// be careful, here we must post-concatenate quaternions for inversion 
	// to compensate for the transformation pre-concatenation
	prsKey1->m_RotationKey *= invR2;

	// translation
	AMMatrix TmpMatrix(prsKey1->m_RotationKey);

	tmpPoint.x = prsKey2->m_PositionKey.x;
	tmpPoint.y = prsKey2->m_PositionKey.y;
	tmpPoint.z = prsKey2->m_PositionKey.z;
#ifndef NO_SCALE
	tmpPoint.x *= prsKey1->m_ScaleKey.x;
	tmpPoint.y *= prsKey1->m_ScaleKey.y;
	tmpPoint.z *= prsKey1->m_ScaleKey.z;
#endif

	tmpPoint = TmpMatrix * tmpPoint;
	prsKey1->m_PositionKey = prsKey1->m_PositionKey - tmpPoint;
}

// ******************************
// * CopyData
// *-----------------------------
// * - do : data1 = data2
// * - 
// ******************************

void	APRSStream::CopyData(LocalToGlobalBaseType* dst, LocalToGlobalBaseType* src)
{
	*((PRSKey *)dst) = *((PRSKey *)src);
}



bool APRSStream::SameData(LocalToGlobalBaseType* data, LocalToGlobalBaseType* other_data)
{
	PRSKey* a = (PRSKey*)data;
	PRSKey* b = (PRSKey*)other_data;
	return a->m_PositionKey == b->m_PositionKey && a->m_RotationKey == b->m_RotationKey && a->m_ScaleKey == b->m_ScaleKey;
}

