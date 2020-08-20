#pragma once

#include "DataDrivenBaseApplication.h"
#include "AttributePacking.h"
#include "maReference.h"
#include "Node3D.h"
#include "Plane.h"

struct Hit;
class Panel;
class Holo3DPanel;
class UIVerticesInfo;
class Camera;

struct touchPosInfos;

struct GetDistanceForInputSortParam;
// ****************************************
// * Holo3DSequenceManager class
// * --------------------------------------
/**
 * \class	Holo3DSequenceManager
 * \file	Holo3DSequenceManager.h
 * \ingroup StandAlone
 * \brief	Sequence manager for Hololens.
 */
 // ****************************************
class Holo3DSequenceManager : public DataDrivenSequenceManager
{
public:
	DECLARE_CLASS_INFO(Holo3DSequenceManager, DataDrivenSequenceManager, 2DLayers)

	Holo3DSequenceManager(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	//! Do drawing here if any
	//void TravDraw(TravState* state) override;
	void Update(const Timer&  timer, void* addParam) override;


	bool	GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout);
	void	ShowPanel(bool show, bool atFront=true);

	void ChangePosition(Point3D pos) { mPosition = pos; mCurrentPos = pos; }
	
	SP<Node3D>& GetSpatialNode() { return mSpacialNode; }

	SP<Panel> GetCollidablePanel()
	{
		return mCollidablePanel;
	}

protected:
	WRAP_METHODS(GetDataInTouchSupport, ShowPanel, GetSpatialNode, GetDistanceForInputSort);
	
	
	void GetDistanceForInputSort(GetDistanceForInputSortParam&);
	
	~Holo3DSequenceManager();
	void InitModifiable() override;
	void UninitModifiable() override;
	void NotifyUpdate(const unsigned int labelid) override;

	// Node3D placed in the scene3D for collision managment
	SP<Node3D> mSpacialNode = nullptr;
	CMSP mRenderingScreen = nullptr;
	SP<Panel> mCollidablePanel = nullptr;
	SP<Holo3DPanel> mDrawer = nullptr;

	maReference mParentNode = BASE_ATTRIBUTE(ParentNode);
	maReference mFollowCamera = BASE_ATTRIBUTE(FollowCamera, "");

	maString  mRenderingScreenName = BASE_ATTRIBUTE(RenderingScreenName, "");

	maVect3DF mPosition;
	maVect3DF mNormal;
	maVect3DF mUp;
	maVect2DF mSize;
	maVect2DF mDesignSize;

	maBool mKeepDistance = BASE_ATTRIBUTE(KeepDistance, true);
	maFloat mDistance = BASE_ATTRIBUTE(Distance, 1);

	maFloat mDeltaDist = BASE_ATTRIBUTE(DeltaDist, 1);
	maFloat mMaxVelocity = BASE_ATTRIBUTE(MaxVelocity, 1);

	maUInt mCollideMask = BASE_ATTRIBUTE(CollideMask, 1);

	maInt mInputSortingLayer = BASE_ATTRIBUTE(InputSortingLayer, 0);

	maBool mUseFixedPosition = BASE_ATTRIBUTE(UseFixedPosition, true);
	maBool mUseFixedUp = BASE_ATTRIBUTE(UseFixedUp, true);
	maBool mUseFixedNormal = BASE_ATTRIBUTE(UseFixedNormal, true);

	maBool mManualPosition = BASE_ATTRIBUTE(ManualPosition, false);

	bool mNeedRecomputePosition = false;
	bool mIsFixed;
	bool mIsFirstDraw;
	bool mIsShow = false;

	bool mSetIsShow = false;

	bool mForceInFront = false;
	float mCurrentVelocity = 0.0f;

	Vector3D mCurrentView;
	Vector3D mCurrentPos;
	Vector3D mCurrentUp;
};


// ****************************************
// * InterfacePanel class
// * --------------------------------------
/**
 * \class	InterfacePanel
 * \file	Holo3DSequenceManager.h
 * \ingroup StandAlone
 * \brief	
 */
 // ****************************************
class InterfacePanel : public Panel
{
public:
	DECLARE_CLASS_INFO(InterfacePanel, Panel, StandAlone);
	DECLARE_INLINE_CONSTRUCTOR(InterfacePanel) {}

	bool ValidHit(const Point3D& pos) override
	{
		if (!ParentClassType::ValidHit(pos)) return false;
		if (!mContentForHitTest) return false;
		return mContentForHitTest->SimpleCall<bool>("PanelValidHit", v2f(mHitPos));
	}

protected:
	maReference mContentForHitTest = BASE_ATTRIBUTE(ContentForHitTest, "");
};

