// **********************************************************************
// * FILE  : ABoneChannel.cpp
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : animation channel for one bone
// * COMMENT : 
// *---------------------------------------------------------------------
// **********************************************************************

#include "AObject.h"

#include "Bones/ABoneChannel.h"
#include "Bones/APRSKeyStream.h"
#include "Bones/ABoneSystem.h"



//template class AChannel<PRSKey>;

IMPLEMENT_CLASS_INFO(ABoneChannel)

ABoneChannel::ABoneChannel(const kstl::string& name,CLASS_NAME_TREE_ARG) : AChannel<PRSKey>(name,PASS_CLASS_NAME_TREE_ARG)
{
	m_CurrentPRS.SetIdentity();
}


ABoneChannel::~ABoneChannel()
{
	
};

void    ABoneChannel::UpdateTransformParameters()
{
	mLocalToGlobalStreamData.get(m_CurrentPRS);
};


void    ABoneChannel::SetStandStreamData()
{
	maInt copythis(*this,false,"this",(int)mGroupID);
	maInt restype(*this,false,"type",Resource_Bone_Information);
	maInt result(*this,false,"result",0);
	
	kstl::vector<CoreModifiableAttribute*> params;
	params.push_back(&copythis);
	params.push_back(&restype);
	params.push_back(&result);
	
	mSystem->GetAObject()->CallMethod("GetResource",params);
	
	PRSKey * prskey=(PRSKey *)((int)result);
	
	if(prskey)
	{
		ABaseStream*    tmp_stream=mSystem->GetValidStream();
		
		tmp_stream->CopyData(&mStandStreamData,prskey);
		
		delete prskey;
	}
};


