#include "TouchInputEventManager.h"
#include "CoreBaseApplication.h"
#include "ModuleInput.h"
#include "MultiTouchDevice.h"
#include "SpatialInteractionDevice.h"
#include "AttributePacking.h"
#include "Timer.h"
#include "Camera.h"

#include "TecLibs/Math/Algorithm.h"

#include <algorithm>

//#include "imgui.h"
#ifdef WUP
extern bool gIsVR;
#endif




//IMPLEMENT_AND_REGISTER_CLASS_INFO(TouchInputEventManager, TouchInputEventManager, Input);
IMPLEMENT_CLASS_INFO(TouchInputEventManager)


IMPLEMENT_CONSTRUCTOR(TouchInputEventManager)
, mTheInputModule(0)
, mCurrentTouchSupportRoot(0)
, mTriggerSquaredDist(100)
, mIsActive(true)
{
	StackedEventStateStruct	firstOne;
	mStackedEventState.push_back(firstOne);
	mTheInputModule = reinterpret_cast<ModuleInput*>(CoreGetModule(ModuleInput));
}


bool TouchInputEventManager::isRegisteredOnCurrentState(CoreModifiable* obj)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	return mStackedEventState.back().mEventMap.find(obj) != mStackedEventState.back().mEventMap.end();
}

// registered object wants to be called when event "type" occurs (start, end, update, cancel...)  
TouchEventState*	TouchInputEventManager::registerEvent(CoreModifiable* registeredObject,KigsID methodNameID, InputEventType type, InputEventManagementFlag flag, CoreModifiable* root_scene)
{
	
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	if (mStackedEventState.back().mEventMap.find(registeredObject) == mStackedEventState.back().mEventMap.end()) // need to create an mEntry
	{
		StackedEventStateStruct::EventMapEntry& lcurrentEntry = mStackedEventState.back().mEventMap[registeredObject];
	
		// We do the search for the root scene in the update if not found, no need to duplicate the code here
		lcurrentEntry.mRootScene3D = root_scene;
		lcurrentEntry.mTouchEventStateList.clear();
	}

	StackedEventStateStruct::EventMapEntry& currentEntry= mStackedEventState.back().mEventMap[registeredObject];
	kstl::vector<TouchEventState*>& eventlist = currentEntry.mTouchEventStateList;

	// search if not already there
	auto itlist = eventlist.begin();
	auto itend = eventlist.end();
	while(itlist != itend)
	{
		// you can not register several time for the same type
		// even with different flags or method name
		if ((*itlist)->mType == type)
		{
			KIGS_WARNING("trying to register the same input event several time", 1);
			return (*itlist);
		}
		++itlist;
	}

	TouchEventState* toAdd=0;
	
	switch (type)
	{
	case Click:
		toAdd = new TouchEventStateClick(methodNameID, flag/*, touchSupportObject*/, type);
		break;
	case Swipe:
		toAdd = new TouchEventStateSwipe(methodNameID, flag/*, touchSupportObject*/, type);
		break;
	case Pinch:
		toAdd = new TouchEventStatePinch(methodNameID, flag/*, touchSupportObject*/, type);
		break;
	case Scroll:
		toAdd = new TouchEventStateScroll(methodNameID, flag/*, touchSupportObject*/, type);
		break;
	case DragAndDrop:
		break;
	case DirectTouch:
		toAdd = new TouchEventStateDirectTouch(methodNameID, flag/*, touchSupportObject*/, type);
		break;
	case DirectAccess:
		toAdd = new TouchEventStateDirectAccess(methodNameID, flag/*, touchSupportObject*/, type);
		break;
	case InputSwallow:
		toAdd = new TouchEventStateInputSwallow(methodNameID, flag/*, touchSupportObject*/, type);
		break;

	default:
		if ((type >= UserDefinedStart) && (type < MaxType)) // user defined
		{
			// TODO
		}
	}
	if (toAdd)
	{
		eventlist.push_back(toAdd);
		KigsCore::Connect(registeredObject, "Destroy", this, "OnDestroyCallback");
		++(mStackedEventState.back().mRegisteredCount[type]);
	}

	return toAdd;
}

// touch support are :
// - rendering screens
// - camera
// - HoloPanel
// - UIScene
// ...
// they are organized like a tree, with window rendering screen as root(s) 
void	TouchInputEventManager::addTouchSupport(CoreModifiable* ts, CoreModifiable* parent) 
{
	//check if it's of the good type...

	std::lock_guard<std::recursive_mutex> lk{ mMutex };

	if (!ts->HasMethod("GetDataInTouchSupport"))
	{
		KIGS_ERROR("no support for GetDataInTouchSupport", 1);
	}

	// search if already there

	for (auto& root : mTouchSupportTreeRootList)
	{
		touchSupportTreeNode* found = root.searchNode(ts);
		if (found)
		{
			return; // just exit
		}
	}
	KigsCore::Connect(ts, "Destroy", this, "OnDestroyTouchSupportCallback");
	if (parent) // search parent in tree
	{
		bool foundOne = false;
		
		for (auto& root : mTouchSupportTreeRootList)
		{
			touchSupportTreeNode* found = root.searchNode(parent);
			if (found)
			{
				touchSupportTreeNode toAdd;
				toAdd.mCurrentNode = ts;
				toAdd.mParentNode = parent;
				found->mSons.push_back(toAdd);
				foundOne = true;
				break;
			}
				
		}
		if (!foundOne)
		{
			for (auto& unmapped : mTemporaryUnmappedTouchSupport)
			{
				touchSupportTreeNode* found = unmapped.searchNode(parent);
				if (found)
				{
					touchSupportTreeNode toAdd;
					toAdd.mCurrentNode = ts;
					toAdd.mParentNode = parent;
					found->mSons.push_back(toAdd);
					foundOne = true;
					break;
				}
			}

			if (!foundOne)
			{
				// parent not found => add it to temporary list
				touchSupportTreeNode	toAdd;
				toAdd.mCurrentNode = ts;
				toAdd.mParentNode = parent;
				mTemporaryUnmappedTouchSupport.push_back(toAdd);
				//dumpTouchSupportTrees();
				return;
			}
		}
	}
	else
	{
		touchSupportTreeNode	toAdd;
		toAdd.mCurrentNode = ts;
		mTouchSupportTreeRootList.push_back(toAdd);
	}

	manageTemporaryUnmappedTouchSupport(ts, parent);
	//dumpTouchSupportTrees();
}

void	TouchInputEventManager::manageTemporaryUnmappedTouchSupport(CoreModifiable* ts, CoreModifiable* parent)
{
	if (mTemporaryUnmappedTouchSupport.size()==0)
	{
		return;
	}

	bool doItAgain = false;
	CoreModifiable* recurse_ts = 0;
	CoreModifiable* recurse_parent = 0;

	// search if ts is parent of one elem in mTemporaryUnmappedTouchSupport

	auto ittmpfound = mTemporaryUnmappedTouchSupport.begin();
	auto ittmpfoundE = mTemporaryUnmappedTouchSupport.end();

	while (ittmpfound != ittmpfoundE)
	{
		if ((*ittmpfound).mParentNode == ts)
		{

			// search current node and add ittmpfound

			auto itfound = mTouchSupportTreeRootList.begin();
			auto itE = mTouchSupportTreeRootList.end();
			while (itfound != itE)
			{
				touchSupportTreeNode* found = (*itfound).searchNode(ts);
				if (found)
				{
					found->mSons.push_back((*ittmpfound));
					// prepare the recursion
					recurse_ts = (*ittmpfound).mCurrentNode;
					recurse_parent = (*ittmpfound).mParentNode;
					doItAgain = true;
					break;
				}
				++itfound;
			}

			mTemporaryUnmappedTouchSupport.erase(ittmpfound);
			
			break;
		}
		++ittmpfound;
	}

	if (recurse_ts != 0)
	{
		manageTemporaryUnmappedTouchSupport(recurse_ts, recurse_parent);
	}

	if (doItAgain) // if several sons are waiting for the same parent
	{
		manageTemporaryUnmappedTouchSupport(ts, parent);
	}
}

// return true if found
bool	TouchInputEventManager::removeTemporaryUnmappedTouchSupport(CoreModifiable* ts)
{

	auto itfound = mTemporaryUnmappedTouchSupport.begin();
	auto itE = mTemporaryUnmappedTouchSupport.end();
	while (itfound != itE)
	{
		touchSupportTreeNode* found = (*itfound).searchNode(ts);
		if (found)
		{
			if (found->mParentNode)
			{
				touchSupportTreeNode* pfound = (*itfound).searchNode(found->mParentNode);
				if (pfound)
				{
					// search iterator
					auto toErase = pfound->mSons.begin();
					auto toEraseE = pfound->mSons.end();
					while (toErase != toEraseE)
					{
						if ((*toErase).mCurrentNode == ts)
						{
							pfound->mSons.erase(toErase);
							break;
						}
						++toErase;
					}
				}
				else
				{
					mTemporaryUnmappedTouchSupport.erase(itfound);
				}

			}
			else // remove root 
			{
				mTemporaryUnmappedTouchSupport.erase(itfound);
			}
			return true;
		}
		++itfound;
	}


	return false;
}




void TouchInputEventManager::dumpTouchSupportTrees()
{
	printf("Current Touch Supports : \n");

	std::function<void(touchSupportTreeNode*, int)> func = [&](touchSupportTreeNode* current_node, int depth)
	{
		for (int i = 0; i < depth; ++i)
			printf("\t");
		printf("| - %s\n", current_node->mCurrentNode->getName().c_str());

		for (auto& son : current_node->mSons)
		{
			func(&son, depth + 1);
		}
	};

	for (auto& root : mTouchSupportTreeRootList)
	{
		func(&root, 0);
	}

	printf("Unmmapped Touch Supports : \n");
	for (auto& unmapped : mTemporaryUnmappedTouchSupport)
	{
		func(&unmapped, 0);
	}

}

void	TouchInputEventManager::removeTouchSupport(CoreModifiable* ts)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };

	if (removeTemporaryUnmappedTouchSupport(ts))
	{
		KigsCore::Disconnect(ts, "Destroy", this, "OnDestroyTouchSupportCallback");
		return;
	}

	auto itfound = mTouchSupportTreeRootList.begin();
	auto itE = mTouchSupportTreeRootList.end();
	while (itfound != itE)
	{
		touchSupportTreeNode* found = (*itfound).searchNode(ts);
		if (found)
		{
			for (auto& son : found->mSons)
			{
				for (auto& grandson : son.mSons)
				{
					mTemporaryUnmappedTouchSupport.push_back(grandson);
				}
			}
			if (found->mParentNode)
			{
				touchSupportTreeNode* pfound = (*itfound).searchNode(found->mParentNode);

				// search iterator
				auto toErase = pfound->mSons.begin();
				auto toEraseE = pfound->mSons.end();
				while (toErase != toEraseE)
				{
					if ((*toErase).mCurrentNode == ts)
					{
						pfound->mSons.erase(toErase);
						break;
					}
					++toErase;
				}
				
			}
			else // remove root 
			{
				mTouchSupportTreeRootList.erase(itfound);
			}

			KigsCore::Disconnect(ts, "Destroy", this, "OnDestroyTouchSupportCallback");

			break;
		}
		++itfound;
	}
}

// deactivate current events and create a new management state (ie for popup)
void TouchInputEventManager::pushNewState()
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	StackedEventStateStruct	newOne;
	mStackedEventState.push_back(newOne);
}

// go back to previous activated state
void TouchInputEventManager::popState()
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	// unregister everything in current state
	StackedEventStateStruct& state = mStackedEventState.back();

	while (state.mEventMap.size())
	{
		auto it = state.mEventMap.begin();
		unregisterObjectOnCurrentState(state, (*it).first);
	}

	mStackedEventState.pop_back();
}


void TouchInputEventManager::ResetCurrentStates()
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	// unregister everything in current state
	StackedEventStateStruct& state = mStackedEventState.back();

	for (auto& eventsInMap : state.mEventMap)
	{
		for (auto& toreset : eventsInMap.second.mTouchEventStateList)
		{
			toreset->Reset();
		}
	}
	mNearInteractionActiveItemsLeft.clear();
	mNearInteractionActiveItemsRight.clear();
}

void TouchInputEventManager::unregisterEvent(CoreModifiable* registeredObject, InputEventType type)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	auto itc = mStackedEventState.rbegin();
	auto ite = mStackedEventState.rend();

	bool found = false;

	while (itc != ite)
	{
		StackedEventStateStruct& state = (*itc);
		found |= unregisterEventOnCurrentState(state, registeredObject, type);

		// if unregister was done stop
		if (found)
		{
			if (type == DirectTouch) 
			{
				mNearInteractionActiveItemsLeft.erase(registeredObject);
				mNearInteractionActiveItemsRight.erase(registeredObject);
			}
			break;
		}

		++itc;
	}

	if (found == false)
	{
		KIGS_WARNING("trying to unregister a not found input event", 1);
	}
}

void TouchInputEventManager::unregisterObject(CoreModifiable* registeredObject)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	auto itc = mStackedEventState.rbegin();
	auto ite = mStackedEventState.rend();
	while (itc != ite)
	{
		StackedEventStateStruct& state = (*itc);
		unregisterObjectOnCurrentState(state, registeredObject);
		++itc;
	}

	mNearInteractionActiveItemsLeft.erase(registeredObject);
	mNearInteractionActiveItemsRight.erase(registeredObject);
}

bool TouchInputEventManager::unregisterEventOnCurrentState(StackedEventStateStruct& state, CoreModifiable* registeredObject, InputEventType type)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	if (state.mRegisteredCount[type] > 0)
	{
		auto itfound = state.mEventMap.find(registeredObject);

		if (itfound == state.mEventMap.end())
		{
			return false;
		}

		kstl::vector<TouchEventState*>& eventlist = (*itfound).second.mTouchEventStateList;

		bool found = false;
		// search event type in event list
		auto itlist = eventlist.begin();
		auto itend = eventlist.end();
		while (itlist != itend)
		{
			if ((*itlist)->mType == type)
			{
				state.mRegisteredCount[type]--;
				found = true;
				delete (*itlist);
				eventlist.erase(itlist);

				if (eventlist.size() == 0)
				{
					state.mEventMap.erase(itfound);
				}
				break;
			}
			++itlist;
		}

		if (!found)
		{
			return false;
		}
		return true;
	}
	return false;
	
}

bool TouchInputEventManager::unregisterObjectOnCurrentState(StackedEventStateStruct& state, CoreModifiable* registeredObject)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	auto itfound = state.mEventMap.find(registeredObject);

	if (itfound == state.mEventMap.end())
	{
		return false;
	}

	kstl::vector<TouchEventState*>& eventlist = (*itfound).second.mTouchEventStateList;

	// disconnect each event
	auto itlist = eventlist.begin();
	auto itend = eventlist.end();
	while (itlist != itend)
	{
		state.mRegisteredCount[(*itlist)->mType]--;
		delete (*itlist);
		++itlist;
	}
	state.mEventMap.erase(itfound);

	return true;
}

#include "GLSLDebugDraw.h"

void TouchInputEventManager::Update(const Timer& timer, void* addParam)
{
	bool force_click = mForceClick;
	mForceClick = false;

	// if manager is inactive, just stop update here
	if (!mIsActive)
	{
		return;
	}

	auto spatial_interaction_device = mTheInputModule->GetSpatialInteraction();
	auto camera = spatial_interaction_device ? (Camera*)spatial_interaction_device->getValue<CoreModifiable*>("GazeCamera") : nullptr;

	u16 any_touch_state = 0;
	if (force_click) any_touch_state = 1;
	v3f any_pos;

	// TODO ask module input for current touch device(s) (multitouch, mouse, hololens gesture...)
	kigs::unordered_map<TouchSourceID, TouchEventState::TouchInfos>	Touches;
	
	MultiTouchDevice* mtDevice = mTheInputModule->GetMultiTouch();
	if (mTheInputModule->GetMouse() && !mtDevice)
	{
		TouchEventState::TouchInfos mouseTouch;
		mouseTouch.ID = TouchSourceID::Mouse;
		mouseTouch.posInfos.flag = 0;
		mouseTouch.posInfos.dir = { 0, 0, 0 };
		mouseTouch.posInfos.origin = { 0, 0, 0 };
		mouseTouch.posInfos.pos = { 0, 0, 0 };
		mouseTouch.has_position = true;

		mouseTouch.in_touch_support = 0;
		
		mouseTouch.touch_state = (force_click || mTheInputModule->GetMouse()->getButtonState(MouseDevice::LEFT) != 0) ? 1 : 0;
		mouseTouch.touch_state |= (mTheInputModule->GetMouse()->getButtonState(MouseDevice::RIGHT) != 0) ? 2 : 0;
		mouseTouch.touch_state |= (mTheInputModule->GetMouse()->getButtonState(MouseDevice::MIDDLE) != 0) ? 4 : 0;

		any_touch_state = any_touch_state | mouseTouch.touch_state;

		mTheInputModule->GetMouse()->getPos(mouseTouch.posInfos.pos.x, mouseTouch.posInfos.pos.y);
		any_pos = mouseTouch.posInfos.pos;
		
		if(!spatial_interaction_device)
			Touches[mouseTouch.ID] = mouseTouch;
	}

	if (mtDevice)
	{
		int currentTouchCount = mtDevice->getMaxTouch();
		for (int i = 0; i < currentTouchCount; i++)
		{
			if (mtDevice->getTouchState(i))
			{
				TouchEventState::TouchInfos mouseTouch;
				mouseTouch.ID = (TouchSourceID)((u32)TouchSourceID::MultiTouch_0 + i);
				mouseTouch.has_position = true;
				mouseTouch.posInfos.flag = 0;
				mouseTouch.posInfos.dir = { 0, 0, 0 };
				mouseTouch.posInfos.origin = { 0, 0, 0 };

				mouseTouch.touch_state = 1;
				mouseTouch.posInfos.pos = { 0, 0, 0 };

				mtDevice->getTouchPos(i, mouseTouch.posInfos.pos.x, mouseTouch.posInfos.pos.y);
				Touches[mouseTouch.ID] = mouseTouch;
			}
		}
	}
	Interaction* main_interaction = nullptr;
	if (spatial_interaction_device)
	{
		auto& interactions = spatial_interaction_device->GetInteractions();
		
		for (auto& itr : interactions)
		{
			//dd::line(itr.second.Position, itr.second.Position + itr.second.Forward, itr.second.pressed ? v3f{ 0,1,0 } : v3f{ 1,1,1 }, 0.02f);
			//dd::arrow(itr.second.Position, itr.second.Position + itr.second.Up*0.05f, v3f{ 0,0,1 }, 0.02f);

			auto& interaction = itr.second;
			if(interaction.allowed)
			{
				TouchEventState::TouchInfos interaction_infos;
				
				interaction_infos.has_position = false;
				interaction_infos.posInfos.flag = 0;
				interaction_infos.starting_touch_support = camera;
				interaction_infos.need_starting_touch_support_transform = false;
				interaction_infos.posInfos.setHas3DInfos(true);
				interaction_infos.interaction = &interaction;

				if (interaction.index_tip.has_value())
				{
					auto orientation = (interaction.index_tip->orientation * v3f(0, -1, 1)).Normalized();
					if (Dot(orientation, camera->GetGlobalViewVector()) > 0)
					{
						interaction_infos.ID = interaction.handedness == Handedness::Left ? TouchSourceID::SpatialInteractionLeft : TouchSourceID::SpatialInteractionRight;
						interaction_infos.posInfos.dir = (interaction.index_tip->position - camera->GetGlobalPosition()).Normalized();
						interaction_infos.posInfos.pos = interaction.index_tip->position;
						interaction_infos.posInfos.origin = interaction_infos.posInfos.pos - interaction_infos.posInfos.dir * getSpatialInteractionOffset();
						interaction_infos.touch_state = (force_click || interaction.pressed) ? 1 : 0;
						interaction_infos.posInfos.min_distance = 0.0;
						interaction_infos.posInfos.max_distance = (mEventCaptureObject && mEventCapturedEventID == interaction_infos.ID) ? DBL_MAX : 0.3;
						
						Touches[interaction_infos.ID] = interaction_infos;
					}
				}
				if(GetNearInteractionActiveItems(interaction.handedness).size() == 0 || !interaction.index_tip.has_value())
				{
					auto orientation = (interaction.palm->orientation * v3f(0, -1, 1)).Normalized();
					if (Dot(orientation, camera->GetGlobalViewVector()) > 0)
					{
						if (interaction.palm.has_value())
						{
							interaction_infos.posInfos.pos = interaction.palm->position;
							interaction_infos.posInfos.dir = ((interaction.palm->position - camera->GetGlobalPosition()).Normalized() + interaction.palm->orientation * v3f(interaction.handedness == Handedness::Left ? -0.15f : 0.15f, 0, 0.33f)).Normalized();
							interaction_infos.posInfos.origin = interaction.palm->position;
						}
						else
						{
							interaction_infos.posInfos.pos = interaction.Position;
							interaction_infos.posInfos.dir = interaction.Forward;
							interaction_infos.posInfos.origin = interaction.Position;
						}
						interaction_infos.posInfos.min_distance = -DBL_MAX;
						interaction_infos.posInfos.max_distance = DBL_MAX;
						interaction_infos.ID = interaction.handedness == Handedness::Left ? TouchSourceID::SpatialInteractionRayLeft : TouchSourceID::SpatialInteractionRayRight;
						interaction_infos.touch_state = (force_click || interaction.pressed) ? 1 : 0;
						any_touch_state = any_touch_state | interaction_infos.touch_state;
						//dd::line(interaction_infos.posInfos.pos, interaction_infos.posInfos.pos + interaction_infos.posInfos.dir*0.2f, v3f{ 1, 1, 1 });
						Touches[interaction_infos.ID] = interaction_infos;
					}
				}
			}
		}
	}
	
	if(camera && mUseGazeAsTouchDevice)
	{
		TouchEventState::TouchInfos gazeTouch;
		gazeTouch.has_position = false;
		gazeTouch.posInfos.flag = 0;
		gazeTouch.posInfos.dir = camera->GetGlobalViewVector();
		gazeTouch.posInfos.origin = camera->GetGlobalPosition();
		gazeTouch.ID = TouchSourceID::Gaze;
		gazeTouch.touch_state = any_touch_state;
		gazeTouch.interaction = main_interaction;
		if (main_interaction)
		{
			gazeTouch.posInfos.pos = main_interaction->Position;
			gazeTouch.starting_touch_support = camera;
			gazeTouch.posInfos.setHas3DInfos(true);
		}
		else
		{
			gazeTouch.has_position = true;
			gazeTouch.posInfos.pos = any_pos;
		}
		Touches[gazeTouch.ID] = gazeTouch;
	}

	auto touches_copy = Touches;
	for (auto t : mLastFrameTouches)
	{
		if (Touches.find(t.first) == Touches.end())
		{
			t.second.touch_ended = true;
			t.second.touch_state = 0;
			Touches[t.first] = t.second;
		}
	}

	mLastFrameTouches = touches_copy;

	mLock = std::unique_lock<std::recursive_mutex>{ mMutex };
	

	if( (Touches.size() > 0) && (mTouchSupportTreeRootList.size()>1)) // there's a touch and more than one touchsupport root 
		// => check if current touchsupport has changed
	{
		// find touch support root for current touch position (only one possible at this time ? (don't manage overlapping windows)

		//touchSupportTreeNode* 	CurrentTouchSupportRoot = 0;

		auto itTS = mTouchSupportTreeRootList.begin();
		auto itTSe = mTouchSupportTreeRootList.end();

		while (itTS != itTSe)
		{
			if ((*itTS).mCurrentNode->SimpleCall<bool>("IsValidTouchSupport", Touches.begin()->second.posInfos.pos))
			{
				mCurrentTouchSupportRoot = &(*itTS);
			}
			++itTS;
		}
	}
	else if(mTouchSupportTreeRootList.size() == 1)
	{
		mCurrentTouchSupportRoot = &mTouchSupportTreeRootList[0];
	}
	

	// no touch support ? return now
	if (mCurrentTouchSupportRoot == 0)
	{
		return;
	}

	//kigsprintf("touch before transform : %f %f \n", (*Touches.begin()).posInfos.pos.x, (*Touches.begin()).posInfos.pos.y);

	// now do transformation hierarchy for mCurrentTouchSupportRoot
	kigs::unordered_map<CoreModifiable*, kigs::unordered_map<TouchSourceID, TouchEventState::TouchInfos>>	transformedInfosMap;
	transformTouchesInTouchSupportHierarchy(mCurrentTouchSupportRoot, transformedInfosMap, Touches);


	// first step : manage a list of item per Scene3D parent
	kigs::unordered_map<CoreModifiable*, kstl::vector<CoreModifiable*> > perScene3DMap;

	StackedEventStateStruct& state = mStackedEventState.back();

	auto	itO = state.mEventMap.begin();
	auto	itE = state.mEventMap.end();

	while (itO != itE)
	{
		if ((*itO).second.mRootScene3D == nullptr) // if not set, search again
		{
			(*itO).second.mRootScene3D = (*itO).first->getRootParentByType("UINode3DLayer");
			if((*itO).second.mRootScene3D == nullptr)
				(*itO).second.mRootScene3D = (*itO).first->getRootParentByType("Scene3D");
		}
		CoreModifiable* foundParentScene3D = (*itO).second.mRootScene3D;
		if (foundParentScene3D)
		{
			perScene3DMap[foundParentScene3D].push_back((*itO).first);
		}
		
		++itO;
	}

	// second step : associate each rendering screen to a priority sorted list of Scene3D

	kigs::unordered_map<CoreModifiable*, kstl::set< Scene3DAndCamera, Scene3DAndCamera::PriorityCompare > >	perRenderingScreenSortedMap;

	auto itScene = perScene3DMap.begin();
	auto itSceneE = perScene3DMap.end();

	while (itScene != itSceneE)
	{
		// layer or 3D scene ?
		

		if ((*itScene).first->isSubType("Abstract2DLayer"))
		{
			// get associated rendering screen
			CoreModifiable*	getScreen=nullptr;
			(*itScene).first->getValue("RenderingScreen", getScreen);

			CoreModifiable* renderingScreen = (CoreModifiable*)getScreen;

			// check that rendering screen is in active touch support list
			touchSupportTreeNode * foundts=mCurrentTouchSupportRoot->searchNode(renderingScreen);

			if (foundts)
			{
				kstl::set< Scene3DAndCamera, Scene3DAndCamera::PriorityCompare >& currentVector = perRenderingScreenSortedMap[renderingScreen];

				Scene3DAndCamera	toPush;
				toPush.scene3D = (*itScene).first;
				toPush.camera = 0;
				
				currentVector.insert(toPush);
			}
		}
		else if ((*itScene).first->isSubType("UINode3DLayer"))
		{

			// UINode3DLayer is a "virtual" rendering screen of sorts
			kstl::set< Scene3DAndCamera, Scene3DAndCamera::PriorityCompare >& currentVector = perRenderingScreenSortedMap[(*itScene).first];

			Scene3DAndCamera	toPush;
			toPush.scene3D = (*itScene).first;
			toPush.camera = 0;

			currentVector.insert(toPush);
		}
		else // it's a scene 3D, so search each camera 
		{
			kstl::vector<CoreModifiable*> cameras;
			(*itScene).first->SimpleCall("GetCameraVector", cameras);

			if (cameras.size())
			{
				for (auto foreachinstance : cameras)
				{
					touchSupportTreeNode * foundcamera = mCurrentTouchSupportRoot->searchNode(foreachinstance);

					if (foundcamera)
					{
						// get associated rendering screen
						CoreModifiable*	getScreen=nullptr;
						foreachinstance->getValue("RenderingScreen", getScreen);

						CoreModifiable* renderingScreen = (CoreModifiable*)getScreen;

						// check that rendering screen is in active touch support list
						touchSupportTreeNode * foundts = mCurrentTouchSupportRoot->searchNode(renderingScreen);

						if (foundts)
						{
							kstl::set< Scene3DAndCamera, Scene3DAndCamera::PriorityCompare >& currentVector = perRenderingScreenSortedMap[renderingScreen];

							Scene3DAndCamera toPush;
							toPush.scene3D = (*itScene).first;
							toPush.camera = foundcamera;

							currentVector.insert(toPush);
						}
					}
				}
			}
		}
		
		++itScene;
	}

	// Build the ordered element list for each touch
	kigs::unordered_map<TouchSourceID, kstl::vector<SortedElementNode>> flat_trees;
	for (auto& t : Touches)
	{
		RecursiveFlattenTreeForTouchID(flat_trees[t.first], mCurrentTouchSupportRoot, perRenderingScreenSortedMap, perScene3DMap, transformedInfosMap, t.first);
	}
	mInUpdate = true;

	// Update events for each touch
	for (auto& pair : flat_trees)
	{
		LinearCallEventUpdate(pair.second, timer, transformedInfosMap, pair.first);
	}
	mInUpdate = false;
	mDestroyedThisFrame.clear();
}


void TouchInputEventManager::RecursiveFlattenTreeForTouchID(kstl::vector<SortedElementNode>& flat_tree, touchSupportTreeNode* CurrentTouchSupport,
	kigs::unordered_map<CoreModifiable*, kstl::set< Scene3DAndCamera, Scene3DAndCamera::PriorityCompare > >& perRenderingScreenSortedMap,
	kigs::unordered_map<CoreModifiable*, kstl::vector<CoreModifiable*> >& perScene3DMap,
	kigs::unordered_map<CoreModifiable*, kigs::unordered_map<TouchSourceID, TouchEventState::TouchInfos>>& transformedInfosMap, TouchSourceID touch_id)
{

	if (perRenderingScreenSortedMap.find(CurrentTouchSupport->mCurrentNode) != perRenderingScreenSortedMap.end())
	{
		for (auto attachedScene3D : perRenderingScreenSortedMap[CurrentTouchSupport->mCurrentNode])
		{
			touchSupportTreeNode * cameraOrRenderingScreen = CurrentTouchSupport;
			if (attachedScene3D.camera)
			{
				cameraOrRenderingScreen = attachedScene3D.camera;
			}

			SortItemsFrontToBackParam param;
			param.toSort = perScene3DMap[attachedScene3D.scene3D];

			struct SonStruct
			{
				touchSupportTreeNode*	tsn;
				bool					currentNodeCatchEvent;
			};
			kigs::unordered_map<CoreModifiable*, SonStruct>	sonTouchSupport;
			for (auto& SonSupportTouch : cameraOrRenderingScreen->mSons)
			{
				SonStruct toInsert{ &SonSupportTouch, false};
				// this son is part of current scene, so add it to the item vector, and ask the scene to sort it

				// check if SonSupportTouch.currentNode is already in the list
				if (std::find(param.toSort.begin(), param.toSort.end(), SonSupportTouch.mCurrentNode) != param.toSort.end())
					toInsert.currentNodeCatchEvent = true;
				else
					param.toSort.push_back(SonSupportTouch.mCurrentNode);

				sonTouchSupport[SonSupportTouch.mCurrentNode] = toInsert;
			}

			auto& touches = transformedInfosMap[cameraOrRenderingScreen->mCurrentNode];
			
			auto it = touches.find(touch_id);

			param.camera = attachedScene3D.camera ? cameraOrRenderingScreen->mCurrentNode : nullptr;

			param.position = it->second.posInfos.pos;
			param.origin = it->second.posInfos.origin;
			param.direction = it->second.posInfos.dir;
			param.min_distance = it->second.posInfos.min_distance;
			param.max_distance = it->second.posInfos.max_distance;
			param.touchID = touch_id;

			attachedScene3D.scene3D->SimpleCall("SortItemsFrontToBack", param);
			
			for (auto&& item_tuple : param.sorted)
			{
				SortedElementNode	sonItemToAdd;
				
				auto item = std::get<0>(item_tuple);
				Hit hit = std::get<1>(item_tuple);
				
				//if (!hit.HitNode) 
				//	hit = it->second.posInfos.hit;

				auto foundAsTouchSupport = sonTouchSupport.find(item);
				if (foundAsTouchSupport != sonTouchSupport.end())
				{
					auto SonSupportTouch = (*foundAsTouchSupport).second.tsn;
					RecursiveFlattenTreeForTouchID(flat_tree, SonSupportTouch, perRenderingScreenSortedMap, perScene3DMap, transformedInfosMap, touch_id);

					if ((*foundAsTouchSupport).second.currentNodeCatchEvent) // also add it as event item
					{
						sonItemToAdd.element = item;
						sonItemToAdd.hit = hit;
						sonItemToAdd.touchSupport = cameraOrRenderingScreen->mCurrentNode;
						flat_tree.push_back(sonItemToAdd);
					}
				}
				else
				{
					sonItemToAdd.element = item;
					sonItemToAdd.hit = hit;
					sonItemToAdd.touchSupport = cameraOrRenderingScreen->mCurrentNode;
					flat_tree.push_back(sonItemToAdd);
				}
			}
		}
	}
	else
	{
		if (CurrentTouchSupport->mSons.size())
		{
			if (CurrentTouchSupport->mSons.size() == 1)
			{
				RecursiveFlattenTreeForTouchID(flat_tree, &CurrentTouchSupport->mSons[0], perRenderingScreenSortedMap, perScene3DMap, transformedInfosMap, touch_id);
			}
			else
			{
				//KIGS_WARNING("Something went wrong in TouchInputEventManager::recursiveBuildFullSortedElementList", 2);
			}

		}
	}
}

void	TouchInputEventManager::LinearCallEventUpdate(kstl::vector<SortedElementNode>& flat_tree, const Timer& timer, kigs::unordered_map<CoreModifiable*, kigs::unordered_map<TouchSourceID, TouchEventState::TouchInfos> >& transformedInfosMap, TouchSourceID touch_id)
{
	StackedEventStateStruct& state = mStackedEventState.back();
	u32 swallowMask = 0;
	
	for (auto& element : flat_tree)
	{
		if (mDestroyedThisFrame.find(element.element) == mDestroyedThisFrame.end() && mDestroyedThisFrame.find(element.touchSupport) == mDestroyedThisFrame.end())
		{
			element.element->GetRef(); // In case a modifiable decide to commit suicide during one of its callbacks

			StackedEventStateStruct::EventMapEntry& currentEntry = state.mEventMap[element.element];

			auto	itEvent = currentEntry.mTouchEventStateList.begin();
			auto	itEventE = currentEntry.mTouchEventStateList.end();

			unsigned int swallowMaskResult = swallowMask;
			while (itEvent != itEventE)
			{
				TouchEventState* currentStruct = (*itEvent);
				unsigned int swallowMaskCurrent = swallowMask;
				auto& touches = transformedInfosMap[element.touchSupport];
				//auto it = std::find_if(touches.begin(), touches.end(), [touch_id](auto&& info) { return info.ID == touch_id; });
				auto it = touches.find(touch_id);
				if(!element.hit.HitNode)
					it->second.object_hit = &it->second.posInfos.hit;
				else
					it->second.object_hit = &element.hit;
				mLock.unlock();
				currentStruct->Update(this, timer, element.element, it->second, swallowMaskCurrent);
				mLock.lock();
				swallowMaskResult |= swallowMaskCurrent;
				++itEvent;
			}
			swallowMask = swallowMaskResult;
			element.element->Destroy();
		}
	}
}

void	TouchInputEventManager::transformTouchesInTouchSupportHierarchy(touchSupportTreeNode* current, kigs::unordered_map<CoreModifiable*, kigs::unordered_map<TouchSourceID, TouchEventState::TouchInfos>>& resultmap, kigs::unordered_map<TouchSourceID, TouchEventState::TouchInfos>& Touches)
{
	auto itTouches = Touches.begin();
	auto itTouchesE = Touches.end();

	while (itTouches != itTouchesE)
	{
		// copy touch infos
		TouchEventState::TouchInfos& cTouchinfosIn = itTouches->second;
		TouchEventState::TouchInfos cTouchinfosOut = itTouches->second;

		bool isIn=current->mCurrentNode->SimpleCall<bool>("GetDataInTouchSupport", cTouchinfosIn.posInfos, cTouchinfosOut.posInfos);
		cTouchinfosOut.in_touch_support = isIn ? 1 : 0;

		if (cTouchinfosOut.starting_touch_support && current->mCurrentNode != cTouchinfosOut.starting_touch_support)
		{
			cTouchinfosOut.posInfos = itTouches->second.posInfos; // Don't transform until we find starting touch support
		}
		else if(current->mCurrentNode == cTouchinfosOut.starting_touch_support)
		{
			if(!itTouches->second.need_starting_touch_support_transform)
				cTouchinfosOut.posInfos = itTouches->second.posInfos;
			cTouchinfosOut.starting_touch_support = nullptr;
		}

		//if(isIn)
		//	printf("transformed coords in %s = %f %f %f - %f %f %f\n", current->currentNode->getName().c_str(), cTouchinfos.posInfos.pos.x, cTouchinfos.posInfos.pos.y, cTouchinfos.posInfos.pos.z, cTouchinfos.tmove.x, cTouchinfos.tmove.y, cTouchinfos.tmove.z);

		resultmap[current->mCurrentNode][itTouches->first] = cTouchinfosOut;

		++itTouches;
	}

	// now recurse to sons 

	for (u32 i = 0; i < current->mSons.size(); i++)
	{
		transformTouchesInTouchSupportHierarchy(&current->mSons[i], resultmap, resultmap[current->mCurrentNode]);
	}

}

TouchInputEventManager::~TouchInputEventManager()
{
	while (mStackedEventState.size())
	{
		popState();
	}
}


void TouchInputEventManager::ManageCaptureObject(InputEvent& ev, CoreModifiable* target)
{
	if (ev.capture_inputs) 
	{
		mEventCaptureObject = target;
		mEventCapturedEventID = ev.touch_id;
	}
	else if (mEventCaptureObject == target) mEventCaptureObject = nullptr;
}

DEFINE_METHOD(TouchInputEventManager, OnDestroyCallback)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };

	if (mInUpdate)
		mDestroyedThisFrame.insert(sender);

	unregisterObject(sender);

	if (mEventCaptureObject == sender)
		mEventCaptureObject = nullptr;
	return false;
}

DEFINE_METHOD(TouchInputEventManager, OnDestroyTouchSupportCallback)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };

	if (mInUpdate)
		mDestroyedThisFrame.insert(sender);

	removeTouchSupport(sender);
	return false;
}

bool	operator<(const TouchEventState::TouchInfos& first, const TouchEventState::TouchInfos& other)
{
	return first.ID < other.ID;
}

// event updates
void TouchEventStateClick::Update(TouchInputEventManager* manager, const Timer& timer,CoreModifiable* target, const TouchInfos& touch, u32& swallowMask)
{
	bool swallow = (swallowMask & (1 << mType)) != 0 && (mFlag & IgnoreSwallow) == 0;
	swallow = swallow || !manager->AllowEventOn(target);
	
	ClickEvent ev;
	ev.type = mType;
	ev.swallow_mask = &swallowMask;
	ev.touch_id = touch.ID;
	ev.has_position = touch.has_position;
	ev.interaction = touch.interaction;
	ev.hit = *touch.object_hit;
	ev.item = target;

	auto touch_state = touch.touch_state;

	
	auto dist_from_finger_tip = Dot(ev.hit.HitPosition - (touch.posInfos.origin + touch.posInfos.dir * manager->getSpatialInteractionOffset()), touch.posInfos.dir);
	bool is_near_interaction = IsNearInteraction(touch.ID) && manager->GetNearInteractionActiveItems(touch.interaction->handedness).size() > 0;
	/*if (is_spatial_interaction && mSpatialInteractionAutoClickDistance > 0.0f) 
	{
		if (std::abs(dist - manager->getSpatialInteractionOffset()) < mSpatialInteractionAutoClickDistance)
			touch_state = 1;
		else
			touch_state = 0;
	}*/

	bool is_down = false;
	if (!swallow)
	{
		if (is_near_interaction && mSpatialInteractionAutoClickDistance > 0.0f && ev.hit.HitNode)
		{
			auto now = timer.GetTime();
			touch_state = 0;
			touch.interaction->near_interaction_distance = dist_from_finger_tip;

			bool is_touch_down = mCurrentClickStart.find(touch.ID) != mCurrentClickStart.end();

			if (dist_from_finger_tip > mSpatialInteractionAutoClickDistance && !is_touch_down)
			{
				mNearTouchLastAboveTime = now; // Allow touch down for 1 sec
			}

			if (dist_from_finger_tip < mSpatialInteractionAutoClickDistance)
			{
				if (!is_touch_down)
				{
					if (now - mNearTouchLastAboveTime < 1.0 && dist_from_finger_tip > 0.0)
					{
						touch_state = 1;
					}
				}
				else
				{
					touch_state = 1;
				}
			}
		}


		if (touch_state != 0) // a touch is there
		{
			is_down = true;
			// check if it was there before
			auto foundClick = mCurrentClickStart.find(touch.ID);
			if (foundClick == mCurrentClickStart.end())
			{
				PotentialClick	toStart;
				toStart.startPos = touch.posInfos.pos;
				toStart.currentPos = touch.posInfos.pos;
				toStart.clickCount = 0;
				toStart.buttonState = touch_state;
				toStart.startTime = timer.GetTime();
				toStart.ID = touch.ID;
				toStart.origin = touch.posInfos.origin;
				toStart.direction = touch.posInfos.dir;
				//toStart.start_dist = dist;
				//toStart.min_dist = dist;

				// call target to check if click start is "accepted"

				ev.state = StatePossible;
				ev.origin = touch.posInfos.origin;
				ev.direction = touch.posInfos.dir;
				ev.button_state_mask = (ClickEvent::Button)toStart.buttonState;
				ev.click_count = 0;
				ev.position = toStart.startPos;

				toStart.isValid = target->SimpleCall<bool>(mMethodNameID, ev);

				mCurrentClickStart[touch.ID] = toStart;
			}
			else
			{
				// click was already there, check if it's still valid (touch did not moved to far)
				PotentialClick& startc = (*foundClick).second;
				startc.currentPos = touch.posInfos.pos;
				if (startc.isValid)
				{
					double duration = timer.GetTime() - startc.startTime;
					//startc.min_dist = std::min(dist, startc.min_dist);
					
					if (is_near_interaction && mSpatialInteractionAutoClickDistance>0.0)
					{
						if (dist_from_finger_tip < -mSpatialInteractionAutoClickDistance)
						{
							startc.isValid = false;
						}
					}
					if (startc.buttonState != touch_state) // not the same button ?
					{
						startc.isValid = false;
					}
					else if (DistSquare(startc.startPos, touch.posInfos.pos) > manager->getTriggerSquaredDist())
					{
						startc.isValid = false;
					}
					else if (duration > mClickMaxDuration)
					{
						startc.isValid = false;
					}
					/*else if (is_spatial_interaction && dist > startc.last_dist)
					{
						startc.isValid = false;
					}*/
					if(startc.isValid)
					{
						ev.state = StateChanged;
						ev.origin = touch.posInfos.origin;
						ev.direction = touch.posInfos.dir;
						ev.button_state_mask = (ClickEvent::Button)startc.buttonState;
						ev.click_count = 0;
						ev.position = startc.currentPos;
						startc.isValid = target->SimpleCall<bool>(mMethodNameID, ev);
					}
					else
					{
						ev.state = StateFailed;
						ev.origin = touch.posInfos.origin;
						ev.direction = touch.posInfos.dir;
						ev.button_state_mask = (ClickEvent::Button)startc.buttonState;
						ev.click_count = 0;
						ev.position = startc.currentPos;
						target->SimpleCall<bool>(mMethodNameID, ev);
					}
				}
			}
		}
	}
	

	
	// check if a previous started click was ended
	if (!is_down)
	{
		auto itfind = mCurrentClickStart.find(touch.ID);
		if (itfind != mCurrentClickStart.end())
		{
			PotentialClick& endc = itfind->second;
			if (endc.isValid && !swallow)
			{
				// check if click time is ok
				double duration = timer.GetTime() - endc.startTime;
				if ((duration >= mClickMinDuration) && (duration <= mClickMaxDuration))
				{
					// call target to check if click end is "accepted"
					ev.origin = endc.origin;
					ev.direction = endc.direction;
					ev.button_state_mask = (ClickEvent::Button)endc.buttonState;
					ev.click_count = 0;
					ev.state = StateChanged;
					
					ev.position = endc.currentPos;
					

					if (target->SimpleCall<bool>(mMethodNameID, ev))
					{
						endc.clickCount = 1;

						bool click_end_found = false;

						// look in mCurrentClickEnd to see if an already existint click at almost same position is there
						auto itendclick = mCurrentClickEnd.find(touch.ID);
						if (itendclick != mCurrentClickEnd.end())
						{
							if (DistSquare(itendclick->second.startPos, endc.startPos) < manager->getTriggerSquaredDist())
							{
								if (itendclick->second.buttonState == endc.buttonState)
								{
									click_end_found = true;
									itendclick->second.clickCount++;
								}
							}
						}

						// a corresponding previous click was nos found, add a new one
						if (!click_end_found)
						{
							mCurrentClickEnd[touch.ID] = endc;
						}
					}
				}
			}
			mCurrentClickStart.erase(itfind);
		}
	}
	
	auto itendclick = mCurrentClickEnd.find(touch.ID);
	if (itendclick != mCurrentClickEnd.end())
	{
		bool toKeep = true;
		auto& click = itendclick->second;
		if (click.clickCount == mMaxClickCount)
		{
			// click recognized
			ev.state = StateRecognized;
			ev.origin = click.origin;
			ev.direction = click.direction;
			ev.button_state_mask = (ClickEvent::Button)click.buttonState;
			ev.click_count = click.clickCount;
			ev.position = click.currentPos;

			target->SimpleCall<bool>(mMethodNameID, ev);


			toKeep = false;
		}
		else
		{
			double duration = timer.GetTime() - click.startTime;
			if (duration > mClickMaxDuration)
			{
				if (click.clickCount >= mMinClickCount)
				{
					ev.state = StateRecognized;
					ev.origin = click.origin;
					ev.direction = click.direction;
					ev.button_state_mask = (ClickEvent::Button)click.buttonState;
					ev.click_count = click.clickCount;
					ev.position = click.currentPos;

					target->SimpleCall<bool>(mMethodNameID, ev);
				}
				toKeep = false;
			}
		}

		if (!toKeep)
		{
			mCurrentClickEnd.erase(itendclick);
		}
	}
}

void TouchEventStateDirectTouch::Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallowMask)
{
	bool swallow = (swallowMask & (1 << mType)) != 0 && (mFlag & IgnoreSwallow) == 0;
	swallow = swallow || !manager->AllowEventOn(target);
	
	DirectTouchEvent ev;
	ev.type = mType;
	ev.swallow_mask = &swallowMask;
	ev.touch_id = touch.ID;
	ev.interaction = touch.interaction;
	ev.hit = *touch.object_hit;
	ev.item = target;

	auto dist_from_finger_tip = Dot(ev.hit.HitPosition - (touch.posInfos.origin + touch.posInfos.dir*manager->getSpatialInteractionOffset()), touch.posInfos.dir);// Norm(ev.hit.HitPosition - touch.posInfos.pos);
	auto touch_state = touch.touch_state;
	bool is_near_interaction = IsNearInteraction(touch.ID);
	
	/*if (is_near_interaction && mSpatialInteractionAutoTouchDownDistance > 0.0f)
	{
		touch.interaction->near_interaction_distance = dist_from_finger_tip;
		if (std::abs(dist_from_finger_tip) < mSpatialInteractionAutoTouchDownDistance)
			touch_state = 1;
		else
			touch_state = 0;
	}*/

	// type 0 for hover
	ev.has_position = touch.has_position;
	ev.origin = touch.posInfos.origin;
	ev.direction = touch.posInfos.dir;
	ev.position = touch.posInfos.pos;
	ev.button_state = touch_state;
	ev.state = StatePossible;
	ev.touch_state = DirectTouchEvent::TouchHover;
	
	// We need to send StateEnded when the event is swallowed by someone above us
	bool isHover = !swallow && !touch.touch_ended && target->SimpleCall<bool>(mMethodNameID, ev);

	auto foundPrevious = mCurrentInfosMap.find(touch.ID);

	auto& near_active_items = manager->GetNearInteractionActiveItems(touch.interaction ? touch.interaction->handedness : Handedness::Left);

	// add this touch info if first time
	if (foundPrevious == mCurrentInfosMap.end())
	{
		CurrentInfos toAdd;
		toAdd.currentPos = touch.posInfos.pos;
		toAdd.state = isHover ? 1 : 0;
		toAdd.last_dist= dist_from_finger_tip;

		mCurrentInfosMap[touch.ID] = toAdd;

		if (isHover)
		{
			ev.state = StateBegan;
			ev.touch_state = DirectTouchEvent::TouchHover;
			target->SimpleCall<bool>(mMethodNameID, ev);
			manager->ManageCaptureObject(ev, target);
			if (is_near_interaction) near_active_items.insert(target);
		}
	}

	CurrentInfos&	current = mCurrentInfosMap[touch.ID];

	if (is_near_interaction && mSpatialInteractionAutoTouchDownDistance > 0.0f && ev.hit.HitNode)
	{
		auto now = timer.GetTime();
		touch_state = 0;
		touch.interaction->near_interaction_distance = dist_from_finger_tip;

		bool is_touch_down = (current.state & 6) != 0;

		if (dist_from_finger_tip > mSpatialInteractionAutoTouchDownDistance && !is_touch_down)
		{
			current.near_touch_last_above_time = now; // Allow touch down for 1 sec
		}
		
		if(dist_from_finger_tip < mSpatialInteractionAutoTouchDownDistance)
		{
			if (!is_touch_down)
			{
				if (now - current.near_touch_last_above_time < 1.0 && dist_from_finger_tip > 0.0)
				{
					touch_state = 1;
				}
			}
			else
			{
				touch_state = 1;
			}
		}
		ev.button_state = touch_state;
	}
	
	if (touch_state != 0) // a touch is there
	{
		if ((current.state & 6) == 0) // check if state has changed
		{
			// send touch down if hover
			if (isHover)
			{
				ev.state = StateBegan;
				ev.touch_state = DirectTouchEvent::TouchDown;
				target->SimpleCall<bool>(mMethodNameID, ev);
				manager->ManageCaptureObject(ev, target);
				current.state |= 2;
			}
			else
			{
				current.state |= 4;
			}

		}
		else // state change
		{
			if (isHover) 
			{
				ev.state = StateChanged;
				ev.touch_state = DirectTouchEvent::TouchDown;
				target->SimpleCall<bool>(mMethodNameID, ev);
				manager->ManageCaptureObject(ev, target);
			}
		}
	}
	else // no touch down
	{
		if ((current.state & 2) != 0) // check if state has changed
		{
			// send touch up 
			ev.state = StateBegan; // always send touch up event if we sent a touch down before
			ev.touch_state = DirectTouchEvent::TouchUp;
			ev.near_interaction_went_trough = is_near_interaction && mSpatialInteractionAutoTouchDownDistance > 0.0 && dist_from_finger_tip < 0.0f;
			target->SimpleCall<bool>(mMethodNameID, ev);
			manager->ManageCaptureObject(ev, target);
		}
		current.state = current.state & 1;
	}


	if (isHover)
	{
		if (current.state & 1) // was hover before
		{
			// send hover update
			ev.state = StateChanged;
			ev.touch_state = DirectTouchEvent::TouchHover;
			target->SimpleCall<bool>(mMethodNameID, ev);
			manager->ManageCaptureObject(ev, target);
		}
		else
		{
			ev.state = StateBegan;
			ev.touch_state = DirectTouchEvent::TouchHover;
			target->SimpleCall<bool>(mMethodNameID, ev);
			current.state |= 1;
			manager->ManageCaptureObject(ev, target);
			if (is_near_interaction) near_active_items.insert(target);
		}
	}
	else // not hover
	{
		if ((current.state & 1) != 0) // hover before ?
		{
			// send hover end
			ev.state = StateEnded;
			ev.touch_state = DirectTouchEvent::TouchHover;
			target->SimpleCall<bool>(mMethodNameID, ev);
			current.state -= 1;
			manager->ManageCaptureObject(ev, target);
			if (is_near_interaction) near_active_items.erase(target);
		}
	}
	
	if (touch.touch_ended)
	{
		auto it = mCurrentInfosMap.find(touch.ID);
		if(it != mCurrentInfosMap.end())
			mCurrentInfosMap.erase(it);
	}
}

void TouchEventStateDirectAccess::Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallowMask)
{
	bool swallow = (swallowMask & (1 << mType)) != 0 && (mFlag & IgnoreSwallow) == 0;
	swallow = swallow || !manager->AllowEventOn(target);

	DirectAccessEvent ev;
	ev.is_swallowed = swallow;
	ev.touch_id = touch.ID;
	ev.type = mType;
	ev.swallow_mask = &swallowMask;
	ev.origin = touch.posInfos.origin;
	ev.direction = touch.posInfos.dir;
	ev.position = touch.posInfos.pos;
	ev.touch_state = touch.touch_state;
	ev.in_touch_support = touch.in_touch_support;
	ev.has_position = touch.has_position;
	ev.interaction = touch.interaction;
	ev.item = target;

	if(!touch.touch_ended)
	{
		auto foundPrevious = mCurrentTouches.find(touch.ID);
		if (foundPrevious != mCurrentTouches.end())
		{
			ev.state = StateChanged;
		}
		else
		{
			ev.state = StateBegan;
			mCurrentTouches.insert(touch.ID);
		}
		target->SimpleCall<bool>(mMethodNameID, ev);
		manager->ManageCaptureObject(ev, target);
	}
	else
	{
		ev.state = StateEnded;
		target->SimpleCall<bool>(mMethodNameID, ev);
		manager->ManageCaptureObject(ev, target);
		mCurrentTouches.erase(touch.ID);
	}
}

void TouchEventStateSwipe::Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallowMask)
{
	bool swallow = (swallowMask & (1 << mType)) != 0 && (mFlag & IgnoreSwallow) == 0;
	swallow = swallow || !manager->AllowEventOn(target);

	SwipeEvent ev;
	ev.type = mType;
	ev.swallow_mask = &swallowMask;
	ev.has_position = touch.has_position;
	ev.touch_id = touch.ID;
	ev.interaction = touch.interaction;
	ev.item = target;

	bool is_down = false;
	if (!swallow)
	{
		ev.position = touch.posInfos.pos;
		if (touch.touch_state != 0) // a touch is there
		{
			is_down = true;
			// check if it was there before
			auto foundTouch = mCurrentInfosMap.find(touch.ID);
			if (foundTouch == mCurrentInfosMap.end()) // new touch
			{
				CurrentInfos	toStart;
				TimedTouch	addStart;
				addStart.pos = touch.posInfos.pos;
				addStart.time = timer.GetTime();

				toStart.touchList.push_back(addStart);

				// call target to check if swipe could start here 
				ev.direction = { 0,0,0 };
				ev.state = StatePossible;
				toStart.isValid = target->SimpleCall<bool>(mMethodNameID, ev);

				mCurrentInfosMap[touch.ID] = toStart;
			}
			else // existing touch
			{
				CurrentInfos& cswipe = (*foundTouch).second;
				if (cswipe.isValid)
				{
					TimedTouch	addt;
					addt.pos = touch.posInfos.pos;
					addt.time = timer.GetTime();

					if (cswipe.touchList.size() < 3)
					{
						cswipe.touchList.push_back(addt);
					}
					else
					{
						// check duration
						if ((addt.time - cswipe.touchList[0].time) > mSwipeMaxDuration)
						{
							cswipe.isValid = false;
						}
						else if (DistSquare(addt.pos, cswipe.touchList[0].pos) < manager->getTriggerSquaredDist())
						{
							cswipe.isValid = false;
						}
						else
						{
							// ask for possible start
							if (cswipe.touchList.size() == 3)
							{
								Vector3D swipemaindir(cswipe.touchList[0].pos, addt.pos,asVector());
								ev.direction = swipemaindir;
								ev.state = StateBegan;
								cswipe.isValid = target->SimpleCall<bool>(mMethodNameID, ev);
							}

							cswipe.touchList.push_back(addt);
						}
					}
				}
			}
		}
		
	}
	// now check mCurrentInfosMap for finished swipes

	// check if a previous started click was ended
	if (!is_down)
	{
		auto it = mCurrentInfosMap.find(touch.ID);
		if (it != mCurrentInfosMap.end())
		{
			CurrentInfos& cswipe = it->second;

			if (cswipe.isValid && !swallow)
			{
				// check if swipe if is ok 

				double duration = timer.GetTime() - cswipe.touchList[0].time;
				if ((duration > mSwipeMinDuration) && (duration < mSwipeMaxDuration))
				{
					// call target to check if swipe end is "accepted"
					Vector3D swipemaindir(cswipe.touchList[0].pos, cswipe.touchList.back().pos, asVector());

					ev.position = cswipe.touchList.back().pos;					
					ev.direction = swipemaindir;
					ev.state = StateChanged;
					if (target->SimpleCall<bool>(mMethodNameID, ev))
					{
						// check swipe "trajectory"
						Vector3D nswipeDir(swipemaindir);
						nswipeDir.Normalize();
						for (u32 i = 3; i < cswipe.touchList.size(); i++)
						{
							Vector3D checkdir(cswipe.touchList[0].pos, cswipe.touchList[i].pos, asVector());
							checkdir.Normalize();

							float dot = Dot(nswipeDir, checkdir);
							if (dot < 0.6f)
							{
								cswipe.isValid = false;
								break;
							}
						}
						if (cswipe.isValid)
						{
							ev.direction = swipemaindir;
							ev.state = StateRecognized;
							target->SimpleCall<bool>(mMethodNameID, ev);
						}
					}
				}
			}


			mCurrentInfosMap.erase(it);
		}
		
	}

}

void TouchEventStateScroll::Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallowMask)
{
	bool swallow = (swallowMask & (1<<mType)) != 0 && (mFlag & IgnoreSwallow) == 0;
	swallow = swallow || !manager->AllowEventOn(target);

	ScrollEvent ev;
	ev.type = mType;
	ev.swallow_mask = &swallowMask;
	ev.touch_id = touch.ID;
	ev.interaction = touch.interaction;
	ev.hit = *touch.object_hit;
	ev.item = target;
	ev.has_position = touch.has_position;
	ev.origin = touch.posInfos.origin;
	ev.direction = touch.posInfos.dir;
	ev.position = touch.posInfos.pos;
	ev.delta = v3f(0, 0, 0);

	auto right = (ev.direction ^ v3f(0, 1, 0)).Normalized();
	auto up = (right ^ ev.direction).Normalized();
	auto project_on_view = [&](v3f position)
	{
		auto x = ProjectOnLineScalar(position, ev.origin, ev.origin + right);
		auto y = ProjectOnLineScalar(position, ev.origin, ev.origin + up);
		return v3f(x, -y, 0) * 8192*2;
	};
	auto position = touch.interaction ? project_on_view(touch.interaction->Position) : ev.position;

	bool is_down = false;
	if (!swallow)
	{
		if (touch.touch_state != 0) // a touch is there
		{
			is_down = true;
			// check if it was there before
			auto foundTouch = mCurrentInfosMap.find(touch.ID);
			if (foundTouch == mCurrentInfosMap.end()) // new touch
			{
				CurrentInfos	toStart;
				toStart.startpos = position;
				toStart.currentpos = position;
				toStart.starttime = timer.GetTime();
				toStart.currenttime = toStart.starttime;
				toStart.maindir.Set(0, 0, 0);
				toStart.currentSpeed = toStart.maindir;
				// call target to check if scroll could start here 

				// send direction, start position, current speed vector, offset
				ev.state = StatePossible;
				ev.main_direction = toStart.maindir;
				ev.start_position = toStart.startpos;
				ev.offset = 0.0f;
				ev.speed = { 0,0,0 };

				toStart.isValid = target->SimpleCall<bool>(mMethodNameID, ev);

				mCurrentInfosMap[touch.ID] = toStart;
			}
			else // existing touch
			{
				CurrentInfos& cscroll = (*foundTouch).second;
				if (cscroll.isValid)
				{
					if (NormSquare(cscroll.maindir) < 0.01f) // scroll has not began yet ?
					{
						cscroll.currentpos = position;
						cscroll.currenttime = timer.GetTime();
						if (DistSquare(cscroll.startpos, position) > manager->getTriggerSquaredDist())
						{
							//kfloat duration = timer.GetTime() - cscroll.starttime;
							//if (duration > 0.25f)
							{
								cscroll.maindir.Set(cscroll.startpos, position);
								cscroll.maindir.Normalize();

								// if mScrollForceMainDir is set, check if it's OK
								if (NormSquare(mScrollForceMainDir) > 0.01f)
								{
									kfloat dot = Dot(mScrollForceMainDir, cscroll.maindir);
									if (fabsf(dot) < 0.6)
									{
										cscroll.isValid = false;
									}
									else
									{
										cscroll.maindir = mScrollForceMainDir;
									}
								}

								if (cscroll.isValid)
								{
									// reset cscroll
									cscroll.startpos = cscroll.currentpos;
									cscroll.starttime = timer.GetTime();
									cscroll.currentSpeed.Set(0.0f, 0.0f, 0.0f);
									// send direction, start position, current speed vector, offset

									ev.state = StateBegan;
									ev.main_direction = cscroll.maindir;
									ev.start_position = cscroll.startpos;
									ev.offset = 0.0f;
									ev.speed = cscroll.currentSpeed;

									cscroll.isValid = target->SimpleCall<bool>(mMethodNameID, ev);
									manager->ManageCaptureObject(ev, target);
								}
							}
						}
					}
					else // scroll has began, update offset / speed  
					{
						Vector3D move(cscroll.currentpos, position, asVector());
						float dt = float(timer.GetTime() - cscroll.currenttime);

						cscroll.currentpos = position;
						cscroll.currenttime = timer.GetTime();

						Vector3D speed(move / dt);

						kfloat coefInterp = 0.5f + (( dt > 0.5f) ? 0.5f : dt);

						cscroll.currentSpeed = cscroll.currentSpeed*(1.0f - coefInterp) + speed*coefInterp;

						Vector3D	offsetV(cscroll.startpos, cscroll.currentpos, asVector());
						kfloat offset = Dot(cscroll.maindir, offsetV);

						ev.state = StateChanged;
						ev.main_direction = cscroll.maindir;
						ev.start_position = cscroll.startpos;
						ev.speed = cscroll.currentSpeed;
						ev.offset = offset;
						ev.delta = cscroll.currentpos - cscroll.startpos;

						// send direction, start position, current speed vector, offset
						target->SimpleCall<bool>(mMethodNameID, ev);
						manager->ManageCaptureObject(ev, target);

					}

				}
			}
		}
	}

	
	if (!is_down)
	{
		auto it = mCurrentInfosMap.find(touch.ID);
		if (it != mCurrentInfosMap.end())
		{
			CurrentInfos& cscroll = it->second;

			if ((cscroll.isValid) && (NormSquare(cscroll.maindir) > 0.01f))
			{
				Vector3D	offsetV(cscroll.startpos, cscroll.currentpos, asVector());
				kfloat offset = Dot(cscroll.maindir, offsetV);
				// send direction, start position, current speed vector, offset

				ev.state = StateEnded;
				ev.main_direction = cscroll.maindir;
				ev.start_position = cscroll.startpos;
				ev.speed = cscroll.currentSpeed;
				ev.offset = offset;
				ev.delta = cscroll.currentpos - cscroll.startpos;

				target->SimpleCall<bool>(mMethodNameID, ev);
				manager->ManageCaptureObject(ev, target);
			}
			mCurrentInfosMap.erase(it);
		}
	}
}

void TouchEventStatePinch::Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallowMask)
{
	bool swallow = (swallowMask & (1 << mType)) != 0 && (mFlag & IgnoreSwallow) == 0;
	swallow = swallow || !manager->AllowEventOn(target);

	PinchEvent ev;
	ev.swallow_mask = &swallowMask;
	ev.type = mType;
	ev.interaction = touch.interaction;
	ev.item = target;

	bool new_touch = false;
	bool need_erase = false;
	auto it = mCurrentTouches.find(touch.ID);

	
	
	bool is_down = (!swallow && touch.touch_state != 0 && !touch.touch_ended);
	if(is_down)
	{
		new_touch = it == mCurrentTouches.end();
		if (new_touch)
		{
			auto& t = mCurrentTouches[touch.ID];
			t.position = touch.posInfos.pos;
		}
		else
		{
			it->second.position = touch.posInfos.pos;
		}
	}
	else if (it != mCurrentTouches.end())
	{
		need_erase = true;
	}

	if (need_erase && it->second.in_use_by_pinch)
	{
		auto itpinch = std::find_if(mCurrentPinches.begin(), mCurrentPinches.end(), [&](auto&& pinch) { return pinch.p1_ID == touch.ID || pinch.p2_ID == touch.ID; });
		if (itpinch != mCurrentPinches.end())
		{
			ev.touch_id = itpinch->p1_ID;
			ev.touch_id_2 = itpinch->p2_ID;
			ev.p1_start = itpinch->p1_start_pos;
			ev.p2_start = itpinch->p2_start_pos;
			ev.p1 = mCurrentTouches[itpinch->p1_ID].position;
			ev.p2 = mCurrentTouches[itpinch->p2_ID].position;
				
			ev.state = StateEnded;
			target->SimpleCall<bool>(mMethodNameID, ev);

			mCurrentTouches[itpinch->p1_ID].in_use_by_pinch = false;
			mCurrentTouches[itpinch->p2_ID].in_use_by_pinch = false;

			mCurrentPinches.erase(itpinch);
		}
	}
	else if(new_touch)
	{
		kstl::vector<std::pair<TouchSourceID, float>> possibles_pinches;
		for (auto& t : mCurrentTouches)
		{
			if (t.second.in_use_by_pinch || t.first == touch.ID) continue;
			float dist = NormSquare(t.second.position - touch.posInfos.pos);
			if (dist <= mPinchMaxStartDistSquared)
			{
				possibles_pinches.push_back({ t.first, dist});
			}
		}
		std::sort(possibles_pinches.begin(), possibles_pinches.end(), [](auto&& a, auto&& b) { return a.second < b.second; });

		for (auto& pair : possibles_pinches)
		{
			ev.state = StatePossible;

			auto a = std::make_tuple(pair.first, &mCurrentTouches[pair.first]);
			auto b = std::make_tuple(touch.ID, &mCurrentTouches[touch.ID]);
			auto mm = std::minmax(a, b);

			ev.p1_start = ev.p1 = std::get<1>(mm.first)->position;
			ev.p2_start = ev.p2 = std::get<1>(mm.second)->position;
			ev.touch_id = std::get<0>(mm.first);
			ev.touch_id_2 = std::get<0>(mm.second);

			if (target->SimpleCall<bool>(mMethodNameID, ev))
			{
				ev.state = StateBegan;
				target->SimpleCall<bool>(mMethodNameID, ev);

				CurrentPinch pinch;
				pinch.p1_ID = std::get<0>(mm.first);
				pinch.p2_ID = std::get<0>(mm.second);
				pinch.p1_start_pos = std::get<1>(mm.first)->position;
				pinch.p2_start_pos = std::get<1>(mm.second)->position;
					
				mCurrentTouches[pinch.p1_ID].in_use_by_pinch = true;
				mCurrentTouches[pinch.p2_ID].in_use_by_pinch = true;

				mCurrentPinches.push_back(pinch);
				break;
			}
		}
	}
	if (is_down && !new_touch)
	{
		auto itpinch = std::find_if(mCurrentPinches.begin(), mCurrentPinches.end(), [&](auto&& pinch) { return pinch.p1_ID == touch.ID || pinch.p2_ID == touch.ID; });
		if (itpinch != mCurrentPinches.end())
		{
			ev.touch_id = itpinch->p1_ID;
			ev.touch_id_2 = itpinch->p2_ID;
			ev.p1_start = itpinch->p1_start_pos;
			ev.p2_start = itpinch->p2_start_pos;
			ev.p1 = mCurrentTouches[itpinch->p1_ID].position;
			ev.p2 = mCurrentTouches[itpinch->p2_ID].position;
			ev.state = StateChanged;
			target->SimpleCall<bool>(mMethodNameID, ev);
		}
	}

	if (need_erase)
	{
		mCurrentTouches.erase(it);
	}
}

void TouchEventStateInputSwallow::Update(TouchInputEventManager* manager, const Timer& timer, CoreModifiable* target, const TouchInfos& touch, u32& swallowMask)
{
	bool swallow = (swallowMask & (1 << mType)) != 0 && (mFlag & IgnoreSwallow) == 0;
	if (swallow) return;

	InputEvent ev;
	ev.type = mType;
	ev.has_position = touch.has_position;
	ev.state = StatePossible;
	ev.swallow_mask = &swallowMask;
	ev.position = touch.posInfos.pos;
	ev.touch_id = touch.ID;
	ev.interaction = touch.interaction;
	ev.item = target;

	target->SimpleCall<bool>(mMethodNameID, ev);
	
}