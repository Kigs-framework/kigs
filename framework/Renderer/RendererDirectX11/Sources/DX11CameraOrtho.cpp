//#include "PrecompiledHeaders.h"

//#include "math.h"
#include "DX11CameraOrtho.h"
#include "CullingObject.h"
#include "RendererDX11.h"
//#include "TravState.h"

IMPLEMENT_CLASS_INFO(DX11CameraOrtho);

DX11CameraOrtho::~DX11CameraOrtho()
{
}

void  DX11CameraOrtho::InitCullingObject(CullingObject* obj)
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

	kfloat aspect;
	if (myAspectRatio!=0)
		aspect = myAspectRatio;
	else
		aspect = (width*myViewportSizeX) / (height*myViewportSizeY); 

	kfloat size2 = m_Size;

	auto l2g = GetLocalToGlobal();
	// near plane
	n.Set(1,0,0);
	o.Set(myNear,0,0);
	l2g.TransformVector(&n);
	l2g.TransformPoints(&o,1);
	obj->InitPlane(0,n,o);

	// far plane
	n.Set(-1,0,0);
	o.Set(myFar,0,0);
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

void DX11CameraOrtho::getRay(const kfloat &ScreenX, const kfloat &ScreenY, Point3D &RayOrigin, Vector3D &RayDirection)
{
	kfloat width , height;
	getRenderingScreen()->GetSize(width , height); 

	kfloat aspect;
	if (myAspectRatio!=0)
		aspect = myAspectRatio;
	else
		aspect = (width*myViewportSizeX) / (height*myViewportSizeY); 


	SetupNodeIfNeeded();
	RayOrigin.x = KFLOAT_CONST(0.0f);
	RayOrigin.y = (ScreenX-0.5f)* m_Size*aspect;
	RayOrigin.z = (ScreenY-0.5f)* m_Size;
	auto l2g = GetLocalToGlobal();
	l2g.TransformPoints(&RayOrigin,1);

	RayDirection = GetGlobalViewVector();
}

bool	DX11CameraOrtho::ProtectedSetActive(TravState* state)
{
	ModuleSpecificRenderer* renderer=state->GetRenderer();
	if(getRenderingScreen())
	{
		if(getRenderingScreen()->SetActive(state))
		{
			renderer->SetClearColorValue(myClearColor[0], myClearColor[1], myClearColor[2], myClearColor[3]);

			renderer->SetDepthValueMode(1.0);
			renderer->SetDepthTestMode(true);
			renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_OFF);
			kfloat width, height;
			getRenderingScreen()->GetSize(width, height);

			kfloat aspect;

			if (myAspectRatio != 0)
				aspect = myAspectRatio;
			else
				aspect = (width*myViewportSizeX) / (height*myViewportSizeY);

			renderer->Viewport((int)(myViewportMinX*width), (int)(height - (myViewportSizeY + myViewportMinY)*height), (int)(myViewportSizeX*width), (int)(myViewportSizeY*height));
			renderer->SetScissorValue((int)(myViewportMinX*width), (int)(height - (myViewportSizeY + myViewportMinY)*height), (int)(myViewportSizeX*width), (int)(myViewportSizeY*height));

			kfloat size2=m_Size*0.5f;
			renderer->Ortho(MATRIX_MODE_PROJECTION,- size2*aspect, size2*aspect, -size2, size2, myNear, myFar);
			
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
			if (myClearZBuffer)     clearMode = clearMode | RENDERER_CLEAR_DEPTH;
			if (myClearColorBuffer) clearMode = clearMode | RENDERER_CLEAR_COLOR;
			renderer->ClearView((RendererClearMode)clearMode);

			renderer->SetScissorTestMode(RENDERER_SCISSOR_TEST_OFF);
			renderer->FlushState();

			return true;
		}
	}
	renderer->FlushState();
	return false;
}

void	DX11CameraOrtho::ProtectedRelease(TravState* state)
{
	if(getRenderingScreen())
	{
		getRenderingScreen()->Release(state);
	}
}

