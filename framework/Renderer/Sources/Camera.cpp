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
	v3f  view,up,right,pos;
	
	view = normalize(mViewVector);
	pos = mPosition;
	
	up = normalize(mUpVector);
	
	right=normalize(cross(up,view));
	
	up=cross(view,right);

	mat4 matrix(glm::vec4(view,0.0f), glm::vec4(right,0.0f), glm::vec4(up,0.0f), glm::vec4(pos,1.0f));
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

		v2f pos2D;

		pos2D.x = posin.pos.x / dwidth;
		pos2D.y = posin.pos.y / dheight;

		// pos in [-0.5,+0.5]
		pos2D.x = ((pos2D.x - mViewportMinX) / mViewportSizeX) - 0.5f;
		pos2D.y = ((pos2D.y - mViewportMinY) / mViewportSizeY) - 0.5f;
		

		float tt = 2.0f*tan(((float)mVerticalFOV)*DEG_TO_RAD_2);
		pout.dir.x = 1.0f;
		pout.dir.z = -pos2D.y*tt;
		pout.dir.y = -pos2D.x*tt*aspect;
		pout.dir = transformVector3(GetLocalToGlobal() ,pout.dir);
		pout.dir = normalize(pout.dir);

		pout.origin.x = GetLocalToGlobal()[3][0];
		pout.origin.y = GetLocalToGlobal()[3][1];
		pout.origin.z = GetLocalToGlobal()[3][2];

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
	v3f  n;
	v3f   o;
	
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
	
	n = { 1.0f,0.0f,0.0f };
	o = { mNearPlane,0.0f,0.0f };
	n=transformVector3(l2g,n);
	o = transformPoint3(l2g,o);
	obj->InitPlane(0,n,o);
	
	// far plane
	n = { -1.0f, 0.0f, 0.0f };
	o = { mFarPlane,0.0f,0.0f };
	n = transformVector3(l2g ,n);
	o = transformPoint3(l2g ,o);
	obj->InitPlane(1,n,o);
	
	// down plane
	float s=sinf((float)mVerticalFOV*0.5f*DEG_TO_RAD);
	float c=cosf((float)mVerticalFOV*0.5f*DEG_TO_RAD);
	
	n = { s,0.0f,c };
	o = { 0.0f,0.0f,0.0f };
	n = transformVector3(l2g, n);
	o = transformPoint3(l2g, o);
	obj->InitPlane(2,n,o);
	
	// up plane
	n = { s,0.0f,-c };
	n = transformVector3(l2g, n);
	obj->InitPlane(3,n,o);
	
	// left plane
	//s = sinf((float)mVerticalFOV*aspect*0.5f*DEG_TO_RAD);
	s = sinf(hfov*0.5f);
	//c = cosf((float)mVerticalFOV*aspect*0.5f*DEG_TO_RAD);
	c = cosf(hfov*0.5f);
	
	n = { s,-c,0.0f };
	n = transformVector3(l2g, n);
	obj->InitPlane(4,n,o);
	
	// right plane
	n = { s,c,0.0f };
	n = transformVector3(l2g, n);
	obj->InitPlane(5,n,o);
}

Camera::FrustumPlanes Camera::GetFrustum()
{
	FrustumPlanes result;

	v3f o;
	v3f n;

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
	n = { 1.0f, 0.0f, 0.0f };
	o = { mNearPlane, 0.0f, 0.0f };
	n = transformVector3(l2g, n);
	o = transformPoint3(l2g, o);

	result.Near.n = n;
	result.Near.o = o;

	// far plane
	n = { -1.0f, 0.0f, 0.0f };
	o = { mFarPlane, 0.0f, 0.0f };
	n = transformVector3(l2g, n);
	o = transformPoint3(l2g, o);
	result.Far.n = n;
	result.Far.o = o;

	// down plane
	float s = sinf((float)mVerticalFOV*0.5f*DEG_TO_RAD);
	float c = cosf((float)mVerticalFOV*0.5f*DEG_TO_RAD);

	n = { s, 0.0f, c };
	o = { 0.0f, 0.0f, 0.0f };
	n = transformVector3(l2g, n);
	o = transformPoint3(l2g, o);
	result.Down.n = n;
	result.Down.o = o;

	// up plane
	n = { s, 0.0f, -c };
	n = transformVector3(l2g, n);
	result.Up.n = n;
	result.Up.o = o;

	// left plane
	//s = sinf((float)mVerticalFOV*aspect*0.5f*DEG_TO_RAD);
	s = sinf(hfov*0.5f);
	//c = cosf((float)mVerticalFOV*aspect*0.5f*DEG_TO_RAD);
	c = cosf(hfov*0.5f);

	n = { s, -c, 0.0f };
	n = transformVector3(l2g, n);
	result.Left.n = n;
	result.Left.o = o;

	// right plane
	n = { s, c, 0.0f };
	n = transformVector3(l2g, n);
	result.Right.n = n;
	result.Right.o = o;
	
	return result;
}

void Camera::getRay(const float &ScreenX, const float &ScreenY, v3f &RayOrigin, v3f &RayDirection)
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
	
	RayOrigin.x = GetLocalToGlobal()[3][0];
	RayOrigin.y = GetLocalToGlobal()[3][1];
	RayOrigin.z = GetLocalToGlobal()[3][2];
	RayDirection = transformVector3(GetLocalToGlobal(), RayDirection);

	RayDirection=normalize(RayDirection);
}


// Pt parameter is in global coordinates
bool Camera::Project(float &ScreenX, float &ScreenY, v3f Pt)
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
	float frustumHeight = (float)tanf(mVerticalFOV * DEG_TO_RAD_2) * mNearPlane;
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
	v3f localPt = Pt;
	localPt = transformPoint3(GetGlobalToLocal(), localPt);

	v4f result;

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
		v3f outP;
		v3f outV;
		const mat4& lMat = mFatherNode->GetLocalToGlobal();
		v3f* PosOffset = (v3f*)mPosition.getVector();
		lMat.TransformPoint(PosOffset, &outP);
		lMat.TransformVector(&GetViewVector(), &outV);
		
		dd::sphere(outP, v3f(0, 1, 0), 0.08);
		dd::line(outP, outP + outV, v3f(0, 1, 0));
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
			v2f currentPos = pinch_event.p1_start;
			currentPos += v2f(pinch_event.p2_start);
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

			currentDataStruct->mStartV = ((v3f)mViewVector);
			currentDataStruct->mStartV *= currentDataStruct->mTargetPointDist;
			currentDataStruct->mStartV += (v3f)mPosition;
		}

		if (pinch_event.state == Input::StateChanged)
		{
			float currentDist = length(pinch_event.p1 - pinch_event.p2);

			if (currentDist > 1.0f)
			{
				currentDataStruct->mOneOnCoef = length(pinch_event.p1_start - pinch_event.p2_start)/ currentDist;
			}
			v3f newPosition((v3f)mViewVector);

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

		v2f currentPos = scroll_event.position;
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
			v2f		screenCenter;
			screenCenter.x = dwidth*0.5f;
			screenCenter.y = dheight*0.5f;

			currentDataStruct->mStartPt = scroll_event.start_position;
			currentDataStruct->mStartPt -= screenCenter;

			currentDataStruct->mStartPt *= currentDataStruct->mOneOnCoef;

			currentDataStruct->mState = 0;
			if (length2(currentDataStruct->mStartPt) < 1.0f)
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
				currentDataStruct->mStartV = { -thc, currentDataStruct->mStartPt.x, currentDataStruct->mStartPt.y };

				// store starting camera data

				v3f	right;
				right=cross( (v3f)mUpVector, (v3f)mViewVector);
				currentDataStruct->mStartMatrix = mat4(0.0f);
				currentDataStruct->mStartMatrix[0] = v4f(mViewVector,0.0f);
				currentDataStruct->mStartMatrix[1] = v4f(right, 0.0f);
				currentDataStruct->mStartMatrix[2] = v4f(mUpVector, 0.0f);
				currentDataStruct->mStartMatrix[3] = v4f(mPosition, 1.0f);

				currentDataStruct->mTargetPointDist = getValue<float>("TargetPointDist");
			}

		}

		if (scroll_event.state == Input::StateChanged)
		{
			if (currentDataStruct->mState == 1) // rotation
			{
				// sphere ray intersection at current position
				v2f		screenCenter;
				screenCenter.x = dwidth*0.5f;
				screenCenter.y = dheight*0.5f;

				v2f currentPos;
				currentPos = scroll_event.position;
				currentPos -= screenCenter;
				currentPos *= currentDataStruct->mOneOnCoef;

				if (length2(currentPos) > 1.0f)
				{
					currentPos = normalize(currentPos);
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
				

				v3f currentV;

				// store currentV in "eye" coordinates (depth is on X axis)
				currentV = { -thc, currentPos.x, currentPos.y };


				// find normal vector to both currentV & currentDataStruct->mStartV

				v3f	normalV;
				normalV=normalize(cross(currentDataStruct->mStartV, currentV));
	
				normalV = transformVector3(currentDataStruct->mStartMatrix,normalV);

				// quaternion transforming mStartV to currentV
				v3f startV = currentDataStruct->mStartV;

				startV = transformVector3(currentDataStruct->mStartMatrix,startV);
				currentV = transformVector3(currentDataStruct->mStartMatrix,currentV);

				
				v3f a = cross(currentV, startV);
				float w = sqrtf(length2(currentV) * length2(startV)) + dot(currentV, startV);
				quat	q(a.x,a.y,a.z, w);
				
				q=normalize(q);

				mat3	rotate(q);

				v3f	rview = rotate * currentDataStruct->mStartMatrix[0];

				mViewVector = rview;

				v3f	rup = rotate*currentDataStruct->mStartMatrix[2];

				mUpVector = rup;

				v3f	rotationCenterPos(currentDataStruct->mStartMatrix[3]);
				rotationCenterPos += currentDataStruct->mStartMatrix[0] *currentDataStruct->mTargetPointDist;

				v3f	posRotation(v3f(currentDataStruct->mStartMatrix[3]) - rotationCenterPos);

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


void Camera::ChangeMatrix(const mat4& m)
{
	mViewVector = m[0];
	mUpVector = m[2];
	mPosition = m[3];
	RecomputeMatrix();
}