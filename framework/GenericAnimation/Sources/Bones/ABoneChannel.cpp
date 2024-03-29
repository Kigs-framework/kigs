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


using namespace Kigs::Anim;
//template class AChannel<PRSKey>;

IMPLEMENT_CLASS_INFO(ABoneChannel)

ABoneChannel::ABoneChannel(const std::string& name,CLASS_NAME_TREE_ARG) : AChannel<PRSKey>(name,PASS_CLASS_NAME_TREE_ARG)
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
	maIntOrphan copythis("this",(int)mGroupID);
	maIntOrphan restype("type",Resource_Bone_Information);
	maIntOrphan result("result",0);
	
	std::vector<CoreModifiableAttribute*> params;
	params.push_back(&copythis);
	params.push_back(&restype);
	params.push_back(&result);
	
	mSystem->GetAObject()->CallMethod("GetResource",params);
	
	PRSKey * prskey=(PRSKey *)((int)result);
	
	if(prskey)
	{
		SP<ABaseStream>    tmp_stream=mSystem->GetValidStream();
		
		tmp_stream->CopyData(&mStandStreamData,prskey);
		
		delete prskey;
	}
};


