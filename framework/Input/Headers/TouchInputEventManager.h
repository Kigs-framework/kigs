#pragma once
#include "Core.h"
#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"

#include "maReference.h"

#include "TecLibs/Math/IntersectionAlgorithms.h"

#include <map>

class ModuleInput;
struct Interaction;
class Camera;

enum class TouchSourceID : u32
{
	Mouse,
	Gaze,
	SpatialInteractionLeft,
	SpatialInteractionRight,
	SpatialInteractionRayLeft,
	SpatialInteractionRayRight,
	MultiTouch_0,
	Invalid = UINT32_MAX
};

inline bool IsNearInteraction(TouchSourceID id)
{
	return id == TouchSourceID::SpatialInteractionLeft || id == TouchSourceID::SpatialInteractionRight;
}

namespace std
{
	template<>
	struct hash< TouchSourceID >
	{
		typedef TouchSourceID argument_type;
		typedef std::underlying_type< argument_type >::type underlying_type;
		//typedef std::hash< underlying_type >::result_type result_type;
		auto operator()(const argument_type& arg) const
		{
			std::hash< underlying_type > hasher;
			return hasher(static_cast< underlying_type >(arg));
		}
	};
}

struct SortItemsFrontToBackParam
{
	CoreModifiable* camera;
	std::vector<CoreModifiable*> toSort;
	std::vector<std::tuple<CoreModifiable*, Hit>> sorted;

	v3f position;
	v3f origin;
	v3f direction;
	double min_distance = -DBL_MAX;
	double max_distance = DBL_MAX;
	TouchSourceID touchID;
};

struct GetDistanceForInputSortParam
{
	CoreModifiable* camera;
	v3f origin;
	v3f direction;
	v3f position;
	std::vector<Hit>* hits = nullptr;
	double min_distance;
	double max_distance;

	double inout_distance;
	Hit* inout_hit = nullptr;
	// Each element is sorted within its layer by inout_distance. Layers go from front (-INT_MAX) to back (INT_MAX)
	int inout_sorting_layer = INT_MAX;
};


enum GestureRecognizerState
{
	StatePossible,
	StateBegan,
	StateChanged,
	StateEnded,
	StateCancelled,

	StateFailed,

	StateRecognized = StateEnded
};



enum InputEventManagementFlag
{
	EmptyFlag = 0,

	IgnoreSwallow = 1,
	Focusable = 2,
};

enum InputEventType
{
	// catch click / double click ...
	Click = 0,
	// manage swipe (fast touch, directionnal move, up)
	Swipe = 1,
	// manage pinch (two touch distance / orientation)
	Pinch = 2,
	// manage scroll (touch, directionnal move, up)
	Scroll = 3,
	// manage drag & drop
	DragAndDrop = 4,
	// manage direct touch down / up / hover event
	DirectTouch = 5,
	// reserved for input swallowing by UIItems
	InputSwallow = 6,
	// Direct Access
	DirectAccess = 7,

	// UserDefinedStart to 20 can be user defined
	UserDefinedStart,
	MaxType = 20
};

struct touchPosInfos
{
	Vector3D		dir;							// 3D direction if available
	v3f				origin;							// 3D origin if available
	v3f				pos;							// 3D position (z is 0 for 2d mouse or touch)
	unsigned int	flag;
	Hit				hit = {};
	double			min_distance = -DBL_MAX;
	double			max_distance = DBL_MAX;

	inline bool	has3DInfos()
	{
		return flag & 1;
	}

	inline void	setHas3DInfos(bool has3dinfos)
	{
		if (has3dinfos)
		{
			flag |= 1;
		}
		else
		{
			flag &= ~1;
		}
	}
};

class TouchInputEventManager;

// base class for gesture recognizer
class TouchEventState
{
public :

	GestureRecognizerState getCurrentState()
	{
		return mCurrentState;
	}

	InputEventType getCurrentType()
	{
		return mType;
	}

	u32& flags() { return mFlag; }

protected:

	virtual ~TouchEventState() = default;

	class TouchInfos
	{
	public:
		
		// transformed pos in TouchSupport object
		touchPosInfos	posInfos;

		TouchSourceID	ID;
		unsigned short	touch_state;
		unsigned short	in_touch_support;
		bool touch_ended = false;
		bool has_position;

		Hit* object_hit = nullptr;
		Interaction* interaction = nullptr;
		CoreModifiable* starting_touch_support = nullptr;
		bool need_starting_touch_support_transform = true;
	};
	friend bool	operator<(const TouchInfos& first, const TouchInfos& other);
	friend bool	operator<(const TouchInfos& first, unsigned int other);

	// return value is swallow mask, bit n° eventType is set to 1 when swallowed
	virtual void Update(TouchInputEventManager* manager,const Timer& timer,CoreModifiable* target, const TouchInfos& touch, u32& swallow_mask)=0;

	virtual void Reset() = 0;

	friend class TouchInputEventManager;
	TouchEventState(KigsID methodnameID, u32 flag, InputEventType type) : mCurrentState(StatePossible)
		, mMethodNameID(methodnameID)
		, mFlag(flag)
		, mType(type)
	{};
	GestureRecognizerState		mCurrentState;
	KigsID						mMethodNameID;
	u32	mFlag;
	InputEventType				mType;
};



class TouchEventStateInputSwallow : public TouchEventState
{
public:
	TouchEventStateInputSwallow(KigsID methodnameID, InputEventManagementFlag flag/*, CoreModifiable* touchsupport*/, InputEventType type) : TouchEventState(methodnameID, flag/*, touchsupport*/, type)
	{
	};
	void Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallow_mask) override;

protected:
	virtual void Reset() override
	{

	}
};


struct InputEvent
{
	InputEventType type;
	v3f position;
	GestureRecognizerState state;
	
	// Ony available in DirectTouch when the touch support is a camera
	v3f origin;
	v3f direction;

	Hit hit;
	const Interaction* interaction = nullptr;

	// Bitfield of InputEventType
	u32* swallow_mask;
	bool capture_inputs = false;
	TouchSourceID touch_id;
	bool has_position = false;
	CoreModifiable* item = nullptr;
};

struct ClickEvent : InputEvent
{
	int click_count;
	enum Button
	{
		LeftButton = 1,
		RightButton = 2,
	};
	int button_state_mask;
};


/*
	ClickEvent handler:

	StatePossible => called at the beginning. Return true if the click is valid (i.e inside the bounding box, not hidden, etc...)
	StateChanged => called every frame between min and max time. Same return value as StatePossible
	StateRecognized => called at the end when the click is accepted. 
*/


class TouchEventStateClick : public TouchEventState
{
public:
	TouchEventStateClick(KigsID methodnameID, InputEventManagementFlag flag/*, CoreModifiable* touchsupport*/, InputEventType type) : TouchEventState(methodnameID, flag/*, touchsupport*/, type)
		, mMinClickCount(1)
		, mMaxClickCount(1)
		, mClickMinDuration(0.0f)
		, mClickMaxDuration(0.6f)
	{};
	void Update(TouchInputEventManager* manager, const Timer& timer,CoreModifiable* target, const TouchInfos& touch, u32& swallow_mask) override;

	void	setMinNeededClick(int c)
	{
		mMinClickCount = c;
	}
	void	setMaxNeededClick(int c)
	{
		mMaxClickCount = c;
	}

	void setAutoClickDistance(float distance)
	{
		mSpatialInteractionAutoClickDistance = distance;
	}

protected:

	virtual void Reset() override
	{
		mCurrentClickStart.clear();
		mCurrentClickEnd.clear();
	}

	struct PotentialClick
	{
		double		 startTime;
		Vector3D	 startPos;
		Vector3D	 currentPos;
		int			 clickCount;
		int			 buttonState;
		bool		 isValid;
		TouchSourceID ID;
		v3f          origin;
		v3f          direction;
		float		 start_dist;
		float		 min_dist;
	};

	kigs::unordered_map<TouchSourceID, PotentialClick> mCurrentClickStart;
	kigs::unordered_map<TouchSourceID, PotentialClick> mCurrentClickEnd;

	int											mMinClickCount = 1;
	int											mMaxClickCount = 1;
	kfloat										mClickMinDuration = 0.0f;
	kfloat										mClickMaxDuration = 0.6f;
	float										mSpatialInteractionAutoClickDistance = 0.00f;
};

struct DirectTouchEvent : InputEvent
{
	enum TouchState
	{
		TouchHover = 0,
		TouchDown = 1,
		TouchUp = 2,
	} touch_state;
	u32 button_state;
};

/*

DirectTouchEvent handler:

StatePossible => called every frame, return true if the hover is valid (i.e inside the bounding box, not hidden, etc...)
StateChanged => called every frame when the hover is valid
StateBegan => called once when touch start and once when it ends (check touch_state)
StateEnded => called once when the hover ends

*/


class TouchEventStateDirectTouch : public TouchEventState
{
public:
	TouchEventStateDirectTouch(KigsID methodnameID, InputEventManagementFlag flag, /*CoreModifiable* touchsupport,*/ InputEventType type) : TouchEventState(methodnameID, flag, /*touchsupport, */type)
	{};
	void Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallow_mask) override;
	void setAutoTouchDownDistance(float distance)
	{
		mSpatialInteractionAutoTouchDownDistance = distance;
	}
protected:

	virtual void Reset() override
	{
		mCurrentInfosMap.clear();
	}

	struct CurrentInfos
	{
		Vector3D	currentPos;
		// 1 => hover
		// 2 => activation down
		// 4 => not hover down
		int			state;
		float start_dist;
		float min_dist;
	};

	std::map<TouchSourceID, CurrentInfos> mCurrentInfosMap;
	float								 mSpatialInteractionAutoTouchDownDistance = 0.00f;
};





struct DirectAccessEvent : InputEvent
{
	bool is_swallowed;
	u16	touch_state;
	u16 in_touch_support;
};

/*
DirectAccessEvent handler:
StateChanged => called every frame not matter what, user must manage state
*/


class TouchEventStateDirectAccess : public TouchEventState
{
public:
	TouchEventStateDirectAccess(KigsID methodnameID, InputEventManagementFlag flag, /*CoreModifiable* touchsupport,*/ InputEventType type) : TouchEventState(methodnameID, flag, /*touchsupport, */type)
	{
	};
	void Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallow_mask) override;

protected:

	virtual void Reset() override
	{
		mCurrentTouches.clear();
	}

	std::set<TouchSourceID> mCurrentTouches;
};




struct SwipeEvent : InputEvent
{
	v3f direction;
};

/*

SwipeEvent handler:

StatePossible => called once. return true if the swipe can start here (i.e inside the bounding box, not hidden, etc...)
StateBegan => called once when the swipe starts. return true if  the swipe if still valid
StateChanged => called every frame during the swipe. return true if  the swipe if still valid
StateRecognized => called once when the swipe is accepted

*/


class TouchEventStateSwipe : public TouchEventState
{
public:

	TouchEventStateSwipe(KigsID methodnameID, InputEventManagementFlag flag, /*CoreModifiable* touchsupport,*/ InputEventType type) : TouchEventState(methodnameID, flag, /*touchsupport, */type)
		, mSwipeMinDuration(0.01)
		, mSwipeMaxDuration(0.6)
	{};
	void Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallow_mask) override;
protected:

	struct TimedTouch
	{
		Vector3D	pos;
		double		time;
	};
	struct CurrentInfos
	{
		std::vector<TimedTouch>	touchList;
		bool	isValid;
	};

	virtual void Reset() override
	{
		mCurrentInfosMap.clear();
	}

	std::map<TouchSourceID, CurrentInfos>		mCurrentInfosMap;

	double										mSwipeMinDuration;
	double										mSwipeMaxDuration;
};

struct ScrollEvent : InputEvent
{
	v3f main_direction;
	v3f start_position;
	v3f speed;
	v3f delta;
	float offset;
};

/*

ScrollEvent handler

StatePossible: called once, return true if the scroll can start here
StateBegan: called once when the scroll starts, return true if the scroll is still valid
StateChanged: called every frame during the scroll
StateEnded: called once when the scroll end

*/
class TouchEventStateScroll : public TouchEventState
{
public:

	TouchEventStateScroll(KigsID methodnameID, InputEventManagementFlag flag, /*CoreModifiable* touchsupport, */InputEventType type) : TouchEventState(methodnameID, flag,/* touchsupport,*/ type)
		, mScrollForceMainDir(0,0,0)
	{};
	void Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallow_mask) override;

	void	forceMainDir(Vector3D maindir)
	{
		mScrollForceMainDir = maindir;
		mScrollForceMainDir.Normalize();
	}

protected:

	virtual void Reset() override
	{
		mCurrentInfosMap.clear();
	}

	struct CurrentInfos
	{
		Point3D	startpos;
		Point3D	currentpos;
		double		starttime;
		double		currenttime;
		bool		isValid;
		Vector3D	maindir;
		Vector3D	currentSpeed;
	};
	std::map<TouchSourceID, CurrentInfos>		mCurrentInfosMap;
	Vector3D									mScrollForceMainDir;
};



class TouchEventStatePinch : public TouchEventState
{
public:
	TouchEventStatePinch(KigsID methodnameID, InputEventManagementFlag flag, InputEventType type) : TouchEventState(methodnameID, flag, type) {}
	void Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallowMask) override;

protected:

	struct CurrentTouch
	{
		v3f position;
		bool in_use_by_pinch;
	};

	struct CurrentPinch
	{
		TouchSourceID p1_ID;
		TouchSourceID p2_ID;
		v3f p1_start_pos;
		v3f p2_start_pos;
	};

	virtual void Reset() override
	{
		mCurrentTouches.clear();
		mCurrentPinches.clear();
	}

	kigs::unordered_map<TouchSourceID, CurrentTouch> mCurrentTouches;
	std::vector<CurrentPinch> mCurrentPinches;
	
	float  mPinchMaxStartDistSquared = 9999999;
};

struct PinchEvent : InputEvent
{
	v3f p1, p2;
	v3f p1_start, p2_start;
	TouchSourceID touch_id_2;
};


/*
class TouchEventStateDragAndDrop : public TouchEventState
{
};
*/
// ****************************************
// * TouchInputEventManager class
// * --------------------------------------
/**
 * \class	TouchInputEventManager
 * \file	TouchInputEventManager.h
 * \ingroup Input
 * \brief	Manager for all input events : click, swipe, scroll...
 */
 // ****************************************

class TouchInputEventManager : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(TouchInputEventManager, CoreModifiable, Input)
	DECLARE_CONSTRUCTOR(TouchInputEventManager);



	// registered object wants to be called when event "type" occurs (start, end, update...)  
	TouchEventState*	registerEvent(CoreModifiable* registeredObject,KigsID methodNameID,InputEventType type, InputEventManagementFlag flag, CoreModifiable* root_scene=nullptr);
	void	unregisterEvent(CoreModifiable* registeredObject, InputEventType type);
	void	unregisterObject(CoreModifiable* registeredObject);

	bool isRegisteredOnCurrentState(CoreModifiable* obj);

	void Update(const Timer& timer, void* addParam) override;

	void ForceClick() { mForceClick = true; }

	// deactivate current events and create a new management state (ie for popup)
	void	pushNewState();

	// go back to previous activated state
	void	popState();

	void	addTouchSupport(CoreModifiable* ts, CoreModifiable* parent);

	void	removeTouchSupport(CoreModifiable* ts);

	inline int getTriggerSquaredDist()
	{
		return mTriggerSquaredDist;
	}

	void	setTriggerDist(int dist)
	{
		mTriggerSquaredDist = dist*dist;
	}

	inline float getSpatialInteractionOffset()
	{
		return mSpatialInteractionOffset;
	}

	void setSpatialInteractionOffset(float offset)
	{
		mSpatialInteractionOffset = offset;
	}

	void ManageCaptureObject(InputEvent& ev, CoreModifiable* target);
	bool AllowEventOn(CoreModifiable* target) { return !mEventCaptureObject || target == mEventCaptureObject; }
	
	void	activate(bool activate)
	{
		if ((mIsActive == true) && (activate == false))
		{
			ResetCurrentStates();
		}
		mIsActive = activate;
	}

	bool	isActive()
	{
		return mIsActive;
	}

	
protected:

	void	ResetCurrentStates();

	void dumpTouchSupportTrees();
	DECLARE_METHOD(OnDestroyCallback);
	DECLARE_METHOD(OnDestroyTouchSupportCallback);

	COREMODIFIABLE_METHODS(OnDestroyCallback, OnDestroyTouchSupportCallback);
	
	virtual ~TouchInputEventManager();

	maBool mUseGazeAsTouchDevice = BASE_ATTRIBUTE(UseGazeAsTouchDevice, false);

	kigs::unordered_map<TouchSourceID, TouchEventState::TouchInfos> mLastFrameTouches;

	std::recursive_mutex mMutex;
	std::unique_lock<std::recursive_mutex> mLock;

	// states can be pushed on a stack, so that when creating a pop up or a fullscreen IHM in front of another, we can temporary mask previous registered events
	// and go back to them after that
	class StackedEventStateStruct
	{
	public:
		StackedEventStateStruct()
		{
			int i;
			for (i = 0; i < MaxType; i++)
			{
				mRegisteredCount[i] = 0;
			}
		}

		~StackedEventStateStruct()
		{

		}

		struct EventStateSorter
		{
			bool operator()(CoreModifiable* a, CoreModifiable* b)
			{
				return a < b;
			}
		};

		unsigned short		mRegisteredCount[MaxType];

		struct EventMapEntry
		{
			std::vector<TouchEventState*>	mTouchEventStateList;
			CoreModifiable*					mRootScene3D;
		};

		kigs::unordered_map<CoreModifiable*, EventMapEntry>	mEventMap;
	};

	bool	unregisterEventOnCurrentState(StackedEventStateStruct& state,CoreModifiable* registeredObject, InputEventType type);
	bool	unregisterObjectOnCurrentState(StackedEventStateStruct& state, CoreModifiable* registeredObject);


	std::vector<StackedEventStateStruct>	mStackedEventState;
	CoreModifiable*										mEventCaptureObject = nullptr;
	TouchSourceID										mEventCapturedEventID;

	ModuleInput*	mTheInputModule;

	class touchSupportTreeNode
	{
	public:
		touchSupportTreeNode()
		{
			mCurrentNode = 0;
			mParentNode = 0;
		}

		touchSupportTreeNode*	searchNode(CoreModifiable* toSearch, CoreModifiable* toSearchParentOptional = nullptr)
		{
			if (mCurrentNode == toSearch && (!toSearchParentOptional || mParentNode == toSearchParentOptional))
			{
				return this;
			}
			for (u32 i = 0; i < mSons.size(); i++)
			{
				touchSupportTreeNode* found = mSons[i].searchNode(toSearch, toSearchParentOptional);
				if (found)
					return found;
			}
			return 0;
		}

		CoreModifiable*	mCurrentNode;
		CoreModifiable*	mParentNode;
		std::vector<touchSupportTreeNode>	mSons;
	};


	struct SortedElementNode
	{
		CoreModifiable* element;
		CoreModifiable* touchSupport;
		Hit hit;
	};

	class SortedElementTreeNode
	{
	public:
		CoreModifiable*	element;
		CoreModifiable* touchSupport;
		std::vector<SortedElementTreeNode> sons;
	};

	struct Scene3DAndCamera
	{
		CoreModifiable*			scene3D;
		touchSupportTreeNode*	camera;

		struct PriorityCompare
		{
			//! overload operator () for comparison
			bool operator()(const Scene3DAndCamera& a1, const Scene3DAndCamera& a2) const
			{
				s64 p1=0, p2=0;
				a1.scene3D->getValue("Priority", p1);
				a2.scene3D->getValue("Priority", p2);

				if (p1 == p2)
				{
					if (a1.camera && a2.camera)
					{
						a1.camera->mCurrentNode->getValue("Priority", p1);
						a2.camera->mCurrentNode->getValue("Priority", p2);

						if (p1 == p2)
						{
							p1 = (s64)a1.camera;
							p2 = (s64)a2.camera;
						}
					}
					else
					{
						p1 = (s64)a1.scene3D;
						p2 = (s64)a2.scene3D;
					}
					
				}
					
				return (p1)<(p2);
			}
		};
	};


	void RecursiveFlattenTreeForTouchID(std::vector<SortedElementNode>& flat_tree, touchSupportTreeNode* CurrentTouchSupport, 
		kigs::unordered_map<CoreModifiable*, std::set< Scene3DAndCamera, Scene3DAndCamera::PriorityCompare > >& perRenderingScreenSortedMap,
		kigs::unordered_map<CoreModifiable*, std::vector<CoreModifiable*> >& perScene3DMap,
		kigs::unordered_map<CoreModifiable*, kigs::unordered_map<TouchSourceID, TouchEventState::TouchInfos>>& transformedInfosMap, TouchSourceID touch_id);

	void	LinearCallEventUpdate(std::vector<SortedElementNode>& flat_tree, const Timer& timer, kigs::unordered_map<CoreModifiable*, kigs::unordered_map<TouchSourceID, TouchEventState::TouchInfos> >& transformedInfosMap, TouchSourceID touch_id);

	// touch support tree list (more than one root for multiple windows)
	std::vector<touchSupportTreeNode>					mTouchSupportTreeRootList;
	touchSupportTreeNode*								mCurrentTouchSupportRoot;
	std::map<CoreModifiable*, touchSupportTreeNode*>	mTouchSupportMap;
	bool												mInUpdate = false;
	std::set<CoreModifiable*>							mDestroyedThisFrame;

	

	void	manageTemporaryUnmappedTouchSupport(CoreModifiable* ts, CoreModifiable* parent);

	bool	removeTemporaryUnmappedTouchSupport(CoreModifiable* ts);

	std::vector<touchSupportTreeNode>					mTemporaryUnmappedTouchSupport;

	// common squared dist to trigger scroll / click...
	int													mTriggerSquaredDist;
	float												mSpatialInteractionOffset = 0.1f;

	// manage touch active / not active
	bool	mIsActive;

	bool mForceClick = false;

	void	transformTouchesInTouchSupportHierarchy(touchSupportTreeNode* current, kigs::unordered_map<CoreModifiable*, kigs::unordered_map<TouchSourceID, TouchEventState::TouchInfos> >& resultmap, kigs::unordered_map<TouchSourceID, TouchEventState::TouchInfos>& Touches);
};

// Helper struct to track a state across multiple touch sources. 
// For example if multiple sources can hover a button at the same time :
// Do Push/Pop when receiving TouchState::TouchHover StateBegan/StateEnded events, and use IsActive() to check if the button need to be drawn as hovered or not
struct TouchEventTracker 
{
	bool IsActive() const
	{ 
		return mCounters.size() && std::any_of(mCounters.begin(), mCounters.end(), [](auto& pair) { return pair.second > 0; });
	}

	void Push(TouchSourceID id) 
	{
		++mCounters[id];
	}

	void Pop(TouchSourceID id) 
	{
		assert(mCounters[id] > 0);
		--mCounters[id];
	}

private:
	std::unordered_map<TouchSourceID, u8> mCounters;
};