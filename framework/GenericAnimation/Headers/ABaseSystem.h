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
		return m_Priority;
	}
	
	// ******************************
	// * GetAObject
	// *-----------------------------
	/*! return a pointer on the AObject linked to this system
	*/
	// ******************************
	
	AObject*    GetAObject()
	{
		return m_pAObject;
	};
	
	
	// ******************************
	// * GetChannel
	// *-----------------------------
	/*! return the channel acting on the given group ID
	*/
	// ******************************
	
	ABaseChannel*   GetChannelByUID(IntU32 g_id);
	
	// *******************
	// * GetValidStream
	// * -----------------
	/*!
	Return a pointer on a stream that can be used by the system
	*/
	// *******************
	
	ABaseStream*   GetValidStream();
	
	void	DoOnlyLocalUpdate(bool val) { m_OnlyLocalSkeletonUpdate = val; }
	
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
		return m_UseAnimationLocalToGlobal;
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
	
	virtual void    LinkTo(ABaseChannel* data) = 0;
	
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
	
	IntU32*          GetSonGroupIDList(ABaseChannel* first, IntU32& count);
	
	// ******************************
	// * GetRootChannel
	// *-----------------------------
	/*!
	return the root channel
	*/
	// ******************************
	
	
	ABaseChannel*               GetRootChannel()
	{
		return (ABaseChannel*)mp_Root.get();
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
		return m_LinksCount;
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
		++m_LinksCount;
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
		--m_LinksCount;
	};
	
	// ******************************
	// * SetAObject
	// *-----------------------------
	/*!
	Set the AObject using this ASystem
	*/
	// ******************************
	
	void    SetAObject(AObject* object)
	{
		m_pAObject = object;
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
		m_UpdateLocalToGlobalWhenLoop = b;
	};
	
	
	// ----------------------------------------------------------------------------
	
	IntU32                   m_LinksCount;
	
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
	
	void    SetHierarchy(AObjectSkeletonResource* hierarchy);
	
	// ******************************
	// * Hierarchy management
	// *-----------------------------
	/*! copy the channel tree from the given ASystem , and call Init()
	*/
	// ******************************
	
	void    SetHierarchyFromSystem(ABaseSystem* sys);


	virtual void InitSystem() = 0;
	
	AObject*    m_pAObject;
	
	// ******************************
	// * Channel array
	// *-----------------------------
	// * - to access channels quickly
	// * -
	// ******************************
	
	SP<ABaseChannel>*   m_pChannelTab;
	IntU32				m_ChannelsCount;
	
	// ******************************
	// * SortChannels
	// *-----------------------------
	/*! sort channel by AGroupID in the AChannel array
	*/
	// ******************************
	
	void    SortChannels();
	
	
	bool                m_RecurseAnimate;
	bool				m_OnlyLocalSkeletonUpdate;
	
	SP<ABaseChannel>			mp_Root;
	
	maInt						m_Priority;
	
	bool						m_UseAnimationLocalToGlobal;
	bool						m_UpdateLocalToGlobalWhenLoop;
};



#endif //__ABASESYSTEM_H__