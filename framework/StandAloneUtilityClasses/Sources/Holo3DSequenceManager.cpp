#include "Holo3DSequenceManager.h"
#include "RenderingScreen.h"
#include "AttributePacking.h"
#include "ModuleBase.h"
#include "Holo3DPanel.h"
#include "Camera.h"
#include "Node3D.h"
#include "Timer.h"
#include "BaseUI2DLayer.h"
#include "UI/UIItem.h"

#include "Intersection.h"
#include "Plane.h"

#include "ModuleInput.h"
#include "TouchInputEventManager.h"

#include <algorithm>


extern bool gIsVR;

IMPLEMENT_CLASS_INFO(Holo3DSequenceManager);
IMPLEMENT_CLASS_INFO(InterfacePanel);

///////////////////////////////////////////
//
//			Holo3DSequenceManager
//
///////////////////////////////////////////
Holo3DSequenceManager::Holo3DSequenceManager(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: DataDrivenSequenceManager(name, PASS_CLASS_NAME_TREE_ARG)
	, myPosition(*this, false, "Position", 0.0f, 0.0f, 0.0f)
	, myNormal(*this, false, "Normal", 0.0f, 0.0f, 1.0f)
	, myUp(*this, false, "Up", 0.0f, 1.0f, 0.0f)
	, mySize(*this, false, "Size", .2f, .2f)
	, myDesignSize(*this, false, "DesignSize", 1024.0f, 1024.0f)
	, IsFirstDraw(true)
{
	myCurrentView.Set(0, 0, 0);
	myCurrentPos.Set(0, 0, 0);
	myCurrentUp.Set(0, 0, 0);
}

Holo3DSequenceManager::~Holo3DSequenceManager()
{
	if (mySpacialNode != nullptr)
		mySpacialNode->Destroy();
}

void Holo3DSequenceManager::UninitModifiable()
{
	// destroy sequence
	kstl::set<CoreModifiable*>	instances;
	CoreModifiable* mySceneGraph = KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);
	mySceneGraph->GetSonInstancesByType("Abstract2DLayer", instances);

	for (auto scene : instances)
	{
		CheckUniqueObject uo;
		scene->getValue("RenderingScreen", uo);

		if (uo == myRenderingScreen.get())
		{
			CMSP toDel(scene, GetRefTag{});
			mySceneGraph->removeItem(toDel);
		}
	}


	mySpacialNode=nullptr;
	myRenderingScreen = nullptr;

	//delete theMouseInfo;

	DataDrivenSequenceManager::UninitModifiable();
}

void Holo3DSequenceManager::ShowPanel(bool show, bool atFront)
{
	SetIsShow = show;
	ForceInFront = atFront;
}

bool Holo3DSequenceManager::GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout)
{
	if (!IsShow || !myCollidablePanel)
	{
		pout.pos.x = -1;
		pout.pos.y = -1;
		return false;
	}

	double dist = DBL_MAX;
	Point3D		planePos;
	Vector3D	planeNorm;
	myCollidablePanel->GetPlane(planePos, planeNorm);

	Vector3D pos = posin.origin;
	Vector3D dir = posin.dir;

	//! transform Ray in local mesh coordinate system if needed				
	auto& g2l = mySpacialNode->GetGlobalToLocal();
	g2l.TransformPoint(&pos);
	g2l.TransformVector(&dir);

	bool is_in = Intersection::IntersectRayPlane(pos, dir, planePos, planeNorm, dist);
	if (is_in)
	{
		pout.pos.xy = myCollidablePanel->ConvertHit(pos + ((float)dist*dir));
		pout.pos.x = 1 - pout.pos.x;
		pout.pos.y = 1 - pout.pos.y;
		is_in = myCollidablePanel->ValidHit(pos + dir * dist);
		if (is_in)
		{
			auto& l2g = mySpacialNode->GetLocalToGlobal();
			pout.hit.HitPosition = pos + dir * dist;
			pout.hit.HitNormal = -planeNorm;
			l2g.TransformPoint(&pout.hit.HitPosition);
			l2g.TransformVector(&pout.hit.HitNormal);
			pout.hit.HitNormal.Normalize();
			pout.hit.HitNode = mySpacialNode.get();
			pout.hit.HitActor = myCollidablePanel.get();
			pout.hit.HitCollisionObject = myCollidablePanel.get(); 
		}
		return is_in;
	}
	return false;
}

void Holo3DSequenceManager::InitModifiable()
{
	if (_isInit)
	{
		return;
	}
	DataDrivenSequenceManager::InitModifiable();

	if (_isInit)
	{
		Camera * followCam = (Camera*)((CoreModifiable*)mFollowCamera);
		if (followCam == 0)
		{
			UninitModifiable();
			return;
		}
	}

	NeedRecomputePosition = true;
	IsFixed = myUseFixedNormal && myUseFixedUp && myUseFixedPosition;

	myRenderingScreen = KigsCore::GetInstanceOf(myRenderingScreenName.const_ref(), "RenderingScreen");
	myRenderingScreen->setValue("IsOffScreen", true);
	myRenderingScreen->setValue("UseFBO", true);
	myRenderingScreen->setValue("FBOSizeX", myDesignSize[0]);
	myRenderingScreen->setValue("FBOSizeY", myDesignSize[1]);
	myRenderingScreen->setValue("DesignSizeX", myDesignSize[0]);
	myRenderingScreen->setValue("DesignSizeY", myDesignSize[1]);
	myRenderingScreen->setValue("BitsPerZ", 0); // 16
	//myRenderingScreen->setValue("BitsForStencil", 8);
	myRenderingScreen->Init();

	//Create Node3D
	mySpacialNode = KigsCore::GetInstanceOf("HoloUISpacialNode", "Node3D");
	mySpacialNode->AddDynamicAttribute(ATTRIBUTE_TYPE::UINT, "CollideMask", (u64)myCollideMask);
	mySpacialNode->Init();

	v2f size = mySize;
	if (gIsVR) size = mySize = size*2;
	
	//Create collidable object
	myCollidablePanel = KigsCore::GetInstanceOf("HoloUIPanel", "InterfacePanel");
	mySpacialNode->addItem((CMSP&)myCollidablePanel);
	myCollidablePanel->setValue("Size", size);
	myCollidablePanel->setValue("LinkedItem", "Holo3DSequenceManager:" + getName());
	myCollidablePanel->Init();


	//Create drawable object
	myDrawer = KigsCore::GetInstanceOf("Holo3DPanel", "Holo3DPanel");
	mySpacialNode->addItem((CMSP&)myDrawer);
	myDrawer->setValue("Size", size);
	myDrawer->setValue("DepthTest", "Disabled");
	myDrawer->setValue("RenderPassMask", 4);
	myDrawer->Init();


	/*
	auto customizer = KigsCore::CreateInstance("depth_test_disable", "RenderingCustomizer");
	customizer->setValue("TransarencyFlag", true);
	customizer->Init();
	customizer->setValue("OverrideDepthTest", 0);
	myDrawer->addItem(customizer.get());
	*/

	myPosition.changeNotificationLevel(Owner);
	myNormal.changeNotificationLevel(Owner);
	myUp.changeNotificationLevel(Owner);
	mySize.changeNotificationLevel(Owner);

	myUseFixedUp.changeNotificationLevel(Owner);
	myUseFixedNormal.changeNotificationLevel(Owner);
	myUseFixedPosition.changeNotificationLevel(Owner);

	//theMouseInfo = new LayerMouseInfo();

	ModuleInput* theInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");
	theInputModule->getTouchManager()->addTouchSupport(this, (CoreModifiable*)mFollowCamera);

	// add offscreen rendering screen as touch support with this as parent
	theInputModule->getTouchManager()->addTouchSupport(myRenderingScreen.get(), this);
}

//#define DRAWDEBUG
#ifdef DRAWDEBUG
#include "GLSLDebugDraw.h"
#define DRAWLINE(...) dd::line(__VA_ARGS__)
#define DRAWSPHERE(...) dd::sphere(__VA_ARGS__)
#else
#define DRAWLINE(...) 
#define DRAWSPHERE(...) 
#endif

void Holo3DSequenceManager::Update(const Timer&  aTimer, void* addParam)
{
	// first draw init
	if (IsFirstDraw)
	{
		if (!_isInit)
		{
			Init();

			if (!_isInit)
				return;
		}

		ShowPanel(true);
		//((CoreModifiable*)mCollideScene)->addItem(mySpacialNode);
		IsFirstDraw = false;
	}

	if (SetIsShow != IsShow)
	{
		if ((CoreModifiable*)mParentNode == nullptr)
			return;

		IsShow = SetIsShow;

		kstl::set<CoreModifiable*> layers;
		if(myCurrentSequence)
			myCurrentSequence->GetSonInstancesByType("BaseUI2DLayer", layers);

		for (auto l : layers)
		{
			auto layer = (BaseUI2DLayer*)l;
			layer->GetRootItem()->setValue("IsHidden", !IsShow);
		}
		
		if (IsShow)
			((CoreModifiable*)mParentNode)->addItem((CMSP&)mySpacialNode, ItemPosition::First);
		else
			((CoreModifiable*)mParentNode)->removeItem((CMSP&)mySpacialNode);
	}
	

	// retreive texture id
	myDrawer->SetTexture(myRenderingScreen->as<RenderingScreen>()->GetFBOTexture().get());
	// manage positionning
	if (!mManualPosition && (NeedRecomputePosition || !IsFixed))
	{
		Camera * followCam = (Camera*)((CoreModifiable*)mFollowCamera);
		Vector3D camPos = followCam->GetGlobalPosition();

		Matrix3x4 myMat = mySpacialNode->GetLocalToGlobal();
		Vector3D nodePos(myMat.e[3][0], myMat.e[3][1], myMat.e[3][2]);

		Vector3D view, pos, up;
		if (!myUseFixedNormal)
		{
			//view = followCam->GetGlobalViewVector();
			view = (nodePos - camPos).Normalized();

			if (view != myCurrentView)
				NeedRecomputePosition = true;
		}
		else
		{
			view = (Vector3D)myNormal;
		}

		if (!myUseFixedUp)
		{
			up = followCam->GetGlobalUpVector();

			if (up != myCurrentUp)
				NeedRecomputePosition = true;
		}
		else
		{
			up = (Vector3D)myUp;
		}

		if (ForceInFront)
		{
			pos = camPos + (myDistance*followCam->GetGlobalViewVector());
			myCurrentVelocity = 0.0f;
			ForceInFront = false;
			NeedRecomputePosition = true;
		}
		else if (!myUseFixedPosition)
		{
			Vector3D targetPos = camPos + (myDistance*followCam->GetGlobalViewVector());
			DRAWSPHERE(nodePos, Vector3D(0, 255, 255), 0.01);

			Vector3D dir = (targetPos - nodePos);
			float dist = Norm(dir);
			dir.Normalize();

			bool needKeepDistance = false;

			// check if we need to move
			if (dist - myDeltaDist > 0.01f)
			{
				float lVelocity = 0.0f;

				DRAWLINE(targetPos, nodePos, Vector3D(255, 0, 0));
				DRAWSPHERE(targetPos, Vector3D(255, 0, 0), 0.01);

				lVelocity = std::min(dist - myDeltaDist, (float)myMaxVelocity);
				myCurrentVelocity = std::max(lVelocity, myCurrentVelocity);
			}
			// check the distance to cam
			else if (myKeepDistance)
			{
				DRAWLINE(targetPos, nodePos, Vector3D(255, 0, 0));
				DRAWSPHERE(targetPos, Vector3D(255, 0, 0), 0.01);

				Vector3D dirToCal = (nodePos - camPos);
				float distToCam = NormSquare(dirToCal);
				if (fabs(distToCam - (myDistance*myDistance)) > 0.001f)
				{
					needKeepDistance = true;
				}
			}
			else
			{
				DRAWLINE(targetPos, nodePos, Vector3D(0, 255, 0));
				DRAWSPHERE(targetPos, Vector3D(0, 255, 0), 0.01);
			}

			//	apply velocity
			if (myCurrentVelocity > 0.001f)
			{
				if (dist < myCurrentVelocity)
				{
					pos = targetPos;
				}
				else
				{
					pos = nodePos + dir*myCurrentVelocity;
				}

				myCurrentVelocity *= 0.5f;
			}
			else if (needKeepDistance)
			{
				Vector3D dir = (nodePos - camPos);
				dir.Normalize();

				pos = camPos + (myDistance*dir);
			}
			else
			{
				pos = myCurrentPos;
				myCurrentVelocity = 0.0f;
			}

			if (pos != myCurrentPos)
				NeedRecomputePosition = true;
		}
		else
		{
			pos = (Vector3D)myPosition;
		}

		if (NeedRecomputePosition)
		{
			DRAWSPHERE(pos, Vector3D(255, 0, 0), 0.01);
			// compute local matrix
			Vector3D right;

			right = up^view;
			right.Normalize();
			if (myUseFixedUp)
			{
				if (!myUseFixedNormal)
					view = right^up;
			}
			else if (myUseFixedNormal)
			{
				up = right^-view;
			}

			Matrix3x4 mat(right, up, view, pos);

			// check if moved (avoid recompute matrix and bbox if none)
			if (myCurrentView == view &&
				myCurrentPos == pos &&
				myCurrentUp == up)
			{
				// no change do nothing
			}
			else
			{
				if (mySpacialNode->getFather())
					mat = mySpacialNode->getFather()->GetGlobalToLocal() * mat;
				mySpacialNode->ChangeMatrix(mat);

				myCurrentView = view;
				myCurrentPos = pos;
				myCurrentUp = up;
			}

			NeedRecomputePosition = false;
		}
	}
	DataDrivenSequenceManager::Update(aTimer, addParam);
}

void Holo3DSequenceManager::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == myPosition.getLabelID()
		|| labelid == myNormal.getLabelID()
		|| labelid == myUp.getLabelID())
	{
		NeedRecomputePosition = true;
	}
	else if (labelid == mySize.getLabelID())
	{

	}
	else if (labelid == myUseFixedNormal.getLabelID()
		|| labelid == myUseFixedUp.getLabelID()
		|| labelid == myUseFixedPosition.getLabelID())
	{
		NeedRecomputePosition = true;
		IsFixed = myUseFixedNormal && myUseFixedUp && myUseFixedPosition;
	}

	DataDrivenSequenceManager::NotifyUpdate(labelid);
}

void Holo3DSequenceManager::GetDistanceForInputSort(GetDistanceForInputSortParam& params)
{
	// TODO 2 modes: always on front, and regular collider distance
	params.inout_distance = -FLT_MAX;
}