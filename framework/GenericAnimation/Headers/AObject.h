// ****************************************************************************
// * NAME: AObject.h
// * GROUP: Animation module
// * --------------------------------------------------------------------------
// * PURPOSE:   Access to the object to animate, here you can play, stop and modifie 
// *            animations on your object. 
// * 
// * COMMENT: 
// * --------------------------------------------------------------------------
// * RELEASE: 
// ****************************************************************************


#ifndef __AOBJECT_H__
#define __AOBJECT_H__

#include "Drawable.h"
#include "AMDefines.h"
#include "ALinks.h"
// +---------
// | Declare some usefull classes
// +---------

// Required from WRAP_METHOD
#include "AnimationResourceInfo.h"

class   AEntitySet;    
class   ABaseSystem;
class   AEngineObject;
class   ALinearInterp;
class	AObjectResource;
class	ABaseStream;

#include "AttributePacking.h"


struct sortSystems {
	bool operator() (const ABaseSystem* lhs, const ABaseSystem* rhs) const;
};

// +---------
// | AObject Declaration
/*!  Class used to call all the animation methods on an object
     \ingroup Animation
*/
// +---------

class   AObject : public Drawable
{
	DECLARE_CLASS_INFO(AObject, Drawable, Animation);
	DECLARE_CONSTRUCTOR(AObject);
	WRAP_METHODS(	AddAnimation,
					RemoveAnimation,
					StartAnimation,
					SetAnimationPos,
					StartRepeatAnimation,
					StopAnimation,
					StopAllAnimations,
					ResumeAnimation,
					SetAnimationWeight,
					SetAnimationSpeed,
					MulAnimationWeight,
					MulAnimationSpeed,
					SetLoop,
					FadeAnimationTo,
					SynchroniseAnimations,
					HasAnimationLoop,
					AnimationIsSet,
					HasAnimationReachEnd,
					SetLocalToGlobalMode,
					LinkTo,
					UnLink,
					GetAnimationByIndex);
public:
	
	void Update(const Timer& timer, void* addParam) override;
	
	// catch resource manager add & remove
	bool	addItem(CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool	removeItem(CMSP& item DECLARE_DEFAULT_LINK_NAME) override;
	
	//! overloaded to be notified when added to or removed from an animated object
	void		addUser(CoreModifiable* user) override;
	void		removeUser(CoreModifiable* user) override;
	
	
	void	DoPreDraw(TravState*) override;
	bool	
		BBoxUpdate(kdouble /* time */) override
	{
		return false;
	}
	
	
	unsigned int	GetSelfDrawingNeeds() override
	{
		return ((unsigned int)Need_Predraw);
	}
    // ******************************
    // * GetObject
    // *-----------------------------
    /*! return the engine object linked to this AObject (should be the skinning object)
    */ 
    // ******************************
	
    CoreModifiable*    GetObject() const
    {
        return m_pObject;
    };
	
	
    // ******************************
    // * Animate
    // *-----------------------------
    /*! Setup Animation for time t for all the systems used by this AObject
    */ 
    // ******************************
	
    void            Animate(ATimeValue t);
	
	
    // ******************************
    // * AddAnimation
    // *-----------------------------
    /*! Add an animation to this object, but don't play it, wait for the start animation
    */ 
    // ******************************/
	
    void    AddAnimation(AnimationResourceInfo* info);
	
	
    // ******************************
    // * RemoveAnimation
    // *-----------------------------
    /*! delete the animation 
    */ 
    // ******************************
	
    void    RemoveAnimation(AnimationResourceInfo* info);
	
	
    // ******************************
    // * StartAnimation
    // *-----------------------------
    /*! start playing animation
    */ 
    // ******************************
	
    void    StartAnimation(AnimationResourceInfo* info,ATimeValue  t,IntU32* g_id=NULL,IntU32 g_count=0);
	
	
    // ******************************
    // * SetAnimationPos
    // *-----------------------------
    /*! set the animation time for the given % of animation length
    */ 
    // ******************************
	
    void    SetAnimationPos(AnimationResourceInfo* info,Float  percent,IntU32* g_id=NULL,IntU32 g_count=0);
	
	
    // ******************************
    // * StartRepeatAnimation
    // *-----------------------------
    /*! start playing animation n times
    */ 
    // ******************************
	
    void    StartRepeatAnimation(AnimationResourceInfo* info,ATimeValue  t,IntU32 n,IntU32* g_id=NULL,IntU32 g_count=0);
	
	
    // ******************************
    // * StopAnimation
    // *-----------------------------
    /*! stop playing animation
    */ 
    // ******************************
	
    void    StopAnimation(AnimationResourceInfo* info,IntU32* g_id=NULL,IntU32 g_count=0);
	
	
	// ******************************
    // * StopAllAnimations
    // *-----------------------------
    /*! stop playing all animations
    */ 
    // ******************************
	
    void    StopAllAnimations(IntU32* g_id=NULL,IntU32 g_count=0);
	
	
	
	
    // ******************************
    // * ResumeAnimation
    // *-----------------------------
    /*! restart animation where it was stopped
    */ 
    // ******************************
	
    void    ResumeAnimation(AnimationResourceInfo* info,ATimeValue  t,IntU32* g_id=NULL,IntU32 g_count=0);
	
	
    // ******************************
    // * SetAnimationWeight
    // *-----------------------------
    /*! change the weight of an animation
    */ 
    // ******************************
	
    void    SetAnimationWeight(AnimationResourceInfo* info,Float weight,IntU32* g_id=NULL,IntU32 g_count=0);
	
	
    // ******************************
    // * SetAnimationSpeed
    // *-----------------------------
    /*! change the speed of an animation
    */ 
    // ******************************
	
    void    SetAnimationSpeed(AnimationResourceInfo* info,Float speed,IntU32* g_id=NULL,IntU32 g_count=0);
	
	
    // ******************************
    // * MulAnimationWeight
    // *-----------------------------
    /*! multiply the weight of an animation by the given factor
    */ 
    // ******************************
	
    void    MulAnimationWeight(AnimationResourceInfo* info,Float weight,IntU32* g_id=NULL,IntU32 g_count=0);
	
	
    // ******************************
    // * MulAnimationSpeed
    // *-----------------------------
    /*!  multiply the speed of an animation by the given factor
    */ 
    // ******************************
	
    void    MulAnimationSpeed(AnimationResourceInfo* info,Float speed,IntU32* g_id=NULL,IntU32 g_count=0);
	
    // ******************************
	
    // * SetLoop
    // *-----------------------------
    /*! Set loop mode 
    */
    // ******************************
	
    void    SetLoop(AnimationResourceInfo* info,bool loop,IntU32* g_id=NULL,IntU32 g_count=0);
	
	
    // ******************************
    // * FadeAnimationTo
    // *-----------------------------
    /*! Fade the first animation to the second one
    */ 
    // ******************************
	
    void    FadeAnimationTo(AnimationResourceInfo* info1,AnimationResourceInfo* info2,ATimeValue  fade_length,ATimeValue  t);
	
	
    // ******************************
    // * SynchroniseAnimations
    // *-----------------------------
    /*!
        Change the speed of animation 2 so that animation 1 reach is local time synchro1 when
        animation 2 reach is local time synchro2
    */  
    // ******************************
	
    void    SynchroniseAnimations(AnimationResourceInfo* info1,AnimationResourceInfo* info2,ATimeValue  synchro1,ATimeValue  synchro2);
	
	
	
    // ******************************
    // * HasAnimationLoop
    // *-----------------------------
    /*! return true the animation has loop during the last Animate call
    */ 
    // ******************************
	
    bool    HasAnimationLoop(AnimationResourceInfo* info1);
	
	
    // ******************************
    // * AnimationIsSet
    // *-----------------------------
    /*! return true if the animation is already added to this object
    */ 
    // ******************************
	
    bool    AnimationIsSet(AnimationResourceInfo* info1);
	
	
    // ******************************
    // * HasAnimationReachEnd
    // *-----------------------------
    /*!
        return true if animation has reached its end during the last Animate call
    */ 
    // ******************************/
	
    bool    HasAnimationReachEnd(AnimationResourceInfo* info1);
	
	
    // ******************************
    // * Local To Global Data management
    // *-----------------------------
    // * - Three differents modes :
    // *    
    // *    - FromAnimation : the local to global data is only given by the animation itself
    // *      ( from the root channel ), the module ask for a starting local to global data
    // * 
    // *    - FromAnimationWithLoop : the local to global data is given by the animation, but
    // *       the starting local to global data is updated when animation loop
    // * 
    // *    - FromEngine : the animation local to global data is not used, the data is asked to
    // *        the engine.
    // * 
    // ******************************
	
    enum LocalToGlobalMode
    {
        FromAnimation           = 1,
        FromAnimationWithLoop   = 2,
        FromEngine              = 3
    };
	
    // ******************************
    // * SetLocalToGlobalMode
    // *-----------------------------
    /*!
        set one of this mode :
        - FromAnimation : the local to global data is only given by the animation itself
            ( from the root channel ), the module ask for a starting local to global data
			
        - FromAnimationWithLoop : the local to global data is given by the animation, but
            the starting local to global data is updated when animation loop
			
        - FromEngine : the animation local to global data is not used, the data is asked to
            the engine.
    */ 
    // ******************************/
	
    void    SetLocalToGlobalMode(unsigned int system_type, int mode);
	
	
    // ----------------------------------------------------------------------------
	
    // *******************
    // * LinkTo
    // * -----------------
    /*! 
        use the local to global data found in the AObject AChannel with the given GroupID  
    */ 
    // *******************
	
    void    LinkTo(unsigned int system_type,AObject* other_object,IntU32 g_id);
	
	
    // *******************
    // * UnLink
    // * -----------------
    /*! Stop the link
    */ 
    // *******************
	
    void    UnLink(unsigned int system_type);
	
	
    // ******************************
    // * GetSystemByType
    // *-----------------------------
    /*! return a ASystem used on this object with the given super class id ( or NULL if no found )
    */
    // ******************************/
	
	ABaseSystem*     GetSystemByType(unsigned int system_type);
	
	
    // ******************************
    // * GetSystembyIndex
    // *-----------------------------
    /*! return a ASystem used on this object with the given index
    */
    // ******************************/
	
	ABaseSystem*     GetSystembyIndex(IntU32 index);
	
    // ******************************
    // * GetAnimationByIndex
    // *-----------------------------
    /*! return a pointer on a AnimationResourceInfo used on the object
    */ 
    // ******************************/
	
    AnimationResourceInfo*     GetAnimationByIndex(IntU32 index);
	
	
	//protected:
    // ******************************
    // * Structors
    // *-----------------------------
    /*! Destructor
    */
    // ******************************
	
    virtual ~AObject()
    {
		
        // +---------
        // | delete all the animations for this object
        // +---------
		while(m_ALinksTable.size())
		{
			auto it1 = m_ALinksTable.begin();
			RemoveAnimation(it1->second->GetAnimResourceInfo());
        }
    };
	
	void			InitModifiable() override;
    // ******************************
    // * AttachSystem
    // *-----------------------------
    /*! search the corresponding LtoG system and create hierarchy from it
    */ 
    // ******************************
	
    void AttachSystem(ABaseSystem* system);
	
    // +---------
    // | protected members
    // +---------
	
	
	CoreModifiable*										m_pObject;
	kstl::unordered_map<void*,ALinks*>							m_ALinksTable;
	kstl::vector<ALinearInterp*>						m_FadeList;
    kstl::set<ABaseSystem*, sortSystems>				m_pSystemSet;
	kstl::unordered_map<KigsID, AObjectResource*>		m_ObjectResourceMap;
	//int													m_animCount;
	
    // ******************************
    // * System management
    // *-----------------------------
    /*! Add a ASystem in the list of ASystem used by this AObject
    */
    // ******************************
	
    void            AddSystem(ABaseSystem* system);
	
    // ******************************
    // * System management
    // *-----------------------------
    /*! Remove a ASystem from the list of ASystem used by this AObject
    */
    // ******************************
    void            RemoveSystem(ABaseSystem* system);
	
    // ----------------------------------------------------------------------------
	
    // *******************
    // * IsInGroupIDList
    // * -----------------
    /*! search in groupid array for the given groupid
    */ 
    // *******************
	
    static bool    IsInGroupIDList(IntU32 id,IntU32* list,IntU32 count)
    {
        IntU32   i;
        for(i=0;i<count;++i)
        {
            if(list[i] == id)
            {
                return true;
            }
        }
        return false;
    };
	
    // ******************************
    // * DoForEachStream
    // *-----------------------------
    /*! call the given methods for all streams given in the g_id list
    */ 
    // ******************************
    
    void    DoForEachStream(AnimationResourceInfo* info,IntU32* g_id,IntU32 g_count,void* params,void (AObject::*func)(ABaseStream* stream,void* param));
	
    void    StartAnimationFor(ABaseStream* stream,void* param);
	
	void	SetAnimationPosFor(ABaseStream* stream,void* param);
	
    void    StartRepeatAnimationFor(ABaseStream* stream,void* param);
	
    void    StopAnimationFor(ABaseStream* stream,void* param);
	
    void    ResumeAnimationFor(ABaseStream* stream,void* param);
	
    void    SetAnimationWeightFor(ABaseStream* stream,void* param);
	
    void    SetAnimationSpeedFor(ABaseStream* stream,void* param);
	
    void    MulAnimationWeightFor(ABaseStream* stream,void* param);
	
    void    MulAnimationSpeedFor(ABaseStream* stream,void* param);
	
    void    SetLoopFor(ABaseStream* stream,void* param);
	
	
	/*void	SetAnimCount(int count)
	{
	 m_animCount = count;
	}
	
	int GetAnimCount()
	{
	 return m_animCount;
	}
	*/
};

#endif //__AOBJECT_H__


