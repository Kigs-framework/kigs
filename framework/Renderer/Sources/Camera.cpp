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

using namespace Kigs::Draw;

#define DEG_TO_RAD    (3.1415926535897932384626433832795f/180.0f)
#define DEG_TO_RAD_2  (3.1415926535897932384626433832795f/360.0f)

IMPLEMENT_CLASS_INFO(Camera)

IMPLEMENT_CONSTRUCTOR(Camera)
, mIsActive(false)
{
	setInitParameter("RenderingScreen", true);
	SetPosition(0.0f, 0.0f, 0.0f);
	SetViewVector(1.0f, 0.0f, 0.0f);
	SetUpVector(0.0f, 0.0f, 1.0f);
	SetClearColor(0.8f, 0.8f, 1.0f);
	
	RenderPass opaque_pass;
	opaque_pass.pass_mask = 1;
	opaque_pass.name = "Opaque Pass";
	mRenderPasses.push_back(opaque_pass);

	RenderPass transparent_pass;
	transparent_pass.pass_mask = 2;
	transparent_pass.use_front_to_back_sorter = true;
	transparent_pass.allow_instancing = false;
	transparent_pass.name = "Transparent Pass";
	mRenderPasses.push_back(transparent_pass);
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

Camera::~Camera()
{
	void* datastruct;
	if (getValue("TouchControlledData", datastruct))
	{
		if (datastruct)
		{
			delete (touchControlledDataStruct*)datastruct;
		}
	}

	// notify scenegraph that I am dead
	ModuleSceneGraph* scenegraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);
	scenegraph->NotifyDefferedItemDeath(this);

	auto theInputModule = KigsCore::GetModule<Input::ModuleInput>();
	if(theInputModule)
		theInputModule->getTouchManager()->removeTouchSupport(this);
}

void Camera::NotifyUpdate(const unsigned int  labelid)
{
	Node3D::NotifyUpdate(labelid);
	if (labelid == KigsID("TouchControlled")._id)
	{
		activeTouchControlledCamera(mTouchControlled);
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
	
	view.x=mViewVector[0];
	view.y=mViewVector[1];
	view.z=mViewVector[2];
	
	view.Normalize();
	
	pos.x=mPosition[0];
	pos.y=mPosition[1];
	pos.z=mPosition[2];
	
	
	up.x=mUpVector[0];
	up.y=mUpVector[1];
	up.z=mUpVector[2];
	
	up.Normalize();
	
	right=up^view;
	
	right.Normalize();
	
	up=view^right;

	mat3x4 matrix(view, right, up, pos);
	ParentClassType::ChangeMatrix(matrix);
}

bool Camera::SetActive(TravState* state)
{	
	mIsActive=true;
	bool b = ProtectedSetActive(state);
	if (b)
	{
		getRenderingScreen()->setValue("Brightness", mBrightness);
	}
	KigsCore::GetNotificationCenter()->postNotificationName("OnCamera_SetActive", this);
	return b;
}

void Camera::Release(TravState* state)
{	
	mIsActive=false;
	ProtectedRelease(state);
	KigsCore::GetNotificationCenter()->postNotificationName("OnCamera_Release", this);
}

void Camera::InitModifiable()
{
	Node3D::InitModifiable();
	if(IsInit())
	{
		CMSP	screen;
		getValue("RenderingScreen", screen);
		if(screen)
		{
			RecomputeMatrix();
			//! notification on matrix change

			setOwnerNotification("Position", true);
			setOwnerNotification("UpVector", true);
			setOwnerNotification("ViewVector", true);
			setOwnerNotification("ViewportSizeX", true);
			setOwnerNotification("ViewportSizeY", true);
			setOwnerNotification("TouchControlled", true);

			// declare as a touch support potential target with rendering screen as parent
			auto theInputModule = KigsCore::GetModule<Input::ModuleInput>();
			if(theInputModule)
				theInputModule->getTouchManager()->addTouchSupport(this, getRenderingScreen());
			

			activeTouchControlledCamera(mTouchControlled);
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
bool Camera::GetDataInTouchSupport(const Input::touchPosInfos& posin, Input::touchPosInfos& pout)
{
	// TODO : probably not enough as we can have several camera in a scene and not all attached to holographic view 
	if (getRenderingScreen()->IsHolographic())
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
		float aspect = (width*mViewportSizeX) / (height*mViewportSizeY);

		float dwidth, dheight;
		getRenderingScreen()->GetDesignSize(dwidth, dheight);
		// size or design size here ?

		Point2D pos2D;

		pos2D.x = posin.pos.x / dwidth;
		pos2D.y = posin.pos.y / dheight;

		// pos in [-0.5,+0.5]
		pos2D.x = ((pos2D.x - mViewportMinX) / mViewportSizeX) - 0.5f;
		pos2D.y = ((pos2D.y - mViewportMinY) / mViewportSizeY) - 0.5f;
		

		float tt = 2.0f*tan(((float)mVerticalFOV)*DEG_TO_RAD_2);
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
		pout.pos.x = (pos2D.x + 0.5f)*mViewportSizeX*dwidth;
		pout.pos.y = (pos2D.y + 0.5f)*mViewportSizeY*dheight;
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
	
	float width , height;
	getRenderingScreen()->GetSize(width , height);
	
	
	float screenRotation;
	getRenderingScreen()->GetRotation(screenRotation);
	
	
	float aspect = (width*mViewportSizeX) / (height*mViewportSizeY);
	if((screenRotation==90.0f)||(screenRotation==270.0f))
		aspect=1.0f/aspect;
	
	float d = 2.0f*tanf(mVerticalFOV*DEG_TO_RAD*0.5f);
	float hfov = 2.0f*atanf(d*aspect*0.5f);
	
	auto l2g = GetLocalToGlobal();

	// near plane
	n.Set(1.0f,0.0f,0.0f);
	o.Set(mNearPlane,0.0f,0.0f);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(0,n,o);
	
	// far plane
	n.Set(-1.0f,0.0f,0.0f);
	o.Set(mFarPlane,0.0f,0.0f);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(1,n,o);
	
	// down plane
	float s=sinf((float)mVerticalFOV*0.5f*DEG_TO_RAD);
	float c=cosf((float)mVerticalFOV*0.5f*DEG_TO_RAD);
	
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
	//s = sinf((float)mVerticalFOV*aspect*0.5f*DEG_TO_RAD);
	s = sinf(hfov*0.5f);
	//c = cosf((float)mVerticalFOV*aspect*0.5f*DEG_TO_RAD);
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

	float width, height;
	getRenderingScreen()->GetSize(width, height);


	float screenRotation;
	getRenderingScreen()->GetRotation(screenRotation);


	float aspect = (width*mViewportSizeX) / (height*mViewportSizeY);
	if ((screenRotation == 90.0f) || (screenRotation == 270.0f))
		aspect = 1.0f / aspect;

	float d = 2.0f*tanf(mVerticalFOV*DEG_TO_RAD*0.5f);
	float hfov = 2.0f*atanf(d*aspect*0.5f);

	auto l2g = GetLocalToGlobal();

	// near plane
	n.Set(1.0f, 0.0f, 0.0f);
	o.Set(mNearPlane, 0.0f, 0.0f);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o, 1);

	result.Near.n = n;
	result.Near.o = o;

	// far plane
	n.Set(-1.0f, 0.0f, 0.0f);
	o.Set(mFarPlane, 0.0f, 0.0f);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o, 1);
	result.Far.n = n;
	result.Far.o = o;

	// down plane
	float s = sinf((float)mVerticalFOV*0.5f*DEG_TO_RAD);
	float c = cosf((float)mVerticalFOV*0.5f*DEG_TO_RAD);

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
	//s = sinf((float)mVerticalFOV*aspect*0.5f*DEG_TO_RAD);
	s = sinf(hfov*0.5f);
	//c = cosf((float)mVerticalFOV*aspect*0.5f*DEG_TO_RAD);
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

void Camera::getRay(const float &ScreenX, const float &ScreenY, Point3D &RayOrigin, Vector3D &RayDirection)
{
	SetupNodeIfNeeded();
	
	float width , height;
	getRenderingScreen()->GetSize(width , height); 
	float aspect = (width*mViewportSizeX) / (height*mViewportSizeY);

	float screenPosInCameraX = ((ScreenX - mViewportMinX) / mViewportSizeX) - 0.5f;
	float screenPosInCameraY = ((ScreenY - mViewportMinY) / mViewportSizeY) - 0.5f;
		
	float tt = 2.0f*tan(((float)mVerticalFOV)*DEG_TO_RAD_2);
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
bool Camera::Project(float &ScreenX, float &ScreenY, Point3D Pt)
{
	
	SetupNodeIfNeeded();
	
	float width, height;
	getRenderingScreen()->GetSize(width, height);
	
	float aspect;
	
	if (mAspectRatio != 0)
		aspect = mAspectRatio;
	else
		aspect = (width*mViewportSizeX) / (height*mViewportSizeY);
	
	
	// projection (default is perspective)
	float frustumHeight = (float)tanf(mVerticalFOV * fPI / 360.0f) * mNearPlane;
	float frustumWidth = frustumHeight * aspect;
	
	float deltaX = frustumWidth + frustumWidth;
	float deltaY = frustumHeight + frustumHeight;
	float deltaZ = mFarPlane - mNearPlane;
	mat4 frust;
	
	if ((mNearPlane <= 0.0f) || (mFarPlane <= 0.0f) || (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f))
	{
		KIGS_ERROR("Invalid frustrum", 1);
		return false;
	}
	
	/*
	frust[0] = 2.0f * mNearPlane / deltaX;
	frust[4] = frust[8] = frust[12] = 0.0f;
	
	frust[5] = 2.0f * mNearPlane / deltaY;
	frust[1] = frust[9] = frust[13] = 0.0f;
	
	frust[2] = 0.0f;
	frust[6] = 0.0f;
	frust[10] = -(mNearPlane + mFarPlane) / deltaZ;
	frust[14] = -1.0f;
	
	frust[11] = -2.0f * mNearPlane * mFarPlane / deltaZ;
	frust[3] = frust[7] = frust[15] = 0.0f;
	*/

	// can be optimized a lot
	frust[0][0] = 2.0f * mNearPlane / deltaX;
	frust[1][0] = frust[2][0] = frust[3][0] = 0.0f;

	frust[1][1] = 2.0f * mNearPlane / deltaY;
	frust[0][1] = frust[2][1] = frust[3][1] = 0.0f;

	frust[0][2] = 0.0f;
	frust[1][2] = 0.0f;
	frust[2][2] = -(mNearPlane + mFarPlane) / deltaZ;
	frust[3][2] = -1.0f;

	frust[2][3] = -2.0f * mNearPlane * mFarPlane / deltaZ;
	frust[0][3] = frust[1][3] = frust[3][3] = 0.0f;
	
	
	// view vector is on x axis in local camera coordinates
	// so if Pt is just in front of the camera, localPt.x > 0
	// if Pt is above the camera, localPt.z > 0	
	Point3D localPt;
	GetGlobalToLocal().TransformPoint(&Pt, &localPt);

	Vector4D result;

	/*
	result.x = frust[0] * localPt.y + frust[1] * localPt.z + frust[2] * localPt.x + frust[3];
	result.y = frust[4] * localPt.y + frust[5] * localPt.z + frust[6] * localPt.x + frust[7];
	result.z = frust[8] * localPt.y + frust[9] * localPt.z + frust[10] * localPt.x + frust[11];
	result.w = frust[12] * localPt.y + frust[13] * localPt.z + frust[14] * localPt.x + frust[15];
	*/

	result.x = frust[0][0] * localPt.y + frust[0][1] * localPt.z + frust[0][2] * localPt.x + frust[0][3];
	result.y = frust[1][0] * localPt.y + frust[1][1] * localPt.z + frust[1][2] * localPt.x + frust[1][3];
	result.z = frust[2][0] * localPt.y + frust[2][1] * localPt.z + frust[2][2] * localPt.x + frust[2][3];
	result.w = frust[3][0] * localPt.y + frust[3][1] * localPt.z + frust[3][2] * localPt.x + frust[3][3];
	
	if (result.w)
		result.w = 1.0f / result.w;
	else
		return false;
	
	result.x *= result.w;
	result.y *= result.w;
	result.z *= result.w;
	
	ScreenX = ((result.x * 0.5f + 0.5f)*mViewportSizeX + mViewportMinX) * width;
	ScreenY = ((result.y * 0.5f + 0.5f)*mViewportSizeY + mViewportMinY) * height;
	
	return (result.z > 0.0f);
}


// always visible
bool Camera::Cull( TravState* state, unsigned int cullingMask)
{
	if(!Node3D::Cull(state,cullingMask))
	{
		mIsVisible=state->GetVisibilityFrame();
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
		const mat3x4& lMat = mFatherNode->GetLocalToGlobal();
		Point3D* PosOffset = (Point3D*)mPosition.getVector();
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
	auto theInputModule = KigsCore::GetModule<Input::ModuleInput>();

	if (theInputModule == 0)
	{
		return;
	}

	if (active)
	{
		theInputModule->getTouchManager()->registerEvent(this, "ManageScrollTouchEvent", Input::Scroll, Input::EmptyFlag);
		theInputModule->getTouchManager()->registerEvent(this, "ManagePinchTouchEvent", Input::Pinch, Input::EmptyFlag);
		
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
		theInputModule->getTouchManager()->unregisterEvent(this, Input::Scroll);
		theInputModule->getTouchManager()->unregisterEvent(this, Input::Pinch);
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

bool Camera::ManagePinchTouchEvent(Input::PinchEvent& pinch_event)
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

		if (currentDataStruct->mState == 1)
		{
			return false;
		}

		
		float dwidth, dheight;
		getRenderingScreen()->GetDesignSize(dwidth, dheight);

		

		if (pinch_event.state == Input::StatePossible)
		{
			Point2D currentPos = pinch_event.p1_start.xy;
			currentPos += pinch_event.p2_start.xy;
			currentPos *= 0.5f;
			currentPos.x /= dwidth;
			currentPos.y /= dheight;

			if ((currentPos.x >= 0.0f) && (currentPos.x < mViewportSizeX))
				if ((currentPos.y >= 0.0f) && (currentPos.y < mViewportSizeY))
				{
					*pinch_event.swallow_mask = 0xFFFFFFFF;
					return true;
				}

			return false;
		}
		if (pinch_event.state == Input::StateBegan)
		{
			currentDataStruct->mState = 2;
			currentDataStruct->mTargetPointDist = getValue<float>("TargetPointDist");

			currentDataStruct->mStartV = ((Point3D)mViewVector);
			currentDataStruct->mStartV *= currentDataStruct->mTargetPointDist;
			currentDataStruct->mStartV += (Point3D)mPosition;
		}

		if (pinch_event.state == Input::StateChanged)
		{
			float currentDist = Norm(pinch_event.p1 - pinch_event.p2);

			if (currentDist > 1.0f)
			{
				currentDataStruct->mOneOnCoef = Norm(pinch_event.p1_start - pinch_event.p2_start)/ currentDist;
			}
			Point3D newPosition((Point3D)mViewVector);

			newPosition *= -currentDataStruct->mTargetPointDist*currentDataStruct->mOneOnCoef;
			newPosition += currentDataStruct->mStartV;

			mPosition = newPosition;

			NotifyUpdate(KigsID("Position")._id);
		}

		if (pinch_event.state == Input::StateEnded)
		{
			currentDataStruct->mState = 0;

			setValue("TargetPointDist", currentDataStruct->mTargetPointDist*currentDataStruct->mOneOnCoef);
		}
		*pinch_event.swallow_mask = 0xFFFFFFFF;
		return true;
		
	}
	return false;
}


bool Camera::ManageScrollTouchEvent(Input::ScrollEvent& scroll_event)
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

		if (currentDataStruct->mState > 1)
		{
			return false;
		}

		float dwidth, dheight;
		getRenderingScreen()->GetDesignSize(dwidth, dheight);

		Point2D currentPos = scroll_event.position.xy;
		currentPos.x /= dwidth;
		currentPos.y /= dheight;

		if (scroll_event.state == Input::StatePossible)
		{
			if((currentPos.x>=0.0f) && (currentPos.x <mViewportSizeX))
				if ((currentPos.y >= 0.0f) && (currentPos.y < mViewportSizeY))
				{
					*scroll_event.swallow_mask = 0xFFFFFFFF;
					return true;
				}

			return false;
		}		


		if (scroll_event.state == Input::StateBegan)
		{

			if (dwidth > dheight)
			{
				currentDataStruct->mOneOnCoef = 2.0f / dheight;
			}
			else
			{
				currentDataStruct->mOneOnCoef = 2.0f / dwidth;
			}
			Point2D		screenCenter;
			screenCenter.x = dwidth*0.5f;
			screenCenter.y = dheight*0.5f;

			currentDataStruct->mStartPt = scroll_event.start_position.xy;
			currentDataStruct->mStartPt -= screenCenter;

			currentDataStruct->mStartPt *= currentDataStruct->mOneOnCoef;

			currentDataStruct->mState = 0;
			if (NormSquare(currentDataStruct->mStartPt) < 1.0f)
			{
				currentDataStruct->mState = 1; // rotation

				// sphere ray intersection
				float d2 = (currentDataStruct->mStartPt.x*currentDataStruct->mStartPt.x + currentDataStruct->mStartPt.y*currentDataStruct->mStartPt.y);
				float thc = 1.0f - d2;
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
				currentDataStruct->mStartV.Set(-thc, currentDataStruct->mStartPt.x, currentDataStruct->mStartPt.y);

				// store starting camera data

				Vector3D	right;
				right.CrossProduct( (Point3D)mUpVector, (Point3D)mViewVector);
				currentDataStruct->mStartMatrix.Set((Point3D)mViewVector, right, (Point3D)mUpVector, (Point3D)mPosition);
				currentDataStruct->mTargetPointDist = getValue<float>("TargetPointDist");
			}

		}

		if (scroll_event.state == Input::StateChanged)
		{
			if (currentDataStruct->mState == 1) // rotation
			{
				// sphere ray intersection at current position
				Point2D		screenCenter;
				screenCenter.x = dwidth*0.5f;
				screenCenter.y = dheight*0.5f;

				Point2D currentPos;
				currentPos = scroll_event.position.xy;
				currentPos -= screenCenter;
				currentPos *= currentDataStruct->mOneOnCoef;

				if (NormSquare(currentPos) > 1.0f)
				{
					currentPos.Normalize();
				}
				float d2 = (currentPos.x*currentPos.x + currentPos.y*currentPos.y);
				float thc = 1.0f - d2;
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


				// find normal vector to both currentV & currentDataStruct->mStartV

				Vector3D	normalV;
				normalV.CrossProduct(currentDataStruct->mStartV, currentV);
				
				normalV.Normalize();

				currentDataStruct->mStartMatrix.TransformVector(&normalV);

				// quaternion transforming mStartV to currentV
				Vector3D startV = currentDataStruct->mStartV;

				currentDataStruct->mStartMatrix.TransformVector(&startV);
				currentDataStruct->mStartMatrix.TransformVector(&currentV);

				quat	q = RotationArc(currentV, startV);

				mat3	rotate(q);

				Vector3D	rview = rotate*currentDataStruct->mStartMatrix.XAxis;

				mViewVector = rview;

				Vector3D	rup = rotate*currentDataStruct->mStartMatrix.ZAxis;

				mUpVector = rup;

				Point3D	rotationCenterPos(currentDataStruct->mStartMatrix.Pos);
				rotationCenterPos += currentDataStruct->mStartMatrix.XAxis*currentDataStruct->mTargetPointDist;

				Vector3D	posRotation(rotationCenterPos, currentDataStruct->mStartMatrix.Pos,  asVector());

				posRotation = rotate*posRotation;

				posRotation += rotationCenterPos;

				mPosition = posRotation;

				NotifyUpdate(KigsID("UpVector")._id);
				
			}
		}

		if (scroll_event.state == Input::StateEnded)
		{
			currentDataStruct->mState = 0;
		}
		*scroll_event.swallow_mask = 0xFFFFFFFF;
		

		return true;
	}

	
	return false;
}


void Camera::ChangeMatrix(const mat3x4& m)
{
	mViewVector = m.XAxis;
	mUpVector = m.ZAxis;
	mPosition = m.Pos;
	RecomputeMatrix();
}