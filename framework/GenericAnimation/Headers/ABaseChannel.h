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
	
	bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;
	
	// ******************************
	// * IsRootChannel
	// *-----------------------------
	/*! return true if the channel has no father
	*/
	// ******************************
	
	bool   IsRootChannel()
	{
		if (mFatherNode == NULL)
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
			if ((*it).mItem->isSubType(ABaseChannel::mClassID))
			{
				(*it).mItem->as< ABaseChannel>()->GetSonGroupIDList(result, count);
			}
		}
	};
	
	
	// ******************************
	// * SetSystem
	// *-----------------------------
	/*! Set the system used by this AChannel
	
	*/
	// ******************************
	
	void    SetSystem(SP<ABaseSystem> sys)
	{
		mSystem = sys;
	};
	
	// ******************************
	// * GetSystem
	// *-----------------------------
	/*! Return a pointer on the system used by this AChannel
	
	*/
	// ******************************
	
	SP<ABaseSystem>    GetSystem()
	{
		return mSystem;
	};
	
	// ******************************
	// * AutoChannelTree
	// *-----------------------------
	/*! construct the channel tree from the hierarchy and a channel array
	*/
	// ******************************
	
	static  void    AutoChannelTree(const std::vector<SP<ABaseChannel>>& channels, SP<AObjectSkeletonResource> hierarchy);
	
	// ******************************
	// * AutoChannelTree
	// *-----------------------------
	/*! construct the channel tree from another system and a channel array
	*/
	// ******************************
	
	static  void    AutoChannelTree(const std::vector<SP<ABaseChannel>>& channels, SP<ABaseSystem> sys);
	
	
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
	
	void            AddStream(SP<ABaseStream> stream);
	
	// ******************************
	// * RemoveStream
	// *-----------------------------
	/*! Remove a stream from the channel stream list
	*/
	// ******************************
	
	void            RemoveStream(SP<ABaseStream> stream);
	
	
	
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
		return mGroupID;
	};
	
	// ******************************
	// * SetGroupID
	// *-----------------------------
	/*! Set the GroupID of the group manage by this channel
	*/
	// ******************************
	
	void            SetGroupID(IntU32 groupID)
	{
		mGroupID = groupID;
	};
	
	IntU32 GetBoneID() const
	{
		return mBoneID;
	}
	
	void SetBoneID(IntU32 id)
	{
		mBoneID = id;
	}
	
	
	virtual LocalToGlobalBaseType* GetChannelLocalToGlobalData() = 0;
	
	SP<ABaseChannel>	mFatherNode;
	IntU32				mSonCount;
	
	IntU32				mGroupID;
	IntU32				mBoneID;
	
	// the first AStream in the list
	SP<ABaseStream>					mFirstStream;
	SP<ABaseSystem>					mSystem;
};



#endif //__ABASECHANNEL_H__


