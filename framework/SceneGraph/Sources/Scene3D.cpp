#include "PrecompiledHeaders.h"
#include "Scene3D.h"
#include "Camera.h"
#include "TravState.h"
#include "DrawableSorter_Camera.h"
#include "TimeProfiler.h"
#include "Material.h" 
#include "ModuleSceneGraph.h"
#include "CoreBaseApplication.h"

#include "TouchInputEventManager.h"

#include "TecLibs/Math/IntersectionAlgorithms.h"

#include <algorithm>

//IMPLEMENT_AND_REGISTER_CLASS_INFO(Scene3D, Scene3D, SceneGraph);
IMPLEMENT_CLASS_INFO(Scene3D)

bool sortCamera::operator() (const Camera* lhs, const Camera* rhs) const{
	if (lhs->getPriority() == rhs->getPriority())
		return lhs < rhs;

	return lhs->getPriority() < rhs->getPriority();
}

IMPLEMENT_CONSTRUCTOR(Scene3D)
	, myLightsHaveChanged(true)
	, myPriority(*this, false, "Priority", 0)
	, mySort3DNodesFrontToBack(*this, false, "Sort3DNodesFrontToBack", false)
{
}

void	Scene3D::InitModifiable()
{
	Node3D::InitModifiable();
	if (IsInit())
	{
		// I want to be notified when Priority is changed
		myPriority.changeNotificationLevel(Owner);
		myLightsHaveChanged = true;
	}
}

void	Scene3D::UninitModifiable()
{
	if (IsInit())
	{
		// Notify scenegraph  that I am dead
		ModuleSceneGraph* scenegraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);
		scenegraph->removeItem(this);
		Node3D::UninitModifiable();
	}
}


Scene3D::~Scene3D()
{
	// release ref on light
	auto itr= myLights.begin();
	auto end= myLights.end();
	for (; itr != end; itr++)
	{
		(*itr)->Destroy();
	}
}

void Scene3D::NotifyUpdate(const unsigned int  labelid)
{
	if (labelid == myPriority.getLabelID())
	{
		// search for module scenegraph in parents
		kstl::vector<CoreModifiable*>::const_iterator itparents = GetParents().begin();
		kstl::vector<CoreModifiable*>::const_iterator itparentsend = GetParents().end();
		for (; itparents != itparentsend; ++itparents)
		{
			if ((*itparents)->isSubType(ModuleSceneGraph::myClassID))
			{
				ModuleSceneGraph* tonotify = (ModuleSceneGraph*)(*itparents);
				tonotify->setValue("SceneListNeedsSort", true);
				// only one modulescenegraph
				break;
			}
		}
	}
}

// Camera managment
///////////////////
void  Scene3D::addCamera(CoreModifiable* camera)
{
	myCameras.insert(static_cast<Camera*>(camera));
}

bool  Scene3D::hasCamera(CoreModifiable* camera)
{
	return (myCameras.find(static_cast<Camera*>(camera)) != myCameras.end());
}

bool  Scene3D::removeCamera(CoreModifiable* camera)
{
	kstl::set<Camera*>::iterator found = myCameras.find(static_cast<Camera*>(camera));
	if (found != myCameras.end())
	{
		myCameras.erase(found);
		return true;
	}
	return false;
}


// Light managment
//////////////////
void  Scene3D::addLight(CoreModifiable* light)
{
	myLights.insert(light);
	light->GetRef();
	myLightsHaveChanged = true;
}

bool  Scene3D::hasLight(CoreModifiable* light)
{
	return (myLights.find(light) != myLights.end());
}

bool  Scene3D::removeLight(CoreModifiable* light)
{
	auto found = myLights.find(light);
	if (found != myLights.end())
	{
		(*found)->Destroy();
		myLights.erase(found);
		return true;
		myLightsHaveChanged = true;
	}
	return false;
}


void ManageFrontToBackStruct::Init(Camera* cam)
{
	cam->GetPosition(camPos.x, camPos.y, camPos.z);
	cam->GetViewVector(camViewVector.x, camViewVector.y, camViewVector.z);
	camViewVector.Normalize();

	if (camViewVector.x > 0)
	{
		BBoxPointToTestIndexes[0] = 0;
	}
	else
	{
		BBoxPointToTestIndexes[0] = 3;
	}

	if (camViewVector.y > 0)
	{
		BBoxPointToTestIndexes[1] = 1;
	}
	else
	{
		BBoxPointToTestIndexes[1] = 4;
	}

	if (camViewVector.z > 0)
	{
		BBoxPointToTestIndexes[2] = 2;
	}
	else
	{
		BBoxPointToTestIndexes[2] = 5;
	}
}

void Scene3D::TravDraw(TravState* state)
{
	if (!IsRenderable()) return;
	
	state->SetHolographicMode(KigsCore::GetCoreApplication()->IsHolographic());

	if (myLightsHaveChanged)
	{
		ModuleSpecificRenderer::LightCount count = state->GetRenderer()->SetLightsInfo(&myLights);
		myDirLightCount = count.dir;
		mySpotLightCount = count.spot;
		myPointLightCount = count.point;
		myLightsHaveChanged = false;
	}
	state->myLights = &myLights;

	// add sort struct to travstate
	if (mySort3DNodesFrontToBack)
	{
		state->pManageFrontToBackStruct = new ManageFrontToBackStruct();
		ComputeNodePriority();
	}

	for (auto cam : myCameras)
	{
		state->GetSceneGraph()->TreatNeedUpdateList();
		if (!cam->IsEnabled()) continue;
		if (!cam->SetActive(state)) continue;
			
		auto& passes = cam->GetRenderPasses();
		if (mySort3DNodesFrontToBack)
		{
			state->pManageFrontToBackStruct->Init(cam);
		}
		state->myVisibilityFrame++;
		state->SetCurrentCamera(cam);
		state->SetAllVisible(cam->AllVisible());
		cam->InitCullingObject(state->GetCullingObject());
		
		// reset visibility settings
		state->GetSceneGraph()->ResetVisibleNodeList();
		if (Node3D::Cull(state, 0))
		{
			Node3D::TravCull(state);
		}

		for (auto& pass : passes)
		{
#ifdef KIGS_TOOLS
			pass.debug_draw_path.clear();
#endif
			DrawableSorter_Camera lDrawableSorter;

			state->SetCurrentLocalToGlobalMatrix(Matrix3x4::IdentityMatrix());
			state->SetCurrentGlobalToLocalMatrix(Matrix3x4::IdentityMatrix());
			state->myCurrentMaterial = 0;
			
			state->myPath = 0;
			lDrawableSorter.SetCamera(cam);
			lDrawableSorter.Clear();

			if(pass.use_front_to_back_sorter)
				pass.sorter = &lDrawableSorter;

			state->mCurrentPass = &pass;

			if (state->mCurrentPass->clear_depth)
				state->GetRenderer()->ClearView(RendererClearMode::RENDERER_CLEAR_DEPTH_AND_STENCIL);
			state->GetRenderer()->SetDepthMaskMode(pass.write_depth ? RendererDepthMaskMode::RENDERER_DEPTH_MASK_ON : RendererDepthMaskMode::RENDERER_DEPTH_MASK_OFF);

			Node3D::TravDraw(state);
			state->DrawInstances();

			TravPath path;
			lDrawableSorter.ConstructPath(path);
			state->myPath = &path;
			if (path.GetSize())
			{
				if (state->myCurrentMaterial)
				{
					Material* toPostDraw = state->myCurrentMaterial;
					state->myCurrentMaterial = 0;
					toPostDraw->CheckPostDraw(state);
				}

				Node3D::TravDraw(state);
				state->DrawInstances();
			}

			// TODO
			// change multipass support for shared material
			if (state->myCurrentMaterial)
			{
				Material*	toPostDraw = state->myCurrentMaterial;
				state->myCurrentMaterial = 0;
				toPostDraw->CheckPostDraw(state);
			}
#ifdef KIGS_TOOLS
			pass.record_pass = false;
#endif
		}
		state->mCurrentPass = nullptr;
		cam->Release(state);
	}

	if (mySort3DNodesFrontToBack)
	{
		delete state->pManageFrontToBackStruct;
		state->pManageFrontToBackStruct = 0;
	}

	// remove lights
	state->myLights = nullptr;
	// no more camera after scene 3D 
	state->SetCurrentCamera(nullptr);
}


void     Scene3D::TravCull(TravState* /* state */)
{

	// Do nothing here, we can not separate cull and draw for the scene as we have to do it once per camera

}


void Scene3D::SortItemsFrontToBack(SortItemsFrontToBackParam& param)
{
	unsigned int mask = 0;
	auto collision = GetFirstInstance("CollisionManager");
	if (!collision || !(param.camera && param.camera->getValue("SortForInputManager", mask))) 
	{
		std::transform(param.toSort.begin(), param.toSort.end(), std::back_inserter(param.sorted), [](CoreModifiable* cm)
		{
			return std::make_tuple(cm, Hit{});
		});
		return;
	}
	
	
	Camera* cam = (Camera*)param.camera;
	v3f cam_pos = cam->GetGlobalPosition();
	v3f cam_view = cam->GetGlobalViewVector();

	kstl::vector<Hit> hits;
	collision->SimpleCall("GetAllRayIntersection", param.origin, param.direction, hits, mask);
	
	struct Sorter
	{
		CoreModifiable* cm;
		float dist;
		Hit* hit;
	};

	kstl::vector<Sorter> sorter(param.toSort.size(), Sorter{nullptr, FLT_MAX, nullptr });
	for (auto&& hit : hits)
	{
		auto n = hit.HitActor;
		while (n)
		{
			auto found = std::find(param.toSort.begin(), param.toSort.end(), n);
			if (found != param.toSort.end())
			{
				auto i = std::distance(param.toSort.begin(), found);
				if (sorter[i].dist > hit.HitDistance)
				{
					sorter[i].dist = hit.HitDistance;
					sorter[i].hit = &hit;
				}
			}
			n = n->getFirstParent("SceneNode");
		}
	}
	
	for (u32 i = 0; i < param.toSort.size(); ++i)
	{
		auto cm = param.toSort[i];
		sorter[i].cm = cm;

		if (param.toSort[i]->HasMethod("GetDistanceForInputSort"))
		{
			GetDistanceForInputSortParam params;
			params.camera = param.camera;
			params.position = param.position;
			params.origin = param.origin;
			params.direction = param.direction;
			params.hits = &hits;
			params.inout_distance = sorter[i].dist;
			params.inout_hit = sorter[i].hit;
			param.toSort[i]->SimpleCall("GetDistanceForInputSort", params);
			sorter[i].dist = params.inout_distance;
			sorter[i].hit = params.inout_hit;
		}
		else
		{
			if (sorter[i].dist == FLT_MAX && param.toSort[i]->isSubType("Node3D"))
			{
				auto bbox_local = param.toSort[i]->as<Node3D>()->GetLocalBoundingBox();

				auto origin_local = param.origin;
				Vector3D direction_local = param.direction;
				
				auto g2l = param.toSort[i]->as<Node3D>()->GetGlobalToLocal();

				g2l.TransformPoint(&origin_local);
				g2l.TransformVector(&direction_local);

				double distance = DBL_MAX;
				v3f intersection;
				
				v3f dir;

				if (Intersection::IntersectionRayBBox(origin_local, direction_local, bbox_local.m_Min, bbox_local.m_Max, intersection, distance))
				{
					dir = param.toSort[i]->as<Node3D>()->GetLocalToGlobal()*intersection - cam_pos;
				}
				else
				{
					dir = param.toSort[i]->as<Node3D>()->GetLocalToGlobal()*bbox_local.Center() - cam_pos;
				}

				auto d = NormSquare(dir);
				if (Dot(dir, cam_view) < 0)
					d = -d;

				sorter[i].dist = d;
			}
		}
	}

	std::sort(sorter.begin(), sorter.end(), [](auto&& a, auto&& b) { return a.dist < b.dist; });

	param.sorted.resize(param.toSort.size());
	for (u32 i = 0; i < sorter.size(); ++i)
	{
		param.sorted[i] = std::make_tuple(sorter[i].cm, sorter[i].hit ? *sorter[i].hit : Hit{});
	}
}