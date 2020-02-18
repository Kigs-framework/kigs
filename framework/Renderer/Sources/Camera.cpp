#include "PrecompiledHeaders.h"

#include "Camera.h"
#include "CullingObject.h"
#include "TravState.h"
#include "NotificationCenter.h"
#include "ModuleSceneGraph.h"
#include "ModuleRenderer.h"
#include "ModuleInput.h"
#include "TouchInputEventManager.h"
#include "CoreBaseApplication.h"

#define DEG_TO_RAD    KFLOAT_CONST(3.1415926535897932384626433832795f/180.0f)
#define DEG_TO_RAD_2  KFLOAT_CONST(3.1415926535897932384626433832795f/360.0f)

IMPLEMENT_CLASS_INFO(Camera)

IMPLEMENT_CONSTRUCTOR(Camera)
, myViewportMinX(*this, false, "ViewportMinX", 0.0f)
, myViewportMinY(*this, false, "ViewportMinY", 0.0f)
, myViewportSizeX(*this, false, "ViewportSizeX", 1.0f)
, myViewportSizeY(*this, false, "ViewportSizeY", 1.0f)
, myNear(*this, false, "NearPlane", 0.1f)
, myFar(*this, false, "FarPlane", 40.0f)
, myPosition(*this, false, "Position")
, myUpVector(*this, false, "UpVector")
, myViewVector(*this, false, "ViewVector")
, myClearColor(*this, false, "ClearColor")
, myRenderingScreen(*this, true, "RenderingScreen")
, myVerticalFOV(*this, false, "VerticalFOV", 45.0f)
, myAspectRatio(*this, false, "AspectRatio", 0.0f)
, myClearZBuffer(*this, false, "ClearZBuffer", true)
, myClearColorBuffer(*this, false, "ClearColorBuffer", true)
, myClearStencilBuffer(*this, false, "ClearStencilBuffer", false)
, mybCameraIsEnabled(*this, false, "CameraIsEnabled", true)
, myPriority(*this, false, "Priority", 0)
, myBrightness(*this, false, "Brightness", 0)
, myAllVisible(*this, false, "AllVisible", false)
, myTouchControlled(*this, false, "TouchControlled", false)
, myIsActive(false)
{
	SetPosition(0.0f, 0.0f, 0.0f);
	SetViewVector(1.0f, 0.0f, 0.0f);
	SetUpVector(0.0f, 0.0f, 1.0f);
	SetClearColor(0.8f, 0.8f, 1.0f);
	
	RenderPass opaque_pass;
	opaque_pass.pass_mask = 1;
	mRenderPasses.push_back(opaque_pass);

	RenderPass transparent_pass;
	transparent_pass.pass_mask = 2;
	transparent_pass.use_front_to_back_sorter = true;
	transparent_pass.allow_instancing = false;
	mRenderPasses.push_back(transparent_pass);

	OVERLOAD_DECORABLE(Cull, Node3D, Camera);
}


void Camera::UninitModifiable()
{
	void* datastruct;
	if (getValue("TouchControlledData", datastruct))
	{
		if (datastruct)
		{
			delete (touchControlledDataStruct*) datastruct;
		}
	}

	Node3D::UninitModifiable();

}

void Camera::ProtectedDestroy()
{
	UninitModifiable();
	Node3D::ProtectedDestroy();
}


Camera::~Camera()
{
	// notify scenegraph that I am dead
	ModuleSceneGraph* scenegraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);
	scenegraph->NotifyDefferedItemDeath(this);

	ModuleInput* theInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");
	if(theInputModule)
		theInputModule->getTouchManager()->removeTouchSupport(this);
}

void Camera::NotifyUpdate(const unsigned int  labelid)
{
	Node3D::NotifyUpdate(labelid);
	if (labelid == myTouchControlled.getID())
	{
		activeTouchControlledCamera(myTouchControlled);
	}
	else
	{
		RecomputeMatrix();
	}
}

void Camera::RecomputeMatrix()
{
	// compute local matrix
	Vector3D  view,up,right,pos;
	
	view.x=myViewVector[0];
	view.y=myViewVector[1];
	view.z=myViewVector[2];
	
	view.Normalize();
	
	pos.x=myPosition[0];
	pos.y=myPosition[1];
	pos.z=myPosition[2];
	
	
	up.x=myUpVector[0];
	up.y=myUpVector[1];
	up.z=myUpVector[2];
	
	up.Normalize();
	
	right=up^view;
	
	right.Normalize();
	
	up=view^right;

	Matrix3x4 matrix(view, right, up, pos);
	ParentClassType::ChangeMatrix(matrix);
}

bool Camera::SetActive(TravState* state)
{	
	myIsActive=true;
	bool b = ProtectedSetActive(state);
	if (b)
	{
		getRenderingScreen()->setValue("Brightness", myBrightness);
	}
	KigsCore::GetNotificationCenter()->postNotificationName("OnCamera_SetActive", this);
	return b;
}

void Camera::Release(TravState* state)
{	
	myIsActive=false;
	ProtectedRelease(state);
	KigsCore::GetNotificationCenter()->postNotificationName("OnCamera_Release", this);
}

void Camera::InitModifiable()
{
	Node3D::InitModifiable();
	if(IsInit())
	{
		RenderingScreen* screen=getRenderingScreen();
		if(screen)
		{
			RecomputeMatrix();
			//! notification on matrix change
			myPosition.changeNotificationLevel(Owner);
			myUpVector.changeNotificationLevel(Owner);
			myViewVector.changeNotificationLevel(Owner);
			myViewportSizeX.changeNotificationLevel(Owner);
			myViewportSizeY.changeNotificationLevel(Owner);
			myTouchControlled.changeNotificationLevel(Owner);

			// declare as a touch support potential target with rendering screen as parent
			ModuleInput* theInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");
			if(theInputModule)
				theInputModule->getTouchManager()->addTouchSupport(this,myRenderingScreen);	
			

			activeTouchControlledCamera(myTouchControlled);
			// test
			//theInputModule->getTouchManager()->registerEvent(this, "ManageClickTouchEvent", Click, emptyFlag);
		}
		else
		{
			UninitModifiable();
		}
	}
}

// test
/*
bool Camera::ManageClickTouchEvent(v3f pos, int type, int state, int clickCount)
{
	
	if (state == StateRecognized)
	{
		return true;
	}
	return false;
}
*/
// for 3D view, pos is eye pos and move is view vector
bool Camera::GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout)
{
	// TODO : probably not enough as we can have several camera in a scene and not all attached to holographic view 
	if (myRenderingScreen->as<RenderingScreen>()->IsHolographic())
	{
		pout.origin = GetGlobalPosition();
		pout.dir = GetGlobalViewVector();
		pout.setHas3DInfos(true);
		return true;
	}
	else
	{
		SetupNodeIfNeeded();

		float width, height;
		getRenderingScreen()->GetSize(width, height);
		float aspect = (width*myViewportSizeX) / (height*myViewportSizeY);

		float dwidth, dheight;
		getRenderingScreen()->GetDesignSize(dwidth, dheight);
		// size or design size here ?

		Point2D pos2D;

		pos2D.x = posin.pos.x / dwidth;
		pos2D.y = posin.pos.y / dheight;

		// pos in [-0.5,+0.5]
		pos2D.x = ((pos2D.x - myViewportMinX) / myViewportSizeX) - 0.5f;
		pos2D.y = ((pos2D.y - myViewportMinY) / myViewportSizeY) - 0.5f;
		

		float tt = 2.0f*tan(((float)myVerticalFOV)*DEG_TO_RAD_2);
		pout.dir.x = 1.0f;
		pout.dir.z = -pos2D.y*tt;
		pout.dir.y = -pos2D.x*tt*aspect;
		GetLocalToGlobal().TransformVector(&pout.dir);
		pout.dir.Normalize();

		pout.origin.x = GetLocalToGlobal().e[3][0];
		pout.origin.y = GetLocalToGlobal().e[3][1];
		pout.origin.z = GetLocalToGlobal().e[3][2];

		pout.setHas3DInfos(true);

		// pos in viewport
		pout.pos.x = (pos2D.x + 0.5f)*myViewportSizeX*dwidth;
		pout.pos.y = (pos2D.y + 0.5f)*myViewportSizeY*dheight;
		pout.pos.z = 0;
		if ((pos2D.x >= -0.5f) && (pos2D.x < 0.5f))
		{
			if ((pos2D.y >= -0.5f) && (pos2D.y < 0.5f))
			{
				return true;
			}
		}
	}

	return false;
}

void  Camera::InitCullingObject(CullingObject* obj)
{
	Vector3D  n;
	Point3D   o;
	
	int i;
	
	// add missing planes if needed
	for(i=obj->GetPlaneCount();i<6;i++)
	{
		obj->AddPlane(n,o);
	}
	
	// remove additionnal planes if needed
	for(i=6;i<obj->GetPlaneCount();i++)
	{
		obj->RemovePlane(6);
	}
	
	kfloat width , height;
	getRenderingScreen()->GetSize(width , height);
	
	
	kfloat screenRotation;
	getRenderingScreen()->GetRotation(screenRotation);
	
	
	kfloat aspect = (width*myViewportSizeX) / (height*myViewportSizeY);
	if((screenRotation==90.0f)||(screenRotation==270.0f))
		aspect=1.0f/aspect;
	
	kfloat d = 2.0f*tanf(myVerticalFOV*DEG_TO_RAD*0.5f);
	kfloat hfov = 2.0f*atanf(d*aspect*0.5f);
	
	auto l2g = GetLocalToGlobal();

	// near plane
	n.Set(1.0f,0.0f,0.0f);
	o.Set(myNear,0.0f,0.0f);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(0,n,o);
	
	// far plane
	n.Set(-1.0f,0.0f,0.0f);
	o.Set(myFar,0.0f,0.0f);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(1,n,o);
	
	// down plane
	kfloat s=sinf((kfloat)myVerticalFOV*0.5f*DEG_TO_RAD);
	kfloat c=cosf((kfloat)myVerticalFOV*0.5f*DEG_TO_RAD);
	
	n.Set(s,0.0f,c);
	o.Set(0.0f,0.0f,0.0f);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(2,n,o);
	
	// up plane
	n.Set(s,0.0f,-c);
	l2g.TransformVector(&n);
	obj->InitPlane(3,n,o);
	
	// left plane
	//s = sinf((kfloat)myVerticalFOV*aspect*0.5f*DEG_TO_RAD);
	s = sinf(hfov*0.5f);
	//c = cosf((kfloat)myVerticalFOV*aspect*0.5f*DEG_TO_RAD);
	c = cosf(hfov*0.5f);
	
	n.Set(s,-c,0.0f);
	l2g.TransformVector(&n);
	obj->InitPlane(4,n,o);
	
	// right plane
	n.Set(s,c,0.0f);
	l2g.TransformVector(&n);
	obj->InitPlane(5,n,o);
}

Camera::FrustumPlanes Camera::GetFrustum()
{
	FrustumPlanes result;

	v3f o;
	Vector3D n;

	kfloat width, height;
	getRenderingScreen()->GetSize(width, height);


	kfloat screenRotation;
	getRenderingScreen()->GetRotation(screenRotation);


	kfloat aspect = (width*myViewportSizeX) / (height*myViewportSizeY);
	if ((screenRotation == 90.0f) || (screenRotation == 270.0f))
		aspect = 1.0f / aspect;

	kfloat d = 2.0f*tanf(myVerticalFOV*DEG_TO_RAD*0.5f);
	kfloat hfov = 2.0f*atanf(d*aspect*0.5f);

	auto l2g = GetLocalToGlobal();

	// near plane
	n.Set(1.0f, 0.0f, 0.0f);
	o.Set(myNear, 0.0f, 0.0f);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o, 1);

	result.Near.n = n;
	result.Near.o = o;

	// far plane
	n.Set(-1.0f, 0.0f, 0.0f);
	o.Set(myFar, 0.0f, 0.0f);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o, 1);
	result.Far.n = n;
	result.Far.o = o;

	// down plane
	kfloat s = sinf((kfloat)myVerticalFOV*0.5f*DEG_TO_RAD);
	kfloat c = cosf((kfloat)myVerticalFOV*0.5f*DEG_TO_RAD);

	n.Set(s, 0.0f, c);
	o.Set(0.0f, 0.0f, 0.0f);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o, 1);
	result.Down.n = n;
	result.Down.o = o;

	// up plane
	n.Set(s, 0.0f, -c);
	l2g.TransformVector(&n);
	result.Up.n = n;
	result.Up.o = o;

	// left plane
	//s = sinf((kfloat)myVerticalFOV*aspect*0.5f*DEG_TO_RAD);
	s = sinf(hfov*0.5f);
	//c = cosf((kfloat)myVerticalFOV*aspect*0.5f*DEG_TO_RAD);
	c = cosf(hfov*0.5f);

	n.Set(s, -c, 0.0f);
	l2g.TransformVector(&n);
	result.Left.n = n;
	result.Left.o = o;

	// right plane
	n.Set(s, c, 0.0f);
	l2g.TransformVector(&n);
	result.Right.n = n;
	result.Right.o = o;
	
	return result;
}

void Camera::getRay(const kfloat &ScreenX, const kfloat &ScreenY, Point3D &RayOrigin, Vector3D &RayDirection)
{
	SetupNodeIfNeeded();
	
	float width , height;
	getRenderingScreen()->GetSize(width , height); 
	float aspect = (width*myViewportSizeX) / (height*myViewportSizeY);

	float screenPosInCameraX = ((ScreenX - myViewportMinX) / myViewportSizeX) - 0.5f;
	float screenPosInCameraY = ((ScreenY - myViewportMinY) / myViewportSizeY) - 0.5f;
		
	float tt = 2.0f*tan(((float)myVerticalFOV)*DEG_TO_RAD_2);
	RayDirection.x = 1.0f;
	RayDirection.z = -screenPosInCameraY*tt;
	RayDirection.y = -screenPosInCameraX*tt*aspect;
	
	RayOrigin.x = GetLocalToGlobal().e[3][0];
	RayOrigin.y = GetLocalToGlobal().e[3][1];
	RayOrigin.z = GetLocalToGlobal().e[3][2];
	GetLocalToGlobal().TransformVector(&RayDirection);
	RayDirection.Normalize();
}


// Pt parameter is in global coordinates
bool Camera::Project(kfloat &ScreenX, kfloat &ScreenY, Point3D Pt)
{
	
	SetupNodeIfNeeded();
	
	kfloat width, height;
	getRenderingScreen()->GetSize(width, height);
	
	kfloat aspect;
	
	if (myAspectRatio != 0)
		aspect = myAspectRatio;
	else
		aspect = (width*myViewportSizeX) / (height*myViewportSizeY);
	
	
	// projection (default is perspective)
	kfloat frustumHeight = (kfloat)tanf(myVerticalFOV * fPI / 360.0f) * myNear;
	kfloat frustumWidth = frustumHeight * aspect;
	
	kfloat deltaX = frustumWidth + frustumWidth;
	kfloat deltaY = frustumHeight + frustumHeight;
	kfloat deltaZ = myFar - myNear;
	Matrix4x4 frust;
	
	if ((myNear <= 0.0f) || (myFar <= 0.0f) || (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f))
	{
		KIGS_ERROR("Invalid frustrum", 1);
		return false;
	}
	
	// can be optimized a lot
	frust[0] = 2.0f * myNear / deltaX;
	frust[4] = frust[8] = frust[12] = 0.0f;
	
	frust[5] = 2.0f * myNear / deltaY;
	frust[1] = frust[9] = frust[13] = 0.0f;
	
	frust[2] = 0.0f;
	frust[6] = 0.0f;
	frust[10] = -(myNear + myFar) / deltaZ;
	frust[14] = -1.0f;
	
	frust[11] = -2.0f * myNear * myFar / deltaZ;
	frust[3] = frust[7] = frust[15] = 0.0f;
	
	
	// view vector is on x axis in local camera coordinates
	// so if Pt is just in front of the camera, localPt.x > 0
	// if Pt is above the camera, localPt.z > 0	
	Point3D localPt;
	GetGlobalToLocal().TransformPoint(&Pt, &localPt);

	Vector4D result;
	result.x = frust[0] * localPt.y + frust[1] * localPt.z + frust[2] * localPt.x + frust[3];
	result.y = frust[4] * localPt.y + frust[5] * localPt.z + frust[6] * localPt.x + frust[7];
	result.z = frust[8] * localPt.y + frust[9] * localPt.z + frust[10] * localPt.x + frust[11];
	result.w = frust[12] * localPt.y + frust[13] * localPt.z + frust[14] * localPt.x + frust[15];
	
	if (result.w)
		result.w = 1.0f / result.w;
	else
		return false;
	
	result.x *= result.w;
	result.y *= result.w;
	result.z *= result.w;
	
	ScreenX = ((result.x * 0.5f + 0.5f)*myViewportSizeX + myViewportMinX) * width;
	ScreenY = ((result.y * 0.5f + 0.5f)*myViewportSizeY + myViewportMinY) * height;
	
	return (result.z > 0.0f);
}


// always visible
DECLARE_DECORABLE_IMPLEMENT(bool, Cull, Camera, TravState* state, unsigned int cullingMask)
//bool    Camera::Cull(TravState* state,unsigned int cullingMask)
{
	if(!Node3D::Implement_Cull(state,cullingMask))
	{
		myIsVisible=state->GetVisibilityFrame();
	}
	return true;
}

//#define SHOW_DEBUG
#ifdef SHOW_DEBUG
#include "GLSLDebugDraw.h"
#endif

//#include "TravState.h"
bool	Camera::Draw(TravState* state)
{
	
#ifdef SHOW_DEBUG
	int activate = -1;
	if (getValue("DebugVisibleIndex", activate) && activate==1)
	{
		Point3D outP;
		Vector3D outV;
		const Matrix3x4& lMat = myFatherNode->GetLocalToGlobal();
		Point3D* PosOffset = (Point3D*)myPosition.getVector();
		lMat.TransformPoint(PosOffset, &outP);
		lMat.TransformVector(&GetViewVector(), &outV);
		
		dd::sphere(outP, Vector3D(0, 1, 0), 0.08);
		dd::line(outP, outP + outV, Vector3D(0, 1, 0));
	}
#endif
	return Node3D::Draw(state);
}

void	Camera::activeTouchControlledCamera(bool active)
{
	ModuleInput* theInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");

	if (theInputModule == 0)
	{
		return;
	}

	if (active)
	{
		theInputModule->getTouchManager()->registerEvent(this, "ManageScrollTouchEvent", Scroll, EmptyFlag);
		theInputModule->getTouchManager()->registerEvent(this, "ManagePinchTouchEvent", Pinch, EmptyFlag);
		
		// add dynamic attribute with touch controlled data
		// first check if not already there
		if (getAttribute("TouchControlledData") == 0)
		{
			AddDynamicAttribute(ATTRIBUTE_TYPE::RAWPTR, "TouchControlledData");
			touchControlledDataStruct* touchData = new touchControlledDataStruct();
			setValue("TouchControlledData", touchData);
		}

		if (getAttribute("TargetPointDist") == 0) // attribute can be added via xml
		{
			AddDynamicAttribute(ATTRIBUTE_TYPE::FLOAT, "TargetPointDist", 250.0f);
		}

	}
	else
	{
		theInputModule->getTouchManager()->unregisterEvent(this, Scroll);
		theInputModule->getTouchManager()->unregisterEvent(this, Pinch);
		// remove dynamic attribute 
		// first delete data struct if already set
		void* datastruct;
		if (getValue("TouchControlledData", datastruct))
		{
			if (datastruct)
			{
				delete(touchControlledDataStruct*)datastruct;
			}
		}

		RemoveDynamicAttribute("TouchControlledData");
	}
}

bool Camera::ManagePinchTouchEvent(PinchEvent& pinch_event)
{
	touchControlledDataStruct*	currentDataStruct = 0;

	void* datastruct;
	if (getValue("TouchControlledData", datastruct))
	{
		currentDataStruct = (touchControlledDataStruct*)datastruct;
	}

	if (currentDataStruct)
	{

		// if already in scroll, return

		if (currentDataStruct->state == 1)
		{
			return false;
		}

		
		float dwidth, dheight;
		getRenderingScreen()->GetDesignSize(dwidth, dheight);

		

		if (pinch_event.state == StatePossible)
		{
			Point2D currentPos = pinch_event.p1_start.xy;
			currentPos += pinch_event.p2_start.xy;
			currentPos *= 0.5f;
			currentPos.x /= dwidth;
			currentPos.y /= dheight;

			if ((currentPos.x >= 0.0f) && (currentPos.x < myViewportSizeX))
				if ((currentPos.y >= 0.0f) && (currentPos.y < myViewportSizeY))
				{
					*pinch_event.swallow_mask = 0xFFFFFFFF;
					return true;
				}

			return false;
		}
		if (pinch_event.state == StateBegan)
		{
			currentDataStruct->state = 2;
			currentDataStruct->targetPointDist = getValue<kfloat>("TargetPointDist");

			currentDataStruct->startV = ((Point3D)myViewVector);
			currentDataStruct->startV *= currentDataStruct->targetPointDist;
			currentDataStruct->startV += (Point3D)myPosition;
		}

		if (pinch_event.state == StateChanged)
		{
			kfloat currentDist = Norm(pinch_event.p1 - pinch_event.p2);

			if (currentDist > 1.0f)
			{
				currentDataStruct->OneOnCoef = Norm(pinch_event.p1_start - pinch_event.p2_start)/ currentDist;
			}
			Point3D newPosition((Point3D)myViewVector);

			newPosition *= -currentDataStruct->targetPointDist*currentDataStruct->OneOnCoef;
			newPosition += currentDataStruct->startV;

			myPosition = newPosition;

			NotifyUpdate(myPosition.getID().toUInt());
		}

		if (pinch_event.state == StateEnded)
		{
			currentDataStruct->state = 0;

			setValue("TargetPointDist", currentDataStruct->targetPointDist*currentDataStruct->OneOnCoef);
		}
		*pinch_event.swallow_mask = 0xFFFFFFFF;
		return true;
		
	}
	return false;
}


bool Camera::ManageScrollTouchEvent(ScrollEvent& scroll_event)
{
	touchControlledDataStruct*	currentDataStruct = 0;

	void* datastruct;
	if (getValue("TouchControlledData", datastruct))
	{
		currentDataStruct = (touchControlledDataStruct*)datastruct;
	}

	if (currentDataStruct)
	{

		// if already in pinch, return

		if (currentDataStruct->state > 1)
		{
			return false;
		}

		float dwidth, dheight;
		getRenderingScreen()->GetDesignSize(dwidth, dheight);

		Point2D currentPos = scroll_event.position.xy;
		currentPos.x /= dwidth;
		currentPos.y /= dheight;

		if (scroll_event.state == StatePossible)
		{
			if((currentPos.x>=0.0f) && (currentPos.x <myViewportSizeX))
				if ((currentPos.y >= 0.0f) && (currentPos.y < myViewportSizeY))
				{
					*scroll_event.swallow_mask = 0xFFFFFFFF;
					return true;
				}

			return false;
		}		


		if (scroll_event.state == StateBegan)
		{

			if (dwidth > dheight)
			{
				currentDataStruct->OneOnCoef = 2.0f / dheight;
			}
			else
			{
				currentDataStruct->OneOnCoef = 2.0f / dwidth;
			}
			Point2D		screenCenter;
			screenCenter.x = dwidth*0.5f;
			screenCenter.y = dheight*0.5f;

			currentDataStruct->startPt = scroll_event.start_position.xy;
			currentDataStruct->startPt -= screenCenter;

			currentDataStruct->startPt *= currentDataStruct->OneOnCoef;

			currentDataStruct->state = 0;
			if (NormSquare(currentDataStruct->startPt) < 1.0f)
			{
				currentDataStruct->state = 1; // rotation

				// sphere ray intersection
				kfloat d2 = (currentDataStruct->startPt.x*currentDataStruct->startPt.x + currentDataStruct->startPt.y*currentDataStruct->startPt.y);
				kfloat thc = 1.0f - d2;
				if (thc > 0.0f)
				{
					thc = sqrtf(thc);
				}
				else
				{
					thc = 0.0f;
				}

				// store start V in "eye" coordinates (depth is on X axis)
				// 
				currentDataStruct->startV.Set(-thc, currentDataStruct->startPt.x, currentDataStruct->startPt.y);

				// store starting camera data

				Vector3D	right;
				right.CrossProduct( (Point3D)myUpVector, (Point3D)myViewVector);
				currentDataStruct->StartMatrix.Set((Point3D)myViewVector, right, (Point3D)myUpVector, (Point3D)myPosition);
				currentDataStruct->targetPointDist = getValue<kfloat>("TargetPointDist");
			}

		}

		if (scroll_event.state == StateChanged)
		{
			if (currentDataStruct->state == 1) // rotation
			{
				// sphere ray intersection at current position
				Point2D		screenCenter;
				screenCenter.x = dwidth*0.5f;
				screenCenter.y = dheight*0.5f;

				Point2D currentPos;
				currentPos = scroll_event.position.xy;
				currentPos -= screenCenter;
				currentPos *= currentDataStruct->OneOnCoef;

				if (NormSquare(currentPos) > 1.0f)
				{
					currentPos.Normalize();
				}
				kfloat d2 = (currentPos.x*currentPos.x + currentPos.y*currentPos.y);
				kfloat thc = 1.0f - d2;
				if (thc > 0.0f)
				{
					thc = sqrtf(thc);
				}
				else
				{
					thc = 0.0f;
				}
				

				Vector3D currentV;

				// store currentV in "eye" coordinates (depth is on X axis)
				currentV.Set(-thc, currentPos.x, currentPos.y );


				// find normal vector to both currentV & currentDataStruct->startV

				Vector3D	normalV;
				normalV.CrossProduct(currentDataStruct->startV, currentV);
				
				normalV.Normalize();

				currentDataStruct->StartMatrix.TransformVector(&normalV);

				// quaternion transforming startV to currentV
				Vector3D startV = currentDataStruct->startV;

				currentDataStruct->StartMatrix.TransformVector(&startV);
				currentDataStruct->StartMatrix.TransformVector(&currentV);

				Quaternion	q = RotationArc(currentV, startV);

				Matrix3x3	rotate(q);

				Vector3D	rview = rotate*currentDataStruct->StartMatrix.XAxis;

				myViewVector = rview;

				Vector3D	rup = rotate*currentDataStruct->StartMatrix.ZAxis;

				myUpVector = rup;

				Point3D	rotationCenterPos(currentDataStruct->StartMatrix.Pos);
				rotationCenterPos += currentDataStruct->StartMatrix.XAxis*currentDataStruct->targetPointDist;

				Vector3D	posRotation(rotationCenterPos, currentDataStruct->StartMatrix.Pos,  asVector());

				posRotation = rotate*posRotation;

				posRotation += rotationCenterPos;

				myPosition = posRotation;

				NotifyUpdate(myUpVector.getID().toUInt());
				
			}
		}

		if (scroll_event.state == StateEnded)
		{
			currentDataStruct->state = 0;
		}
		*scroll_event.swallow_mask = 0xFFFFFFFF;
		

		return true;
	}

	
	return false;
}


void Camera::ChangeMatrix(const Matrix3x4& m)
{
	myViewVector = m.XAxis;
	myUpVector = m.ZAxis;
	myPosition = m.Pos;
	RecomputeMatrix();
}