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

using namespace Kigs::Scene;
using namespace Kigs::Draw;

//IMPLEMENT_AND_REGISTER_CLASS_INFO(Scene3D, Scene3D, SceneGraph);
IMPLEMENT_CLASS_INFO(Scene3D)

bool sortCamera::operator() (const Camera* lhs, const Camera* rhs) const{
	if (lhs->getPriority() == rhs->getPriority())
		return lhs < rhs;

	return lhs->getPriority() < rhs->getPriority();
}

IMPLEMENT_CONSTRUCTOR(Scene3D)
	, mLightsHaveChanged(true)
{
}

void	Scene3D::InitModifiable()
{
	Node3D::InitModifiable();
	if (IsInit())
	{
		// I want to be notified when Priority is changed
		setOwnerNotification("Priority", true);
		mLightsHaveChanged = true;
	}
}

void	Scene3D::UninitModifiable()
{
	if (IsInit())
	{
		Node3D::UninitModifiable();
		// Notify scenegraph  that I am dead
		ModuleSceneGraph* scenegraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);
		scenegraph->removeItem(SharedFromThis());
	}
}


Scene3D::~Scene3D()
{
}

void Scene3D::NotifyUpdate(const unsigned int  labelid)
{
	if (labelid == KigsID("Priority"))
	{
		// search for module scenegraph in parents
		std::vector<CoreModifiable*>::const_iterator itparents = GetParents().begin();
		std::vector<CoreModifiable*>::const_iterator itparentsend = GetParents().end();
		for (; itparents != itparentsend; ++itparents)
		{
			if ((*itparents)->isSubType(ModuleSceneGraph::mClassID))
			{
				ModuleSceneGraph* tonotify = (ModuleSceneGraph*)(*itparents);
				tonotify->setValue("SceneListNeedsSort", true);
				// only one modulescenegraph
				break;
			}
		}
	}
}

// NOTE(antoine) camera and light have similar code (with DEFERRED_REMOVE etc...) but lights are stored with CMSP and cameras are not

// Camera managment
///////////////////
void  Scene3D::addCamera(CoreModifiable* camera)
{
	mCameras.insert(static_cast<Camera*>(camera));
}

bool  Scene3D::hasCamera(CoreModifiable* camera)
{
	return (mCameras.find(static_cast<Camera*>(camera)) != mCameras.end());
}

bool  Scene3D::removeCamera(CoreModifiable* camera)
{
	std::set<Camera*>::iterator found = mCameras.find(static_cast<Camera*>(camera));
	if (found != mCameras.end())
	{
		mCameras.erase(found);
		return true;
	}
	return false;
}
// Light managment
//////////////////
void  Scene3D::addLight(CoreModifiable* light)
{
	if(mLights.insert(light).second)
		mLightsHaveChanged = true;
}

bool  Scene3D::hasLight(CoreModifiable* light)
{
	return (mLights.find(light) != mLights.end());
}

bool  Scene3D::removeLight(CoreModifiable* light)
{
	auto found = mLights.find(light);
	if (found != mLights.end())
	{
		mLights.erase(found);
		mLightsHaveChanged = true;
		return true;
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

	if (mLightsHaveChanged)
	{
		ModuleSpecificRenderer::LightCount count = state->GetRenderer()->SetLightsInfo(&mLights);
		mDirLightCount = count.mDir;
		mSpotLightCount = count.mSpot;
		mPointLightCount = count.mPoint;
		mLightsHaveChanged = false;
	}
	state->mLights = &mLights;

	// add sort struct to travstate
	if (mSort3DNodesFrontToBack)
	{
		state->mManageFrontToBackStruct = new ManageFrontToBackStruct();
		ComputeNodePriority();
	}

	for (auto cam : mCameras)
	{
		state->GetSceneGraph()->TreatNeedUpdateList();
		if (!cam->IsEnabled()) continue;
		if (!cam->SetActive(state)) continue;
			
		auto& passes = cam->GetRenderPasses();
		if (mSort3DNodesFrontToBack)
		{
			state->mManageFrontToBackStruct->Init(cam);
		}
		state->mVisibilityFrame++;
		state->SetCurrentCamera(cam);
		state->SetAllVisible(cam->AllVisible());
		cam->InitCullingObject(state->GetCullingObject().get());
		
		// reset visibility settings
		state->GetSceneGraph()->ResetVisibleNodeList();
		if (Node3D::Cull(state, 0))
		{
			Node3D::TravCull(state);
		}

		auto rs = cam->getValue<CMSP>("RenderingScreen");

		for (auto& pass : passes)
		{
#ifdef KIGS_TOOLS
			pass.debug_draw_path.clear();
#endif
			DrawableSorter_Camera lDrawableSorter;
			rs->setValue("ActiveDepthBuffer", pass.depth_buffer_index);
			rs->as<RenderingScreen>()->SetActive(state);

			state->SetCurrentLocalToGlobalMatrix(mat3x4::IdentityMatrix());
			state->SetCurrentGlobalToLocalMatrix(mat3x4::IdentityMatrix());
			state->mCurrentMaterial = 0;
			
			state->mPath = 0;
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
			state->mPath = &path;
			if (path.GetSize())
			{
				if (state->mCurrentMaterial)
				{
					Material* toPostDraw = state->mCurrentMaterial;
					state->mCurrentMaterial = 0;
					toPostDraw->CheckPostDraw(state);
				}

				Node3D::TravDraw(state);
				state->DrawInstances();
			}

			// TODO
			// change multipass support for shared material
			if (state->mCurrentMaterial)
			{
				Material*	toPostDraw = state->mCurrentMaterial;
				state->mCurrentMaterial = 0;
				toPostDraw->CheckPostDraw(state);
			}
#ifdef KIGS_TOOLS
			pass.record_pass = false;
#endif
		}
		state->mCurrentPass = nullptr;
		rs->setValue("ActiveDepthBuffer", 0);
		rs->as<RenderingScreen>()->SetActive(state);
		cam->Release(state);
	}

	if (mSort3DNodesFrontToBack)
	{
		delete state->mManageFrontToBackStruct;
		state->mManageFrontToBackStruct = 0;
	}

	// remove lights
	state->mLights = nullptr;
	// no more camera after scene 3D 
	state->SetCurrentCamera(nullptr);
}


void     Scene3D::TravCull(TravState* /* state */)
{

	// Do nothing here, we can not separate cull and draw for the scene as we have to do it once per camera

}


void Scene3D::SortItemsFrontToBack(Input::SortItemsFrontToBackParam& param)
{
	unsigned int mask = 0;
	auto collision = GetFirstInstance("CollisionManager");
	if (!collision || !(param.camera && param.camera->getValue("MaskForInputManager", mask))) 
	{
		std::transform(param.toSort.begin(), param.toSort.end(), std::back_inserter(param.sorted), [](CoreModifiable* cm)
		{
			return std::make_tuple(cm, Maths::Hit{});
		});
		return;
	}
	
	
	Camera* cam = (Camera*)param.camera;
	v3f cam_pos = cam->GetGlobalPosition();
	v3f cam_view = cam->GetGlobalViewVector();

	std::vector<Maths::Hit> hits;
	collision->SimpleCall("GetAllRayIntersection", param.origin, param.direction, hits, mask);
	
	struct Sorter
	{
		CoreModifiable* cm = nullptr;
		double dist = DBL_MAX;
		Maths::Hit* hit = nullptr;
		int sorting_layer = 0;
	};

	std::vector<Sorter> sorter(param.toSort.size(), Sorter{nullptr, DBL_MAX, nullptr, false });
	for (auto&& hit : hits)
	{
		auto n = hit.HitActor;
		if (hit.HitDistance > param.max_distance || hit.HitDistance < param.min_distance) 
			continue;
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
			Input::GetDistanceForInputSortParam params;
			params.camera = param.camera;
			params.position = param.position;
			params.origin = param.origin;
			params.direction = param.direction;
			params.hits = &hits;
			params.inout_distance = sorter[i].dist;
			params.inout_hit = sorter[i].hit;
			params.min_distance = param.min_distance;
			params.max_distance = param.max_distance;

			param.toSort[i]->SimpleCall("GetDistanceForInputSort", params);

			if (params.inout_distance > param.max_distance || params.inout_distance < param.min_distance)
				continue;

			sorter[i].dist = params.inout_distance;
			sorter[i].hit = params.inout_hit;
			sorter[i].sorting_layer = params.inout_sorting_layer;
		}
		else
		{
			if (sorter[i].dist == FLT_MAX && param.toSort[i]->isSubType("Node3D"))
			{
				auto bbox_local = param.toSort[i]->as<Node3D>()->GetLocalBoundingBox();

				auto origin_local = param.origin;
				v3f direction_local = param.direction;
				
				auto g2l = param.toSort[i]->as<Node3D>()->GetGlobalToLocal();

				g2l.TransformPoint(&origin_local);
				g2l.TransformVector(&direction_local);

				double distance = DBL_MAX;
				v3f intersection;
				v3f normal;
				v3f dir;

				if (Maths::IntersectionRayBBox(origin_local, direction_local, bbox_local.m_Min, bbox_local.m_Max, intersection, normal,distance))
				{
					dir = param.toSort[i]->as<Node3D>()->GetLocalToGlobal()*intersection - cam_pos;
				}
				else
				{
					dir = param.toSort[i]->as<Node3D>()->GetLocalToGlobal()*bbox_local.Center() - cam_pos;
				}

				auto d = length2(dir);
				if (Dot(dir, cam_view) < 0)
					d = -d;

				sorter[i].dist = d;
			}
		}
	}

	std::sort(sorter.begin(), sorter.end(), [](const Sorter& a, const Sorter& b) { return std::make_pair(a.sorting_layer, a.dist) <  std::make_pair(b.sorting_layer, b.dist); });

	param.sorted.resize(param.toSort.size());
	for (u32 i = 0; i < sorter.size(); ++i)
	{
		param.sorted[i] = std::make_tuple(sorter[i].cm, sorter[i].hit ? *sorter[i].hit : Maths::Hit{});
	}
}