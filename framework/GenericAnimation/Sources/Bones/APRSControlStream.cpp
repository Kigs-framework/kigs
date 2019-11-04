// **********************************************************************
// * FILE  : APRSControlStream.cpp
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : PRS Keyframe animation for one bone
// * COMMENT : 
// *---------------------------------------------------------------------
// **********************************************************************
#include "PrecompiledHeaders.h"

#include "Bones/APRSControlStream.h"
#include "Bones/ABoneChannel.h"

// ******************************
// * APRSControlStream
// *-----------------------------
// * - fill data members
// * - 
// ******************************

IMPLEMENT_CLASS_INFO(APRSControlStream)

APRSControlStream::APRSControlStream(const kstl::string& name,CLASS_NAME_TREE_ARG) : APRSStream(name,PASS_CLASS_NAME_TREE_ARG)
{
	m_pPRSInfo = NULL;
    m_Priority = 50;
}

APRSControlStream::~APRSControlStream()
{

}



// ******************************
// * UpdateData
// *-----------------------------
// * - UpdateData according to the the local time
// * - 
// ******************************

void    APRSControlStream::UpdateData(LocalToGlobalBaseType* standdata)
{
    PRSKey*             standprs=(PRSKey*)standdata;

    if (m_pPRSInfo->m_HasPosition)
    {
        m_CurrentPRSKey.m_PositionKey = m_pPRSInfo->m_PositionKey;
    }
    else
    {
        m_CurrentPRSKey.m_PositionKey = standprs->m_PositionKey;
    }
    if (m_pPRSInfo->m_HasRotation)
    {
        m_CurrentPRSKey.m_RotationKey = m_pPRSInfo->m_RotationKey;
    }
    else
    {
        m_CurrentPRSKey.m_RotationKey = standprs->m_RotationKey;
    }
    #ifndef NO_SCALE
    if (m_pPRSInfo->m_HasScale)
    {
        m_CurrentPRSKey.m_ScaleKey = m_pPRSInfo->m_ScaleKey;
    }
    else
    {
        m_CurrentPRSKey.m_ScaleKey = standprs->m_ScaleKey;
    }
    #endif
};

// ******************************
// * LERPData
// *-----------------------------
// * - do : data = (this->data*t)+(data*(1-t))
// * - 
// ******************************

void	APRSControlStream::LERPData(LocalToGlobalBaseType* data,Float /* t */)
{
	PRSKey *				prsKey=(PRSKey *)data;

	// Calculate new position
    if (m_pPRSInfo->m_HasPosition)
    {
	    prsKey->m_PositionKey += m_CurrentPRSKey.m_PositionKey;
    }
    #ifndef NO_SCALE
	// Calculate new scale
    if (m_pPRSInfo->m_HasScale)
    {
	    prsKey->m_ScaleKey.x *= m_CurrentPRSKey.m_ScaleKey.x;
	    prsKey->m_ScaleKey.y *= m_CurrentPRSKey.m_ScaleKey.y;
	    prsKey->m_ScaleKey.z *= m_CurrentPRSKey.m_ScaleKey.z;
    }
    #endif
	// Calculate new rotation
    if (m_pPRSInfo->m_HasRotation)
    {
	    prsKey->m_RotationKey *= m_CurrentPRSKey.m_RotationKey;
    }
}


// ******************************
// * InitFromResource
// *-----------------------------
// * - init the stream with the given resource
// * - from AStream
// ******************************

void    APRSControlStream::InitFromResource(AnimationResourceInfo* info,IntU32 streamIndex)
{
    AStreamResourceInfo* ressourceInfo=info->GetStreamResourceInfo(streamIndex);
  
	m_pPRSInfo = (PRSControllerKey*)(ressourceInfo->getData());
    // Length not use
    m_Length = 0xFFFFFF;
};

AnimationResourceInfo* APRSControlStream::CreateAnimationResourceInfo(IntU32 group_id)
{
	return CreateAnimationResourceInfo(1,&group_id);
};

AnimationResourceInfo* APRSControlStream::CreateAnimationResourceInfo(IntU32 group_id_count,IntU32* group_id_list)
{
	kstl::string	currentType = "APRSControlStream";
    // create the AnimationResourceInfo instance
	// first compute size
    IntU32  total_size=sizeof(AResourceFileHeader); // header
	total_size += currentType.length(); // string name length
	total_size += group_id_count * sizeof(IntU32) * 2; // group count * (sizeof(offset) + sizeof(size))
    total_size += (sizeof(AStreamResourceInfo)+ sizeof(PRSControllerKey)) * group_id_count; // data * group count

    AnimationResourceInfo*      resource=(AnimationResourceInfo *)(new char[total_size]);

    resource->m_head.m_ID = 'ATEC';
    resource->m_head.m_StreamCount = group_id_count;
	resource->setStreamType(currentType);

	IntU32 i;
	for(i=0;i<group_id_count;++i)
	{
		AStreamResourceInfo* streamInfo=resource->SetStreamResourceInfo(i, sizeof(PRSControllerKey)+sizeof(AStreamResourceInfo));
		streamInfo->m_StreamGroupID = group_id_list[i];

		// init

		PRSControllerKey* key = (PRSControllerKey*)(streamInfo->getData());
		key->m_HasPosition = true;
		key->m_HasRotation = true;
		key->m_HasScale = true;
		key->m_PositionKey.Set(0.0f);
		key->m_RotationKey.SetIdentity();
		key->m_ScaleKey.Set(1.0f, 1.0f, 1.0f);
	}

    return resource;
};


