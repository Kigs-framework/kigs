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

#include "IFC/CollisionMasks.h"

#include <algorithm>


IMPLEMENT_CLASS_INFO(Holo3DSequenceManager);
IMPLEMENT_CLASS_INFO(InterfacePanel);

///////////////////////////////////////////
//
//			Holo3DSequenceManager
//
///////////////////////////////////////////
Holo3DSequenceManager::Holo3DSequenceManager(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: DataDrivenSequenceManager(name, PASS_CLASS_NAME_TREE_ARG)
	, mPosition(*this, false, "Position", 0.0f, 0.0f, 0.0f)
	, mNormal(*this, false, "Normal", 0.0f, 0.0f, 1.0f)
	, mUp(*this, false, "Up", 0.0f, 1.0f, 0.0f)
	, mSize(*this, false, "Size", .2f, .2f)
	, mDesignSize(*this, false, "DesignSize", 1024.0f, 1024.0f)
	, mIsFirstDraw(true)
{
	mCurrentView.Set(0, 0, 0);
	mCurrentPos.Set(0, 0, 0);
	mCurrentUp.Set(0, 0, 0);
}

Holo3DSequenceManager::~Holo3DSequenceManager()
{
	if (mSpacialNode != nullptr)
		mSpacialNode->Destroy();
}

void Holo3DSequenceManager::UninitModifiable()
{
	// destroy sequence
	std::vector<CMSP>	instances;
	CoreModifiable* sceneGraph = KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);
	sceneGraph->GetSonInstancesByType("Abstract2DLayer", instances);

	for (auto scene : instances)
	{
		CoreModifiable* uo=nullptr;
		scene->getValue("RenderingScreen", uo);

		if (uo == mRenderingScreen.get())
		{
			sceneGraph->removeItem(scene);
		}
	}


	mSpacialNode=nullptr;
	mRenderingScreen = nullptr;

	//delete theMouseInfo;

	DataDrivenSequenceManager::UninitModifiable();
}

void Holo3DSequenceManager::ShowPanel(bool show, bool atFront)
{
	mSetIsShow = show;
	mForceInFront = atFront;
}

bool Holo3DSequenceManager::GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout)
{
	if (!mIsShow || !mCollidablePanelInputs)
	{
		pout.pos.x = -1;
		pout.pos.y = -1;
		return false;
	}

	double dist = DBL_MAX;
	Point3D		planePos;
	Vector3D	planeNorm;
	mCollidablePanelInputs->GetPlane(planePos, planeNorm);

	Vector3D pos = posin.origin;
	Vector3D dir = posin.dir;

	//! transform Ray in local mesh coordinate system if needed				
	auto& g2l = mSpacialNode->GetGlobalToLocal();
	g2l.TransformPoint(&pos);
	g2l.TransformVector(&dir);

	bool is_in = Intersection::IntersectRayPlane(pos, dir, planePos, planeNorm, dist);
	if (is_in)
	{
		auto& l2g = mSpacialNode->GetLocalToGlobal();
		Vector3D dist_vector = Vector3D(dist, 0, 0);
		l2g.TransformVector(&dist_vector);
		auto real_dist = Norm(dist_vector);
		if (real_dist > pout.max_distance || real_dist < pout.min_distance)
		{
			pout.pos.x = -1;
			pout.pos.y = -1;
			return false;
		}

		pout.pos.xy = mCollidablePanelInputs->ConvertHit(pos + ((float)dist*dir));
		if(Dot(dir, planeNorm) > 0)
			pout.pos.x = 1 - pout.pos.x;
		pout.pos.y = 1 - pout.pos.y;
		is_in = mCollidablePanelInputs->ValidHit(pos + dir * dist);
		if (is_in)
		{
			pout.hit.HitPosition = pos + dir * dist;
			pout.hit.HitNormal = -planeNorm;
			l2g.TransformPoint(&pout.hit.HitPosition);
			l2g.TransformVector(&pout.hit.HitNormal);
			pout.hit.HitNormal.Normalize();
			pout.hit.HitNode = mSpacialNode.get();
			pout.hit.HitActor = mCollidablePanelInputs.get();
			pout.hit.HitCollisionObject = mCollidablePanelInputs.get();
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

	mNeedRecomputePosition = true;
	mIsFixed = mUseFixedNormal && mUseFixedUp && mUseFixedPosition;

	mRenderingScreen = KigsCore::GetInstanceOf(mRenderingScreenName.const_ref(), "RenderingScreen");
	mRenderingScreen->setValue("IsOffScreen", true);
	mRenderingScreen->setValue("UseFBO", true);
	mRenderingScreen->setValue("FBOSizeX", mDesignSize[0]);
	mRenderingScreen->setValue("FBOSizeY", mDesignSize[1]);
	mRenderingScreen->setValue("DesignSizeX", mDesignSize[0]);
	mRenderingScreen->setValue("DesignSizeY", mDesignSize[1]);
	mRenderingScreen->setValue("BitsPerZ", 0); // 16
	//myRenderingScreen->setValue("BitsForStencil", 8);
	mRenderingScreen->Init();

	//Create Node3D
	mSpacialNode = KigsCore::GetInstanceOf("HoloUISpacialNode", "Node3D");
	mSpacialNode->AddDynamicAttribute(ATTRIBUTE_TYPE::UINT, "CollideMask", (u64)mCollideMask);
	mSpacialNode->Init();

	v2f size = mSize;
	
	//Create collidable object
	mCollidablePanelRegular = KigsCore::GetInstanceOf("HoloUIPanel", "InterfacePanel");
	mSpacialNode->addItem(mCollidablePanelRegular);
	mCollidablePanelRegular->setValue("Size", size);
	mCollidablePanelRegular->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::UINT, "CollideMask", CollisionMask::Tools);
	mCollidablePanelRegular->Init();

	mCollidablePanelInputs = KigsCore::GetInstanceOf("HoloUIPanelInputs", "Panel");
	mSpacialNode->addItem(mCollidablePanelInputs);
	mCollidablePanelInputs->setValue("Size", size);
	mCollidablePanelInputs->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::UINT, "CollideMask", 32768llu);
	mCollidablePanelInputs->Init();

	//Create drawable object
	mDrawer = KigsCore::GetInstanceOf("Holo3DPanel", "Holo3DPanel");
	mSpacialNode->addItem(mDrawer);
	mDrawer->setValue("Size", size);
	mDrawer->setValue("DepthTest", "Disabled");
	mDrawer->setValue("RenderPassMask", 64);
	mDrawer->setValue("TwoSided", true);
	mDrawer->Init();

	mPosition.changeNotificationLevel(Owner);
	mNormal.changeNotificationLevel(Owner);
	mUp.changeNotificationLevel(Owner);
	mSize.changeNotificationLevel(Owner);

	mUseFixedUp.changeNotificationLevel(Owner);
	mUseFixedNormal.changeNotificationLevel(Owner);
	mUseFixedPosition.changeNotificationLevel(Owner);

	//theMouseInfo = new LayerMouseInfo();

	ModuleInput* theInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");
	theInputModule->getTouchManager()->addTouchSupport(this, (CoreModifiable*)mFollowCamera);

	// add offscreen rendering screen as touch support with this as parent
	theInputModule->getTouchManager()->addTouchSupport(mRenderingScreen.get(), this);
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
	if (mIsFirstDraw)
	{
		if (!_isInit)
		{
			Init();

			if (!_isInit)
				return;
		}

		ShowPanel(true);
		//((CoreModifiable*)mCollideScene)->addItem(mySpacialNode);
		mIsFirstDraw = false;
	}

	if (mSetIsShow != mIsShow)
	{
		if ((CoreModifiable*)mParentNode == nullptr)
			return;

		mIsShow = mSetIsShow;

		std::vector<CMSP> layers;
		if(mCurrentSequence)
			mCurrentSequence->GetSonInstancesByType("BaseUI2DLayer", layers);

		for (auto l : layers)
		{
			auto layer = (BaseUI2DLayer*)l.get();
			layer->GetRootItem()->setValue("IsHidden", !mIsShow);
		}
		
		if (mIsShow)
			((CoreModifiable*)mParentNode)->addItem(mSpacialNode, ItemPosition::First);
		else
			((CoreModifiable*)mParentNode)->removeItem(mSpacialNode);
	}
	

	// retreive texture id
	mDrawer->SetTexture(mRenderingScreen->as<RenderingScreen>()->GetFBOTexture().get());
	// manage positionning
	if ((!mManualPosition && (mNeedRecomputePosition || !mIsFixed)) || mForceInFront)
	{
		Camera * followCam = (Camera*)((CoreModifiable*)mFollowCamera);
		Vector3D camPos = followCam->GetGlobalPosition();

		Matrix3x4 myMat = mSpacialNode->GetLocalToGlobal();
		Vector3D nodePos(myMat.e[3][0], myMat.e[3][1], myMat.e[3][2]);

		Vector3D view, pos, up;
		if (!mUseFixedNormal)
		{
			//view = followCam->GetGlobalViewVector();
			view = (nodePos - camPos).Normalized();

			if (view != mCurrentView)
				mNeedRecomputePosition = true;
		}
		else
		{
			view = (Vector3D)mNormal;
		}

		if (!mUseFixedUp)
		{
			up = followCam->GetGlobalUpVector();

			if (up != mCurrentUp)
				mNeedRecomputePosition = true;
		}
		else
		{
			up = (Vector3D)mUp;
		}

		if (mForceInFront)
		{
			pos = camPos + (mDistance * (followCam->GetGlobalViewVector() + mTargetOffset[0]*followCam->GetGlobalRightVector() + mTargetOffset[1] * followCam->GetGlobalUpVector()));
			view = (pos - camPos).Normalized();
			mCurrentVelocity = 0.0f;
			mForceInFront = false;
			mNeedRecomputePosition = true;
		}
		else if (!mUseFixedPosition)
		{
			Vector3D targetPos = camPos + (mDistance * (followCam->GetGlobalViewVector() + mTargetOffset[0] * followCam->GetGlobalRightVector() + mTargetOffset[1] * followCam->GetGlobalUpVector()));
			DRAWSPHERE(nodePos, Vector3D(0, 255, 255), 0.01);

			Vector3D dir = (targetPos - nodePos);
			float dist = Norm(dir);
			dir.Normalize();

			bool needKeepDistance = false;

			// check if we need to move
			if (dist - mDeltaDist > 0.01f)
			{
				float lVelocity = 0.0f;

				DRAWLINE(targetPos, nodePos, Vector3D(255, 0, 0));
				DRAWSPHERE(targetPos, Vector3D(255, 0, 0), 0.01);

				lVelocity = std::min(dist - mDeltaDist, (float)mMaxVelocity);
				mCurrentVelocity = std::max(lVelocity, mCurrentVelocity);
			}
			// check the distance to cam
			else if (mKeepDistance)
			{
				DRAWLINE(targetPos, nodePos, Vector3D(255, 0, 0));
				DRAWSPHERE(targetPos, Vector3D(255, 0, 0), 0.01);

				Vector3D dirToCal = (nodePos - camPos);
				float distToCam = NormSquare(dirToCal);
				if (fabs(distToCam - (mDistance*mDistance)) > 0.001f)
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
			if (mCurrentVelocity > 0.001f)
			{
				if (dist < mCurrentVelocity)
				{
					pos = targetPos;
				}
				else
				{
					pos = nodePos + dir*mCurrentVelocity;
				}

				mCurrentVelocity *= 0.5f;
			}
			else if (needKeepDistance)
			{
				Vector3D dir = (nodePos - camPos);
				dir.Normalize();

				pos = camPos + (mDistance*dir);
			}
			else
			{
				pos = mCurrentPos;
				mCurrentVelocity = 0.0f;
			}

			if (pos != mCurrentPos)
				mNeedRecomputePosition = true;
		}
		else
		{
			pos = (Vector3D)mPosition;
		}

		if (mNeedRecomputePosition)
		{
			DRAWSPHERE(pos, Vector3D(255, 0, 0), 0.01);
			// compute local matrix
			Vector3D right;

			right = up^view;
			right.Normalize();
			if (mUseFixedUp)
			{
				if (!mUseFixedNormal)
					view = right^up;
			}
			else if (mUseFixedNormal)
			{
				up = right^-view;
			}

			Matrix3x4 mat(right, up, view, pos);

			// check if moved (avoid recompute matrix and bbox if none)
			if (mCurrentView == view &&
				mCurrentPos == pos &&
				mCurrentUp == up)
			{
				// no change do nothing
			}
			else
			{
				if (mSpacialNode->getFather())
					mat = mSpacialNode->getFather()->GetGlobalToLocal() * mat;
				mSpacialNode->ChangeMatrix(mat);

				mCurrentView = view;
				mCurrentPos = pos;
				mCurrentUp = up;
			}

			mNeedRecomputePosition = false;
		}
	}
	DataDrivenSequenceManager::Update(aTimer, addParam);
}

void Holo3DSequenceManager::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == mPosition.getLabelID()
		|| labelid == mNormal.getLabelID()
		|| labelid == mUp.getLabelID())
	{
		mNeedRecomputePosition = true;
	}
	else if (labelid == mSize.getLabelID())
	{
		v2f size = mSize;
		if (mCollidablePanelRegular) mCollidablePanelRegular->setValue("Size", size);
		if (mCollidablePanelInputs) mCollidablePanelInputs->setValue("Size", size);
		if (mDrawer) mDrawer->setValue("Size", size);
	}
	else if (labelid == mUseFixedNormal.getLabelID()
		|| labelid == mUseFixedUp.getLabelID()
		|| labelid == mUseFixedPosition.getLabelID())
	{
		mNeedRecomputePosition = true;
		mIsFixed = mUseFixedNormal && mUseFixedUp && mUseFixedPosition;
	}

	DataDrivenSequenceManager::NotifyUpdate(labelid);
}

void Holo3DSequenceManager::GetDistanceForInputSort(GetDistanceForInputSortParam& params)
{
	params.inout_sorting_layer = mInputSortingLayer;
	params.inout_distance = DBL_MAX;
	if (!mIsShow || !mCollidablePanelInputs)
		return;

	v3f local_pos = params.origin;
	Vector3D local_dir = params.direction;

	Point3D		planePos;
	Vector3D	planeNorm;
	mCollidablePanelInputs->GetPlane(planePos, planeNorm);

	auto& inverseMatrix = mSpacialNode->GetGlobalToLocal();
	inverseMatrix.TransformPoint(&local_pos);
	inverseMatrix.TransformVector(&local_dir);

	f64 dist = DBL_MAX;
	if (Intersection::IntersectRayPlane(local_pos, local_dir, planePos, planeNorm, dist, mCollidablePanelInputs.get()))
	{
		auto& l2g = mSpacialNode->GetLocalToGlobal();
		Vector3D dist_vector = Vector3D(dist, 0, 0);
		l2g.TransformVector(&dist_vector);
		auto real_dist = Norm(dist_vector);
		params.inout_distance = (f64)real_dist;
	}

}