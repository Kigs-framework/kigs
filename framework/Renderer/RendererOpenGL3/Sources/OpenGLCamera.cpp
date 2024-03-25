#include "PrecompiledHeaders.h"

#include "math.h"

#include "OpenGLCamera.h"
#include "RendererOpenGL.h"
#include "TravState.h"

// include after all other kigs include (for iOS)
#include "Platform/Renderer/OpenGLInclude.h"
#include "Platform/Renderer/OpenGL3Camera.inl.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(OpenGLCamera)

OpenGLCamera::OpenGLCamera(const std::string& name, CLASS_NAME_TREE_ARG) : Camera(name, PASS_CLASS_NAME_TREE_ARG)
{
	//myFilters.clear();
#ifdef WUP
	mNearPlane.changeNotificationLevel(Owner);
	mFarPlane.changeNotificationLevel(Owner);
#endif
}

OpenGLCamera::~OpenGLCamera()
{

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

			auto l2g = GetLocalToGlobal();
			renderer->Perspective(MATRIX_MODE_PROJECTION, mVerticalFOV, aspect, mNearPlane, mFarPlane);
			renderer->LookAt(MATRIX_MODE_VIEW,
				l2g[3][0], l2g[3][1], l2g[3][2],
				l2g[0][0] + l2g[3][0], l2g[0][1] + l2g[3][1], l2g[0][2] + l2g[3][2],
				l2g[2][0], l2g[2][1], l2g[2][2]);
			renderer->LoadIdentity(MATRIX_MODE_MODEL);

		//	renderer->SetScissorValue(0, 0, (int)width, (int)height);
			renderer->SetScissorTestMode(RENDERER_SCISSOR_TEST_ON);
			renderer->FlushState();

			// CLEAR BUFFER
			int clearMode = RENDERER_CLEAR_NONE;
			if (mClearZBuffer)     clearMode = clearMode | RENDERER_CLEAR_DEPTH;
			if (mClearColorBuffer) clearMode = clearMode | RENDERER_CLEAR_COLOR;
			if (mClearStencilBuffer) clearMode = clearMode | RENDERER_CLEAR_STENCIL;
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
