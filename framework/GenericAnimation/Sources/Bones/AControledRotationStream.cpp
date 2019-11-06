// **********************************************************************
// * FILE  : AControledRotationStream.cpp
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : 
// * COMMENT : 
// *---------------------------------------------------------------------
// **********************************************************************
#include "PrecompiledHeaders.h"

#include "Bones/AControledRotationStream.h"
#include "Bones/ABoneChannel.h"

// ******************************
// * AControledRotationStream
// *-----------------------------
// * - fill data members
// * - 
// ******************************

IMPLEMENT_CLASS_INFO(AControledRotationStream)

AControledRotationStream::AControledRotationStream(const kstl::string& name,CLASS_NAME_TREE_ARG) : APRSStream(name,PASS_CLASS_NAME_TREE_ARG)
{
	m_Priority = 50;
	m_RotationStruct=NULL;
}

AControledRotationStream::~AControledRotationStream()
{

}

// ******************************
// * UpdateData
// *-----------------------------
// * - UpdateData according to the the local time
// * - 
// ******************************

void    AControledRotationStream::UpdateData(LocalToGlobalBaseType* standdata)
{
    PRSKey*             standprs=(PRSKey*)standdata;

	if(m_RotationStruct->m_LastTime==KFLOAT_CONST(-1.0f))
	{
		m_RotationStruct->m_LastTime=m_LocalTime;
	}
	Float delta_t=(Float)(m_LocalTime-m_RotationStruct->m_LastTime);
	m_RotationStruct->m_LastTime=m_LocalTime;

	m_RotationStruct->m_Angle+=m_RotationStruct->m_AngularSpeed*delta_t/KFLOAT_CONST(1000.0f);

	if(m_RotationStruct->m_UseMinMax)
	{
		if(m_RotationStruct->m_Angle>m_RotationStruct->m_MaxAngle)
		{
			m_RotationStruct->m_Angle=m_RotationStruct->m_MaxAngle;
		}

		if(m_RotationStruct->m_Angle<m_RotationStruct->m_MinAngle)
		{
			m_RotationStruct->m_Angle=m_RotationStruct->m_MinAngle;
		}
	}

	

	Quaternion tmpq;

	Float s,c;
	s=sinF((Float)(m_RotationStruct->m_Angle/KFLOAT_CONST(2.)));
	c=cosF((Float)(m_RotationStruct->m_Angle/KFLOAT_CONST(2.)));

	tmpq.V.x=m_RotationStruct->m_Axis[0] * s;
	tmpq.V.y=m_RotationStruct->m_Axis[1] * s;
	tmpq.V.z=m_RotationStruct->m_Axis[2] * s;
	tmpq.w=c;

	m_CurrentPRSKey.m_RotationKey=standprs->m_RotationKey;
	m_CurrentPRSKey.m_RotationKey*=tmpq;



//  m_CurrentPRSKey.m_RotationKey=Inv(standprs->m_RotationKey);
//	m_CurrentPRSKey.m_RotationKey*= tmpq;
//	m_CurrentPRSKey.m_RotationKey*=standprs->m_RotationKey;


    m_CurrentPRSKey.m_PositionKey = standprs->m_PositionKey;
     
    #ifndef NO_SCALE
   
    m_CurrentPRSKey.m_ScaleKey = standprs->m_ScaleKey;
    
    #endif
};

// ******************************
// * LERPData
// *-----------------------------
// * - do : data = (this->data*t)+(data*(1-t))
// * - 
// ******************************

void	AControledRotationStream::LERPData(LocalToGlobalBaseType* data,Float /* t */)
{
	PRSKey *				prsKey=(PRSKey *)data;

	// Calculate new rotation
  
	prsKey->m_RotationKey *= m_CurrentPRSKey.m_RotationKey;
    
}


// ******************************
// * InitFromResource
// *-----------------------------
// * - init the stream with the given resource
// * - from AStream
// ******************************

void    AControledRotationStream::InitFromResource(AnimationResourceInfo* info,IntU32 streamIndex)
{
	//AStreamResourceInfo* ressourceInfo=info->GetStreamResourceInfo(streamIndex);
    
    //m_RotationStruct = (RotationStruct*)( ((char*)ressourceInfo)+sizeof(AStreamResourceInfo) );
	IntU32 group_id_count=info->m_head.m_StreamCount;

    m_RotationStruct = (RotationStruct*)( ((char*)info) + sizeof(AResourceFileHeader) + (sizeof(AStreamResourceInfo**) + sizeof(AStreamResourceInfo))*group_id_count );

    // Length not use
    m_Length = 0xFFFFFF;
};

AnimationResourceInfo* AControledRotationStream::CreateAnimationResourceInfo(IntU32 group_id,Float*& angular_speed, Float*& max_angle,Float*& min_angle,Float*& angle,Float*& axis,bool*& use_min_max)
{
    // create the AnimationResourceInfo instance
  /*  IntU32  total_size=sizeof(AResourceFileHeader);
    total_size += sizeof(AStreamResourceInfo**) + sizeof(AStreamResourceInfo) + sizeof(RotationStruct);
    AnimationResourceInfo*      resource=(AnimationResourceInfo *)(new char[total_size]);

    resource->m_head.m_ID = 'OKTA';
    resource->m_head.m_StreamClassID = ControledRotationStreamClassID;
    resource->m_head.m_StreamSClassID = PRSStreamSuperClassID;
    resource->m_head.m_StreamCount = 1;

    AStreamResourceInfo*   streamInfo=(AStreamResourceInfo*) ( ((char*)resource) +sizeof(resource->m_head) + sizeof(AStreamResourceInfo**));
    resource->SetStreamResourceInfo(streamInfo,0);
    streamInfo->m_StreamGroupID = group_id;

    RotationStruct* localRotationStruct=(RotationStruct*)( ((char*)streamInfo) + sizeof(AStreamResourceInfo));

	angular_speed=&localRotationStruct->m_AngularSpeed;
	max_angle=&localRotationStruct->m_MaxAngle;
	min_angle=&localRotationStruct->m_MinAngle;
	angle=&localRotationStruct->m_Angle;
	localRotationStruct->m_LastTime=-1;

	axis=&localRotationStruct->m_Axis[0];

	use_min_max=&localRotationStruct->m_UseMinMax;

    return resource;*/

	return CreateAnimationResourceInfo(1,&group_id,angular_speed,max_angle,min_angle,angle,axis,use_min_max);


};

AnimationResourceInfo* AControledRotationStream::CreateAnimationResourceInfo(IntU32 group_id_count,IntU32* group_id_list,Float*& angular_speed, Float*& max_angle,Float*& min_angle,Float*& angle,Float*& axis,bool*& use_min_max)
{
	// TODO
	/*
    // create the AnimationResourceInfo instance
    IntU32  total_size=sizeof(AResourceFileHeader);
    total_size += (sizeof(AStreamResourceInfo**) + sizeof(AStreamResourceInfo))*group_id_count + sizeof(RotationStruct);
    AnimationResourceInfo*      resource=(AnimationResourceInfo *)(new char[total_size]);

    resource->m_head.m_ID = 'ATEC';
    resource->m_head.m_StreamClassID = ControledRotationStreamClassID;
    resource->m_head.m_StreamSClassID = PRSStreamSuperClassID;
    resource->m_head.m_StreamCount = group_id_count;

	IntU32 i;
	for(i=0;i<group_id_count;++i)
	{
	   AStreamResourceInfo*   streamInfo=(AStreamResourceInfo*) ( ((char*)resource) +sizeof(resource->m_head) + sizeof(AStreamResourceInfo**)*group_id_count + i*sizeof(AStreamResourceInfo));
	   resource->SetStreamResourceInfo(streamInfo,i);
	   streamInfo->m_StreamGroupID = group_id_list[i];
	}

	RotationStruct* localRotationStruct=(RotationStruct*)( ((char*)resource) + sizeof(AResourceFileHeader) + (sizeof(AStreamResourceInfo**) + sizeof(AStreamResourceInfo))*group_id_count );

	angular_speed=&localRotationStruct->m_AngularSpeed;
	max_angle=&localRotationStruct->m_MaxAngle;
	min_angle=&localRotationStruct->m_MinAngle;
	angle=&localRotationStruct->m_Angle;
	localRotationStruct->m_LastTime=-1;

	axis=&localRotationStruct->m_Axis[0];

	use_min_max=&localRotationStruct->m_UseMinMax;

    return resource;*/

	return 0;
};

