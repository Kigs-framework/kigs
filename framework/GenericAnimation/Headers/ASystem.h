// ****************************************************************************
// * NAME: ASystem.h
// * GROUP: Animation module
// * --------------------------------------------------------------------------
// * PURPOSE:
// *
// * COMMENT:
// * --------------------------------------------------------------------------
// * RELEASE:
// ****************************************************************************


#ifndef __ASYSTEM_H__
#define __ASYSTEM_H__

#include "AMDefines.h"
#include "ABaseChannel.h"
#include "ABaseStream.h"
#include "ABaseSystem.h"

#include "CoreModifiableAttribute.h"

// +---------
// | Declare some usefull classes
// +---------

class   AObjectSkeletonResource;
class   AObject;
//class	ABaseChannel;

template<typename LocalToGlobalType>
class   ASystem : public ABaseSystem
{
	DECLARE_ABSTRACT_CLASS_INFO(ASystem, ABaseSystem, Animation)
public:
	DECLARE_CONSTRUCTOR(ASystem);
	
    // ----------------------------------------------------------------------------
	// ******************************
	// * Local to Global management
	// *-----------------------------
	/*!  called by AObject to set the object local to global data
	*/
	// ******************************
	void    SetLocalToGlobalData(LocalToGlobalBaseType* new_data);
	
	// ******************************
	// * Local to Global management
	// *-----------------------------
	/*!  return a pointer on the instant local to global data
	*/
	// ******************************
	
	LocalToGlobalBaseType*   GetLocalToGlobalData()
	{
		return &m_pInstantLocalToGlobalData;
	};
	
	
	// *******************
	// * LinkTo
	// * -----------------
	/*!  use the given data as the local to global data
	*/
	// *******************
	
	void LinkTo(ABaseChannel* data) override;
	
	// *******************
	// * UnLink
	// * -----------------
	/*!  stop using the link data
	*/
	// *******************
	
	void UnLink() override;
	
	
	//protected:
	
	virtual ~ASystem()
	{
		if(mChannelTab != NULL)
        {
            DeleteChannelTree();
            delete[] mChannelTab;
            mChannelTab=NULL;
        }
		
	}
	
	void    UseAnimationLocalToGlobalData(bool b) override;
	
	
	
	// ******************************
	// * LocalToGlobal data management
	// *-----------------------------
	// * - protected members
	// * - 
	// ******************************
	
	LocalToGlobalType			 m_pInstantLocalToGlobalData;		// current computed local to global
	LocalToGlobalType			 m_pStartingLocalToGlobalData;	   // local to global at init
	ABaseChannel*	     		m_LinkedChannel;
	
};

IMPLEMENT_TEMPLATE_CLASS_INFO(LocalToGlobalType, ASystem)

template<typename LocalToGlobalType>
ASystem<LocalToGlobalType>::ASystem(const kstl::string& name, CLASS_NAME_TREE_ARG)
: ABaseSystem(name, PASS_CLASS_NAME_TREE_ARG)
, m_LinkedChannel(0)
{
	
	
}

// ******************************
// * Local to Global management
// *-----------------------------
// * - 
// * - 
// ******************************
template<typename LocalToGlobalType>
void    ASystem<LocalToGlobalType>::SetLocalToGlobalData(LocalToGlobalBaseType* new_data)
{
	ABaseStream* tmp_stream = GetValidStream();
	if (tmp_stream != NULL)
	{
		if (mUseAnimationLocalToGlobal == false)
		{
			// just copy data 
			tmp_stream->CopyData(&m_pInstantLocalToGlobalData, new_data);
		}
		else
		{
			tmp_stream->SetAndModifyData(&m_pInstantLocalToGlobalData, new_data, &m_pStartingLocalToGlobalData);
		}
	}
};

template<typename LocalToGlobalType>
void   ASystem<LocalToGlobalType>::UseAnimationLocalToGlobalData(bool b)
{
	mUseAnimationLocalToGlobal = b;
	if (b == false)
	{
		
		// TODO
		/*
		maInt copythis(*this, false, "this", (int)this);
		maInt restype(*this, false, "type", Local_To_Global_Data);
		maInt result(*this, false, "result", 0);
		
		kstl::vector<CoreModifiableAttribute*> params;
		params.push_back(&copythis);
		params.push_back(&restype);
		params.push_back(&result);
		
		GetAObject()->GetObject()->CallMethod("GetResource", params);
		
		m_pInstantLocalToGlobalData = (void*)((int)result);
		*/
	}
};


// ----------------------------------------------------------------------------

// *******************
// * LinkTo
// * -----------------
// *    use the given data as the local to global data
// * 
// *******************
template<typename LocalToGlobalType>
void    ASystem<LocalToGlobalType>::LinkTo(ABaseChannel* data)
{
	
	m_LinkedChannel = data;
	mUseAnimationLocalToGlobal = false;
	mUpdateLocalToGlobalWhenLoop = false;
	
};

template<typename LocalToGlobalType>
void ASystem<LocalToGlobalType>::UnLink()
{
	m_LinkedChannel = NULL;
};




#endif //__ASYSTEM_H__


