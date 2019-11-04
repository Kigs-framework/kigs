#include "PrecompiledHeaders.h"

#include "math.h"

#include "OpenGLCamera.h"
#include "RendererOpenGL.h"
#include "TravState.h"

// include after all other kigs include (for iOS)
#include "Platform/Renderer/OpenGLInclude.h"
#include "Platform/Renderer/OpenGL3Camera.inl.h"

IMPLEMENT_CLASS_INFO(OpenGLCamera)

OpenGLCamera::OpenGLCamera(const kstl::string& name, CLASS_NAME_TREE_ARG) : Camera(name, PASS_CLASS_NAME_TREE_ARG)
{
	//myFilters.clear();
#ifdef WUP
	myNear.changeNotificationLevel(Owner);
	myFar.changeNotificationLevel(Owner);
#endif
}

OpenGLCamera::~OpenGLCamera()
{
	/*auto itr = myFilters.begin();
	auto end = myFilters.end();
	for (; itr != end; ++itr)
		(*itr)->Destroy();
	myFilters.clear();*/
}

bool	OpenGLCamera::ProtectedSetActive(TravState* state)
{
	ModuleSpecificRenderer* renderer = state->GetRenderer();
	if (getRenderingScreen())
	{
		if (getRenderingScreen()->SetActive(state))
		{
			// call platform specific event on set active
			PlatformProtectedSetActive(state);

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

			auto l2g = GetLocalToGlobal();
			renderer->Perspective(MATRIX_MODE_PROJECTION, myVerticalFOV, aspect, myNear, myFar);
			renderer->LookAt(MATRIX_MODE_VIEW,
				l2g.e[3][0], l2g.e[3][1], l2g.e[3][2],
				l2g.e[0][0] + l2g.e[3][0], l2g.e[0][1] + l2g.e[3][1], l2g.e[0][2] + l2g.e[3][2],
				l2g.e[2][0], l2g.e[2][1], l2g.e[2][2]);
			renderer->LoadIdentity(MATRIX_MODE_MODEL);

		//	renderer->SetScissorValue(0, 0, (int)width, (int)height);
			renderer->SetScissorTestMode(RENDERER_SCISSOR_TEST_ON);
			renderer->FlushState();

			// CLEAR BUFFER
			int clearMode = RENDERER_CLEAR_NONE;
			if (myClearZBuffer)     clearMode = clearMode | RENDERER_CLEAR_DEPTH;
			if (myClearColorBuffer) clearMode = clearMode | RENDERER_CLEAR_COLOR;
			if (myClearStencilBuffer) clearMode = clearMode | RENDERER_CLEAR_STENCIL;
			renderer->ClearView((RendererClearMode)clearMode);

			renderer->SetScissorTestMode(RENDERER_SCISSOR_TEST_OFF);
			renderer->FlushState();

			return true;
		}
	}
	return false;
}

void	OpenGLCamera::ProtectedRelease(TravState* state)
{
	if (getRenderingScreen())
	{
		getRenderingScreen()->Release(state);
	}
}

/*void OpenGLCamera::SetAmbient(kfloat r, kfloat g, kfloat b)
{
}*/



bool	OpenGLCamera::addItem(CoreModifiable *item, ItemPosition pos DECLARE_LINK_NAME)
{
	/*if (item->isSubType(API3DDeferredFilter::myClassID))
	{
		myFilters.insert((API3DDeferredFilter*)item);
	}*/

	return Camera::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool OpenGLCamera::removeItem(CoreModifiable* item DECLARE_LINK_NAME)
{
	/*if (item->isSubType(API3DDeferredFilter::myClassID))
	{
		myFilters.erase((API3DDeferredFilter*)item);
	}*/
	return Camera::removeItem(item PASS_LINK_NAME(linkName));
}