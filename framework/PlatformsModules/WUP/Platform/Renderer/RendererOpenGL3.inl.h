#pragma once
//#include "Platform/Main/uwp.h"
#include "Platform/Main/angle_includes.h"

//#include "Platform/Main/BaseApp.h"
//extern UWPKigs::App^ TheApp;
//using namespace winrt::Windows::Graphics::Holographic;

extern bool gIsHolographic;

void RendererOpenGL::PlatformInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	if (KigsCore::GetCoreApplication()->IsHolographic())
	{
		if (mHoloMatrix == nullptr)
			mHoloMatrix = new HoloMatrix();

		float renderTargetArrayIndices[] = { 0.f, 1.f };
		glGenBuffers(1, &mHoloMatrix->mRenderTargetArrayIndices);
		BufferData(mHoloMatrix->mRenderTargetArrayIndices, KIGS_BUFFER_TARGET_ARRAY, sizeof(renderTargetArrayIndices), renderTargetArrayIndices, KIGS_BUFFER_USAGE_STATIC);
		mHoloMatrix->mRenderTargetArrayIndicesCount = 2;
	}
}

void RendererOpenGL::PlatformUpdate(const Timer& timer, void* addParam)
{
}

void RendererOpenGL::PlatformClose()
{
	glDeleteBuffers(1, &mHoloMatrix->mRenderTargetArrayIndices);

	if (mHoloMatrix)
		delete mHoloMatrix;
}

// # Draw functions
void	RendererOpenGL::DrawArrays(TravState* state, unsigned int mode, int first, int count)
{
	if (state->GetHolographicMode())
	{
		glGetFloatv(GL_HOLOGRAPHIC_MVP_MATRICES_ANGLE, mHoloMatrix->mHoloMatrix);
		myVertexBufferManager->FlushBindBuffer();
		FlushState();
		FlushMatrix(state);
		CHECK_GLERROR;

		
		// Load the render target array indices into an array.
		auto locs = myCurrentShader->GetLocation();
		SetVertexAttrib(mHoloMatrix->mRenderTargetArrayIndices, KIGS_VERTEX_ATTRIB_RENDER_TARGET_ARRAY_INDEX_ID, 1, KIGS_FLOAT, false, 0, (void*)0, myCurrentShader->GetLocation());
		CHECK_GLERROR;

		// Enable instancing.
		glVertexAttribDivisorANGLE(locs->attribRenderTargetArrayIndex, 1);
		glDrawArraysInstancedANGLE(mode, first, count, 2);
		glVertexAttribDivisorANGLE(locs->attribRenderTargetArrayIndex, 0);
		CHECK_GLERROR;
	}
	else
	{
		myVertexBufferManager->FlushBindBuffer();
		FlushState();
		FlushMatrix(state);
		CHECK_GLERROR;

		glDrawArrays(mode, first, count);
		CHECK_GLERROR;
	}

	myVertexBufferManager->Clear();
}

void	RendererOpenGL::DrawElements(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, bool clear_manager)
{
	if (state->GetHolographicMode())
	{
		glGetFloatv(GL_HOLOGRAPHIC_MVP_MATRICES_ANGLE, mHoloMatrix->mHoloMatrix);
		myVertexBufferManager->FlushBindBuffer();
		FlushState();
		FlushMatrix(state);
		CHECK_GLERROR;
		
		// Load the render target array indices into an array.
		auto locs = myCurrentShader->GetLocation();
		SetVertexAttrib(mHoloMatrix->mRenderTargetArrayIndices, KIGS_VERTEX_ATTRIB_RENDER_TARGET_ARRAY_INDEX_ID, 1, KIGS_FLOAT, false, 0, (void*)0, myCurrentShader->GetLocation());
		CHECK_GLERROR;

		// Enable instancing.
		glVertexAttribDivisorANGLE(locs->attribRenderTargetArrayIndex, 1);
		CHECK_GLERROR;
		glDrawElementsInstancedANGLE(mode, count, type, indices, 2);
		CHECK_GLERROR;
		glVertexAttribDivisorANGLE(locs->attribRenderTargetArrayIndex, 0);
		CHECK_GLERROR;
	}
	else
	{
		myVertexBufferManager->FlushBindBuffer();
		FlushState();
		FlushMatrix(state);
		CHECK_GLERROR;

		glDrawElements(mode, count, type, indices);
		CHECK_GLERROR;
	}
	if(clear_manager)
		myVertexBufferManager->Clear();
}

void	RendererOpenGL::DrawElementsInstanced(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, int primcount, bool clear_manager)
{

	if (state->GetHolographicMode())
	{
		glGetFloatv(GL_HOLOGRAPHIC_MVP_MATRICES_ANGLE, mHoloMatrix->mHoloMatrix);
		myVertexBufferManager->FlushBindBuffer();
		FlushState();
		FlushMatrix(state);
		CHECK_GLERROR;

		
		if (primcount*2 > mHoloMatrix->mRenderTargetArrayIndicesCount) 
		{
			std::vector<float> data;
			data.resize(2*primcount);
			for (int i = 0; i < 2*primcount; ++i)
				data[i] = float(i % 2);

			BufferData(mHoloMatrix->mRenderTargetArrayIndices, KIGS_BUFFER_TARGET_ARRAY, sizeof(float)*data.size(), data.data(), KIGS_BUFFER_USAGE_STATIC);
			mHoloMatrix->mRenderTargetArrayIndicesCount = 2*primcount;
		}
		
		// Load the render target array indices into an array.
		auto locs = myCurrentShader->GetLocation();
		SetVertexAttrib(mHoloMatrix->mRenderTargetArrayIndices, KIGS_VERTEX_ATTRIB_RENDER_TARGET_ARRAY_INDEX_ID, 1, KIGS_FLOAT, false, 0, (void*)0, myCurrentShader->GetLocation());
		CHECK_GLERROR;

		// Enable instancing.
		glVertexAttribDivisorANGLE(locs->attribRenderTargetArrayIndex, 1);
		CHECK_GLERROR;
		glDrawElementsInstancedANGLE(mode, count, type, indices, 2*primcount);
		CHECK_GLERROR;
		glVertexAttribDivisorANGLE(locs->attribRenderTargetArrayIndex, 0);
		CHECK_GLERROR;
	}
	else
	{
		myVertexBufferManager->FlushBindBuffer();
		FlushState();
		FlushMatrix(state);
		CHECK_GLERROR;

		glDrawElementsInstancedANGLE(mode, count, type, indices, primcount);
		CHECK_GLERROR;
	}
	if (clear_manager)
		myVertexBufferManager->Clear();
}