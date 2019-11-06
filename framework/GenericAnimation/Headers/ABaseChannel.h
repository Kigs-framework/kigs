// ****************************************************************************
// * NAME: AChannel.h
// * GROUP: Animation module
// * --------------------------------------------------------------------------
// * PURPOSE:   AChannel manage animation on one group for a given system. channels
// *            are organised in tree.
// *
// * COMMENT:
// * --------------------------------------------------------------------------
// * RELEASE:
// ****************************************************************************

#ifndef __ABASECHANNEL_H__
#define __ABASECHANNEL_H__

#include "AMDefines.h"
#include "CoreModifiable.h"

// +---------
// | Declare some usefull classes
// +---------
/*
class   AGroupID;
class   AEntitySet;
class   AIndexInterval;
class   AObject;
*/

//template<typename T> class AStream;
//template<typename T> class ASystem;
class ABaseSystem;
class ABaseStream;
class AObjectSkeletonResource;
class LocalToGlobalBaseType;

// ASystem is template to ABaseSystem can be use to have pointer without template parameters
class   ABaseChannel : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(ABaseChannel, CoreModifiable, Animation);	
	DECLARE_CONSTRUCTOR(ABaseChannel);
	
	bool	addItem(CoreModifiable *item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool	removeItem(CoreModifiable *item DECLARE_DEFAULT_LINK_NAME) override;
	
	// ******************************
	// * IsRootChannel
	// *-----------------------------
	/*! return true if the channel has no father
	*/
	// ******************************
	
	bool   IsRootChannel()
	{
		if (mp_FatherNode == NULL)
		{
			return true;
		}
		return false;
	};
	
	// ******************************
	// * GetLocalToGlobalBeforeChange
	// *-----------------------------
	/*! Used to change root channel data when weight are changed, or when
	a animation is add or sub ...
	*/
	// ******************************
	
	virtual LocalToGlobalBaseType*   GetLocalToGlobalBeforeChange() = 0;
	
	// ******************************
	// * ResetLocalToGlobalAfterChange
	// *-----------------------------
	/*! After the weight change or sub/add animation done, reset
	the local to global data, so the animation does not "jump"
	*/
	// ******************************
	
	virtual void    ResetLocalToGlobalAfterChange(LocalToGlobalBaseType* tmp_data) = 0;
	
	
	// ******************************
	// * GetSonGroupIDList
	// *-----------------------------
	/*!  create an array of GroupId including th group id's of this and
	all its sons
	*/
	// ******************************
	
	void    GetSonGroupIDList(IntU32* result, IntU32& count)
	{
		result[count++] = GetGroupID();
		
		kstl::vector<ModifiableItemStruct>::const_iterator it;
		
		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			if ((*it).myItem->isSubType(ABaseChannel::myClassID))
			{
				((ABaseChannel*)(*it).myItem)->GetSonGroupIDList(result, count);
			}
		}
	};
	
	
	// ******************************
	// * SetSystem
	// *-----------------------------
	/*! Set the system used by this AChannel
	
	*/
	// ******************************
	
	void    SetSystem(ABaseSystem* sys)
	{
		m_pSystem = sys;
	};
	
	// ******************************
	// * GetSystem
	// *-----------------------------
	/*! Return a pointer on the system used by this AChannel
	
	*/
	// ******************************
	
	ABaseSystem*    GetSystem()
	{
		return m_pSystem;
	};
	
	// ******************************
	// * AutoChannelTree
	// *-----------------------------
	/*! construct the channel tree from the hierarchy and a channel array
	*/
	// ******************************
	
	static  void    AutoChannelTree(ABaseChannel** channels, AObjectSkeletonResource* hierarchy);
	
	// ******************************
	// * AutoChannelTree
	// *-----------------------------
	/*! construct the channel tree from another system and a channel array
	*/
	// ******************************
	
	static  void    AutoChannelTree(ABaseChannel** channels, ABaseSystem *sys);
	
	
	//protected:
	
	virtual ~ABaseChannel()
	{
	}
	
	// ******************************
	// * Stream management
	// *-----------------------------
	// * - add/sub streams, the streams are sorted by priority when added
	// * -
	// ******************************
	
	// ******************************
	// * AddStream
	// *-----------------------------
	/*! Add a stream to the channel stream list
	*/
	// ******************************
	
	void            AddStream(ABaseStream* stream);
	
	// ******************************
	// * RemoveStream
	// *-----------------------------
	/*! Remove a stream from the channel stream list
	*/
	// ******************************
	
	void            RemoveStream(ABaseStream* stream);
	
	
	
	// ******************************
	// * GetStandData
	// *-----------------------------
	/*! Return a pointer on this AChannel stand data
	*/
	// ******************************
	
	virtual LocalToGlobalBaseType*       GetStandData() = 0;
	
	
	// ******************************
	// * SetStandData
	// *-----------------------------
	/*! set the stand data for the channel
	*/
	// ******************************
	
	virtual void       SetStandData(LocalToGlobalBaseType* stddata) = 0;
	
	// ******************************
	// * Get/Set methods
	// *-----------------------------
	// * - access protected parameters
	// * -
	// ******************************
	
	// ******************************
	// * GetGroupID
	// *-----------------------------
	/*! return the GroupID of the group manage by this channel
	*/
	// ******************************
	
	IntU32       GetGroupID() const
	{
		return m_GroupID;
	};
	
	// ******************************
	// * SetGroupID
	// *-----------------------------
	/*! Set the GroupID of the group manage by this channel
	*/
	// ******************************
	
	void            SetGroupID(IntU32 groupID)
	{
		m_GroupID = groupID;
	};
	
	IntU32 GetBoneID() const
	{
		return m_BoneID;
	}
	
	void SetBoneID(IntU32 id)
	{
		m_BoneID = id;
	}
	
	
	virtual LocalToGlobalBaseType* GetChannelLocalToGlobalData() = 0;
	
	ABaseChannel*	mp_FatherNode;
	IntU32		m_SonCount;
	
	IntU32		m_GroupID;
	IntU32		m_BoneID;
	
	// the first AStream in the list
	ABaseStream*					m_pFirstStream;
	ABaseSystem*					m_pSystem;
};



#endif //__ABASECHANNEL_H__


