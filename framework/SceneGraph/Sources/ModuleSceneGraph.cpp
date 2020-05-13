#include "PrecompiledHeaders.h"

#include "ModuleSceneGraph.h"
#include "SceneGraphIncludes.h"
#include "ModuleRenderer.h"
#include "Timer.h"
#include "CoreDecorator.h"
#include "Core.h"
#include "TravState.h"
#include "Node3DLodDecorator.h"

IMPLEMENT_CLASS_INFO(ModuleSceneGraph);

IMPLEMENT_CONSTRUCTOR(ModuleSceneGraph)
, myTravState(nullptr)
, mySceneListNeedsSort(*this, false, "SceneListNeedsSort", false)
{
}

ModuleSceneGraph::~ModuleSceneGraph()
{

}

void ModuleSceneGraph::AddDefferedItem(void* item, DefferedAction::ENUM action)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	sDefferedAction[item] = action;
}

void ModuleSceneGraph::NotifyDefferedItemDeath(CoreModifiable* item)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	auto found = sDefferedAction.find(item);
	if (found != sDefferedAction.end())
		sDefferedAction.erase(found);

	RemoveFromParentScene(item);
}

void ModuleSceneGraph::SignalLightChange(CoreModifiable* l)
{
	Scene3D* parent = nullptr;

	if (FindParentScene(l, &parent))
	{
		parent->lightHasChanged();
	}
}

bool ModuleSceneGraph::FindParentScene(CoreModifiable* item, Scene3D** parent, bool checkSceneNode)
{
	if (checkSceneNode)
	{
		// check if we can find a scene with this camera
		auto itScene = myScenes.begin();
		auto itSceneend = myScenes.end();

		if (item->isSubType("Camera"))
		{
			while (itScene != itSceneend)
			{
				if ((*itScene)->hasCamera(item))
				{
					*parent = (*itScene);
					return true;
				}
				++itScene;
			}
		}
		else if (item->isSubType("API3DLight"))
		{
			while (itScene != itSceneend)
			{
				if ((*itScene)->hasLight(item))
				{
					*parent = (*itScene);
					return true;
				}
				++itScene;
			}
		}
	}

	CoreModifiable* lParent = item;
	while (lParent)
	{
		if (lParent->isSubType(Scene3D::myClassID))
			break;

		auto& parents = lParent->GetParents();

		lParent = nullptr;		
		for(auto parent : parents)
		{
			if (parent->isSubType("Node3D"))
			{
				lParent = parent;
				break;
			}
		}
	}

	*parent = static_cast<Scene3D*>(lParent);
	return lParent != nullptr;
}

bool ModuleSceneGraph::RemoveFromParentScene(CoreModifiable* item)
{
	if (myScenes.empty())
		return false;

	// check if we can find a scene with this camera
	auto itScene = myScenes.begin();
	auto itSceneend = myScenes.end();

	if (item->isSubType("Camera"))
	{
		while (itScene != itSceneend)
		{
			if ((*itScene)->removeCamera(item))
			{
				return true;
			}
			++itScene;
		}
	}
	else if (item->isSubType("API3DLight"))
	{
		while (itScene != itSceneend)
		{
			if ((*itScene)->removeLight(item))
			{
				return true;
			}
			++itScene;
		}
	}

	return false;
}

void ModuleSceneGraph::DoDefferedAction()
{
	
	std::lock_guard<std::recursive_mutex> lk{ mMutex };

	if (sDefferedAction.empty())
		return;

	Scene3D* parent = nullptr;

	kstl::vector<void*> toErase;

	auto itr = sDefferedAction.begin();
	auto end = sDefferedAction.end();
	for (; itr != end; ++itr)
	{
		switch (itr->second)
		{
		case DefferedAction::ADD_CAMERA:
		case DefferedAction::ADD_LIGHT:
		case DefferedAction::REMOVE_CAMERA:
		case DefferedAction::REMOVE_LIGHT:
			if (!FindParentScene((CoreModifiable*)itr->first, &parent))
				continue;
			break;
		}
		switch (itr->second)
		{
		case DefferedAction::ADD_CAMERA:
			parent->addCamera((CoreModifiable*)itr->first);
			break;
		case DefferedAction::ADD_LIGHT:
			parent->addLight((CoreModifiable*)itr->first);
			break;
		case DefferedAction::REMOVE_CAMERA:
			parent->removeCamera((CoreModifiable*)itr->first);
			break;
		case DefferedAction::REMOVE_LIGHT:
			parent->removeLight((CoreModifiable*)itr->first);
			break;
		case DefferedAction::DESTROY_SHADER:
			// TODO
			break;
		case DefferedAction::DESTROY_PROGRAM:
			// TODO
			break;
		case DefferedAction::DESTROY_TEXTURE:
		{
			unsigned int textureID = (unsigned int)(itr->first);
			myRenderer->DeleteTexture(1, &textureID);
			break;
		}
		case DefferedAction::DESTROY_BUFFER:
		{	
			unsigned int buffer = (unsigned int)(itr->first);
			myRenderer->DeleteBuffer(1, &buffer);
			break;
		}
		default:
			break;
		}

		toErase.push_back(itr->first);
	}

	for (auto it : toErase)
	{
		sDefferedAction.erase(sDefferedAction.find(it));
	}
}




void ModuleSceneGraph::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core, "SceneGraph", params);

	core->RegisterMainModuleList(this, SceneGraphModuleCoreIndex);

	//AddToAutoRegister({}, "SceneGraph");
	DECLARE_FULL_CLASS_INFO(core, AutoOrientedNode3D, AutoOrientedNode3D, SceneGraph);
	DECLARE_FULL_CLASS_INFO(core, CoordinateSystem, CoordinateSystem, SceneGraph);
	DECLARE_FULL_CLASS_INFO(core, CullingObject, CullingObject, SceneGraph);
	DECLARE_FULL_CLASS_INFO(core, Node3D, Node3D, SceneGraph);
	DECLARE_FULL_CLASS_INFO(core, Node3DDelayed, Node3DDelayed, SceneGraph);
	DECLARE_FULL_CLASS_INFO(core, Octree, Octree, SceneGraph);
	DECLARE_FULL_CLASS_INFO(core, OctreeSubNode, OctreeSubNode, SceneGraph);
	DECLARE_FULL_CLASS_INFO(core, Scene3D, Scene3D, SceneGraph);
	DECLARE_FULL_CLASS_INFO(core, TravState, TravState, SceneGraph);
	DECLARE_FULL_CLASS_INFO(core, TouchInputEventManager, TouchInputEventManager, Input);
	DECLARE_FULL_CLASS_INFO(core, TouchInputEventManager, TouchInputEventManager, Input);
	DECLARE_FULL_CLASS_INFO(core, TouchInputEventManager, TouchInputEventManager, Input);
	DECLARE_FULL_CLASS_INFO(core, TouchInputEventManager, TouchInputEventManager, Input);


	DECLARE_FULL_DECORATOR_INFO(KigsCore::Instance(), Node3DLodDecorator);

	// search for the renderer module

	kstl::vector<CMSP>	instances=	CoreModifiable::GetInstances("ModuleSpecificRenderer");

	myTravState = KigsCore::GetInstanceOf("SceneTravState", "TravState");

	if (instances.size())
		myRenderer = (ModuleSpecificRenderer*)(instances[0]).get();

	myTravState->SetRenderer(myRenderer);
	myTravState->SetHolographicMode(false);
	myTravState->SetSceneGraph(this);
}

void ModuleSceneGraph::Close()
{
	DoDefferedAction();
	BaseClose();
}


void ModuleSceneGraph::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);

	if (mySceneListNeedsSort)
	{
		SortSceneList();
	}

	DoDefferedAction();

	myTravState->BeginNewFrame();
	myTravState->GetRenderer()->startFrame(myTravState.get());

	// sort node by renderer
	
	for(auto scene : myScenes)
	{
		if (scene)
		{
			scene->CallUpdate(timer,addParam);
			//debugPrintfTree(20);
			myTravState->SetScene(scene);
			myTravState->SetTime(timer.GetTime());
			scene->TravCull(myTravState.get());
			scene->TravDraw(myTravState.get());
		}
	}
	myTravState->GetRenderer()->endFrame(myTravState.get());
}


bool	ModuleSceneGraph::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	// only Scene3D can be added here

	if (item->isSubType("Scene3D"))
	{
		myScenes.insert((Scene3D*)item.get());
		return ParentClassType::addItem(item,pos PASS_LINK_NAME(linkName));
	}

	return false;
}

bool ModuleSceneGraph::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if (item->isSubType("Scene3D"))
	{
		for (auto it = myScenes.begin(); it != myScenes.end(); ++it)
		{
			if ((*it) == ((Scene3D*)item.get()))
			{
				myScenes.erase(it);
				break;
			}
		}
	}
	return ParentClassType::removeItem(item PASS_LINK_NAME(linkName));
}

void	ModuleSceneGraph::SortSceneList()
{
	mySceneListNeedsSort = false;

	// copy scenes in a tmp vector
	kstl::vector<Scene3D*>	tmpList;
	kstl::set<Scene3D*, Scene3DPriorityCompare>::iterator it;

	for (it = myScenes.begin(); it != myScenes.end(); ++it)
	{
		tmpList.push_back(*it);
	}

	// clear sorted set
	myScenes.clear();

	// then add scenes to set again
	kstl::vector<Scene3D*>::iterator	itvector;
	for (itvector = tmpList.begin(); itvector != tmpList.end(); ++itvector)
	{
		myScenes.insert(*itvector);
	}
}

