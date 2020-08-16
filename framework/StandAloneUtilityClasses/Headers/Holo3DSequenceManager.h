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

	void ChangePosition(Point3D pos) { myPosition = pos; myCurrentPos = pos; }
	
	SP<Node3D>& GetSpatialNode() { return mySpacialNode; }

	SP<Panel> GetCollidablePanel()
	{
		return myCollidablePanel;
	}

protected:
	WRAP_METHODS(GetDataInTouchSupport, ShowPanel, GetSpatialNode, GetDistanceForInputSort);
	
	
	void GetDistanceForInputSort(GetDistanceForInputSortParam&);
	
	~Holo3DSequenceManager();
	void InitModifiable() override;
	void UninitModifiable() override;
	void NotifyUpdate(const unsigned int labelid) override;

	// Node3D placed in the scene3D for collision managment
	SP<Node3D> mySpacialNode = nullptr;
	CMSP myRenderingScreen = nullptr;
	SP<Panel> myCollidablePanel = nullptr;
	SP<Holo3DPanel> myDrawer = nullptr;

	maReference mParentNode = BASE_ATTRIBUTE(ParentNode);
	maReference mFollowCamera = BASE_ATTRIBUTE(FollowCamera, "");

	maString  myRenderingScreenName = BASE_ATTRIBUTE(RenderingScreenName, "");

	maVect3DF myPosition;
	maVect3DF myNormal;
	maVect3DF myUp;
	maVect2DF mySize;
	maVect2DF myDesignSize;

	maBool myKeepDistance = BASE_ATTRIBUTE(KeepDistance, true);
	maFloat myDistance = BASE_ATTRIBUTE(Distance, 1);

	// Offset is calculated as follow : (myTargetOffset.x*CameraRight + myTargetOffset.y*CameraUp)*myDistance
	maVect2DF myTargetOffset = BASE_ATTRIBUTE(TargetOffset, 0, 0);

	maFloat myDeltaDist = BASE_ATTRIBUTE(DeltaDist, 1);
	maFloat myMaxVelocity = BASE_ATTRIBUTE(MaxVelocity, 1);

	maUInt myCollideMask = BASE_ATTRIBUTE(CollideMask, 1);

	maInt myInputSortingLayer = BASE_ATTRIBUTE(InputSortingLayer, 0);

	maBool myUseFixedPosition = BASE_ATTRIBUTE(UseFixedPosition, true);
	maBool myUseFixedUp = BASE_ATTRIBUTE(UseFixedUp, true);
	maBool myUseFixedNormal = BASE_ATTRIBUTE(UseFixedNormal, true);

	maBool mManualPosition = BASE_ATTRIBUTE(ManualPosition, false);

	bool NeedRecomputePosition = false;
	bool IsFixed;
	bool IsFirstDraw;
	bool IsShow = false;

	bool SetIsShow = false;

	bool ForceInFront = false;
	float myCurrentVelocity = 0.0f;

	Vector3D myCurrentView;
	Vector3D myCurrentPos;
	Vector3D myCurrentUp;
};


class InterfacePanel : public Panel
{
public:
	DECLARE_CLASS_INFO(InterfacePanel, Panel, StandAlone);
	DECLARE_INLINE_CONSTRUCTOR(InterfacePanel) {}

	bool ValidHit(const Point3D& pos) override
	{
		if (!ParentClassType::ValidHit(pos)) return false;
		if (!mContentForHitTest) return false;
		return mContentForHitTest->SimpleCall<bool>("PanelValidHit", v2f(myHitPos));
	}

protected:
	maReference mContentForHitTest = BASE_ATTRIBUTE(ContentForHitTest, "");
};

