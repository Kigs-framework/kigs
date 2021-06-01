#ifndef __ABASESYSTEM_H__
#define __ABASESYSTEM_H__


#include "AMDefines.h"

#include "CoreModifiableAttribute.h"
#include "ABaseChannel.h"

class LocalToGlobalBaseType
{
	public:
	LocalToGlobalBaseType()
	{
		
	}
};

class AObject;
class ABaseStream;
class AObjectSkeletonResource;

// +---------
// | ASystem declaration
/*!  Can not be used directly,
base system
\ingroup Animation
*/

// ASystem is template to ABaseSystem can be use to have pointer without template parameters
class   ABaseSystem : public CoreModifiable
{
	DECLARE_ABSTRACT_CLASS_INFO(ABaseSystem, CoreModifiable, Animation);
public:
	DECLARE_CONSTRUCTOR(ABaseSystem);
	
	int getPriority() const
	{
		return mPriority;
	}
	
	// ******************************
	// * GetAObject
	// *-----------------------------
	/*! return a pointer on the AObject linked to this system
	*/
	// ******************************
	
	SP<AObject>    GetAObject()
	{
		return mAObject;
	};
	
	
	// ******************************
	// * GetChannel
	// *-----------------------------
	/*! return the channel acting on the given group ID
	*/
	// ******************************
	
	SP<ABaseChannel>   GetChannelByUID(IntU32 g_id);
	
	// *******************
	// * GetValidStream
	// * -----------------
	/*!
	Return a pointer on a stream that can be used by the system
	*/
	// *******************
	
	SP<ABaseStream>   GetValidStream();
	
	void	DoOnlyLocalUpdate(bool val) { mOnlyLocalSkeletonUpdate = val; }
	
	// ******************************
	// * GetChannelType
	// *-----------------------------
	/*!  return a classID for the default channel type
	*/
	// ******************************
	
	virtual kstl::string    GetChannelType() = 0;
	
	// ******************************
	// * Animate
	// *-----------------------------
	/*! Overloaded by derived systems
	*/
	// ******************************
	
	virtual void    Animate(ATimeValue t) = 0;
	
	
	// ******************************
	// * SetupDraw
	// *-----------------------------
	/*! Overloaded by derived systems
	*/
	// ******************************
	virtual void    SetupDraw() = 0;
	
	
	// ******************************
	// * UseOwnHierarchy
	// *-----------------------------
	/*! Overloaded by derived systems
	*/
	// ******************************
	
	virtual bool    UseOwnHierarchy() = 0;
	
	bool GetUseAnimationLocalToGlobal()
	{
		return mUseAnimationLocalToGlobal;
	}
	
	
	//protected:
	
	virtual ~ABaseSystem()
	{
	}
	// ******************************
	// * Protected members
	// *-----------------------------
	// * -
	// * -
	// ******************************
	
	virtual void		InitLocalToGlobalData() = 0;
	
	virtual void		ApplyLocalToGlobalData() = 0;
	// ----------------------------------------------------------------------------
	
	// *******************
	// * LinkTo
	// * -----------------
	/*!  use the given data as the local to global data
	*/
	// *******************
	
	virtual void    LinkTo(SP<ABaseChannel> data) = 0;
	
	// *******************
	// * UnLink
	// * -----------------
	/*!  stop using the link data
	*/
	// *******************
	
	virtual void    UnLink() = 0;
	
	// *******************
	// * GetSonGroupIDList
	// * -----------------
	/*!    construct a list of group id from the given group, with all the group sons,
	the list should be deleted after its use
	*/
	// *******************
	
	IntU32*          GetSonGroupIDList(SP<ABaseChannel> first, IntU32& count);
	
	// ******************************
	// * GetRootChannel
	// *-----------------------------
	/*!
	return the root channel
	*/
	// ******************************
	
	
	SP<ABaseChannel>               GetRootChannel()
	{
		return mRoot;
	};
	
	// ******************************
	// * GetLinksCount
	// *-----------------------------
	/*!
	return the ALinks count for this system
	*/
	// ******************************
	
	IntU32     GetLinksCount()
	{
		return mLinksCount;
	};
	
	// ******************************
	// * AddLinks
	// *-----------------------------
	/*!
	increment the ALinks count
	*/
	// ******************************
	
	void    AddLinks()
	{
		++mLinksCount;
	};
	
	// ******************************
	// * RemoveLinks
	// *-----------------------------
	/*!
	decrement the ALinks count
	*/
	// ******************************
	
	void    RemoveLinks()
	{
		--mLinksCount;
	};
	
	// ******************************
	// * SetAObject
	// *-----------------------------
	/*!
	Set the AObject using this ASystem
	*/
	// ******************************
	
	void    SetAObject(SP<AObject> object)
	{
		mAObject = object;
	};
	
	// ----------------------------------------------------------------------------
	
	// ******************************
	// * Local to Global management
	// *-----------------------------
	/*!  Tell the ASystem to use the root channel animation data
	*/
	// ******************************
	
	virtual void    UseAnimationLocalToGlobalData(bool b) = 0;
	
	// ******************************
	// * Local to Global management
	// *-----------------------------
	/*!  Tell the ASystem not to use the root channel animation(s) data
	*/
	// ******************************
	
	void    UseEngineLocalToGlobalData(bool b)
	{
		UseAnimationLocalToGlobalData(!b);
	};
	
	// ******************************
	// * Local to Global management
	// *-----------------------------
	/*!  Tell the ASystem to update the local to global data when the root channel
	animation(s) loop
	*/
	// ******************************
	
	void    UpdateLocalToGlobalWhenLoop(bool b)
	{
		mUpdateLocalToGlobalWhenLoop = b;
	};
	
	
	// ----------------------------------------------------------------------------
	
	IntU32                   mLinksCount;
	
	// ******************************
	// * DeleteChannelTree
	// *-----------------------------
	/*! delete all the channels
	*/
	// ******************************
	
	void DeleteChannelTree();
	
	// ******************************
	// * Hierarchy management
	// *-----------------------------
	/*! construct the channel tree with the given AObjectSkeletonResource , and call Init()
	*/
	// ******************************
	
	void    SetHierarchy(SP<AObjectSkeletonResource> hierarchy);
	
	// ******************************
	// * Hierarchy management
	// *-----------------------------
	/*! copy the channel tree from the given ASystem , and call Init()
	*/
	// ******************************
	
	void    SetHierarchyFromSystem(SP<ABaseSystem> sys);


	virtual void InitSystem() = 0;
	
	SP<AObject>    mAObject;
	
	// ******************************
	// * Channel array
	// *-----------------------------
	// * - to access channels quickly
	// * -
	// ******************************
	
	std::vector<SP<ABaseChannel>>   mChannelTab;
	IntU32				mChannelsCount;
	
	// ******************************
	// * SortChannels
	// *-----------------------------
	/*! sort channel by AGroupID in the AChannel array
	*/
	// ******************************
	
	void    SortChannels();
	
	
	bool                mRecurseAnimate;
	bool				mOnlyLocalSkeletonUpdate;
	
	SP<ABaseChannel>			mRoot;
	
	maInt						mPriority;
	
	bool						mUseAnimationLocalToGlobal;
	bool						mUpdateLocalToGlobalWhenLoop;
};



#endif //__ABASESYSTEM_H__