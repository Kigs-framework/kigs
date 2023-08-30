//#include "PrecompiledHeaders.h"

//#include "math.h"
#include "OpenGLCameraOrtho.h"
#include "CullingObject.h"
#include "RendererOpenGL.h"
//#include "TravState.h"
// include after all other kigs include (for iOS)
#include "Platform/Renderer/OpenGLInclude.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(OpenGLCameraOrtho);

OpenGLCameraOrtho::~OpenGLCameraOrtho()
{
}

void  OpenGLCameraOrtho::InitCullingObject(CullingObject* obj)
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

	float aspect;
	if (mAspectRatio!=0)
		aspect = mAspectRatio;
	else
		aspect = (width*mViewportSizeX) / (height*mViewportSizeY); 

	float size2 = mSize;

	auto l2g = GetLocalToGlobal();
	// near plane
	n.Set(1,0,0);
	o.Set(mNearPlane,0,0);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(0,n,o);

	// far plane
	n.Set(-1,0,0);
	o.Set(mFarPlane,0,0);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(1,n,o);
  
	// down plane
	n.Set(0,0,1);
	o.Set(0,0,-size2);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(2,n,o);

	// up plane
	n.Set(0,0,-1);
	o.Set(0,0,size2);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(3,n,o);

	// left plane
	n.Set(0,1,0);
	o.Set(0,-size2*aspect,0);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(4,n,o);

	// right plane
	n.Set(0,-1,0);
	o.Set(0,size2*aspect,0);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(5,n,o);
}

void OpenGLCameraOrtho::getRay(const float &ScreenX, const float &ScreenY, v3f &RayOrigin, v3f &RayDirection)
{
	float width , height;
	getRenderingScreen()->GetSize(width , height); 

	float aspect;
	if (mAspectRatio!=0)
		aspect = mAspectRatio;
	else
		aspect = (width*mViewportSizeX) / (height*mViewportSizeY); 


	SetupNodeIfNeeded();
	RayOrigin.x = 0.0f;
	RayOrigin.y = (ScreenX-0.5f)* mSize*aspect;
	RayOrigin.z = (ScreenY-0.5f)* mSize;
	auto l2g = GetLocalToGlobal();
	l2g.TransformPoints(&RayOrigin,1);

	RayDirection = GetGlobalViewVector();
}

bool	OpenGLCameraOrtho::ProtectedSetActive(TravState* state)
{
	ModuleSpecificRenderer* renderer=state->GetRenderer();
	if(getRenderingScreen())
	{
		if(getRenderingScreen()->SetActive(state))
		{
			renderer->SetClearColorValue(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);

			renderer->SetDepthValueMode(1.0);
			renderer->SetDepthTestMode(true);
			renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_OFF);
			float width, height;
			getRenderingScreen()->GetSize(width, height);

			float aspect;

			if (mAspectRatio != 0)
				aspect = mAspectRatio;
			else
				aspect = (width*mViewportSizeX) / (height*mViewportSizeY);

			renderer->Viewport((int)(mViewportMinX*width), (int)(height - (mViewportSizeY + mViewportMinY)*height), (int)(mViewportSizeX*width), (int)(mViewportSizeY*height));
			renderer->SetScissorValue((int)(mViewportMinX*width), (int)(height - (mViewportSizeY + mViewportMinY)*height), (int)(mViewportSizeX*width), (int)(mViewportSizeY*height));

			float size2=mSize*0.5f;
			renderer->Ortho(MATRIX_MODE_PROJECTION,- size2*aspect, size2*aspect, -size2, size2, mNearPlane, mFarPlane);
			
			auto l2g = GetLocalToGlobal();
			renderer->LookAt(MATRIX_MODE_VIEW,
				l2g.e[3][0], l2g.e[3][1], l2g.e[3][2],
				l2g.e[0][0] + l2g.e[3][0] , l2g.e[0][1] + l2g.e[3][1], l2g.e[0][2] + l2g.e[3][2],
				l2g.e[2][0] , l2g.e[2][1], l2g.e[2][2] ) ;
			renderer->LoadIdentity(MATRIX_MODE_MODEL);

			//	renderer->SetScissorValue(0, 0, (int)width, (int)height);
			renderer->SetScissorTestMode(RENDERER_SCISSOR_TEST_ON);
			renderer->FlushState();

			// CLEAR BUFFER
			int clearMode = RENDERER_CLEAR_NONE;
			if (mClearZBuffer)     clearMode = clearMode | RENDERER_CLEAR_DEPTH;
			if (mClearColorBuffer) clearMode = clearMode | RENDERER_CLEAR_COLOR;
			renderer->ClearView((RendererClearMode)clearMode);

			renderer->SetScissorTestMode(RENDERER_SCISSOR_TEST_OFF);
			renderer->FlushState();

			return true;
		}
	}
	renderer->FlushState();
	return false;
}

void	OpenGLCameraOrtho::ProtectedRelease(TravState* state)
{
	if(getRenderingScreen())
	{
		CHECK_GLERROR;
		getRenderingScreen()->Release(state);
		CHECK_GLERROR;
	}
}

