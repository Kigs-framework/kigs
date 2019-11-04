#include "Platform/Main/BaseApp.h"
#include "Core.h"
#include "CoreBaseApplication.h"


//using namespace winrt::Windows::UI::Input;
//using namespace winrt::Windows::Graphics::Holographic;

extern bool gIsVR;

__declspec(dllimport) void AngleSetNearFar(float NearPlane, float FarPlane);

void OpenGLCamera::NotifyUpdate(unsigned int labelid)
{
	if (labelid == myNear.getID() || labelid == myFar.getID())
	{
		myNeedNearFarUpdate = true;
	}
	ParentClassType::NotifyUpdate(labelid);
}

void OpenGLCamera::PlatformProtectedSetActive(TravState* state)
{
	if (!KigsCore::GetCoreApplication()->IsHolographic())
		return;
	
	if (!getRenderingScreen()->IsHolographic())
		return;

	if (myNeedNearFarUpdate)
	{
		AngleSetNearFar(myNear, myFar);
		myNeedNearFarUpdate = false;
	}
	

	float m[16];
	glGetFloatv(GLEXT_HOLOGRAPHIC_MONO_VIEW_MATRIX_ANGLE, m);

	if (isnan(m[12]))
		return;

	Vector3D v(m[0], m[1], m[2]);
	Vector3D u(m[4], m[5], m[6]);
	Vector3D w(m[8], m[9], m[10]);
	Vector3D t(m[12], m[13], m[14]);

	// Right
	// 
	//
	// Translation
	Matrix3x4 view(
		v.x, u.x, w.x, 
		v.y, u.y, w.y,  
		v.z, u.z, w.z, 
		-Dot(v,t), -Dot(u, t), -Dot(w, t));

	myViewVector = -view.ZAxis;
	myUpVector = view.YAxis;
	myPosition = view.Pos;
	RecomputeMatrix();

	// always clear color on holographic mode
	myClearColorBuffer = true;	

	// hardware value (should be near 17.0f)
	if(gIsVR)
		myVerticalFOV = 110.0f;
	else
		myVerticalFOV = 17.0f;
}

void OpenGLCamera::PlatformProtectedRelease(TravState* state)
{
}