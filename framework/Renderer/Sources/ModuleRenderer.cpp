#include "PrecompiledHeaders.h"

#include "RendererIncludes.h"
#include "Core.h"
#include "Timer.h"
#include "ModuleRenderer.h"
#include "DrawableSwitch.h"
#include "SpriteSheetTexture.h"
#include "DrawableCallback.h"
#include "DrawVertice.h"
#include "KigsBitmap.h"
#include "Material.h"
#include "MaterialStage.h"
#include "RendererProfileDrawingObject.h"
#include "MultiMesh.h"
#include "RenderingCustomizer.h"
#include "StencilMask.h"
#include "TangentSpaceLOD.h"
#include "Shader.h"
#include "ModernMesh.h"
#include "Holo3DPanel.h"

#include "UIVerticesInfo.h"

#ifdef KIGS_TOOLS
RendererStats gRendererStats;
#endif

ModuleSpecificRenderer * ModuleRenderer::mTheGlobalRenderer = nullptr;

IMPLEMENT_CLASS_INFO(ModuleRenderer);

IMPLEMENT_CONSTRUCTOR(ModuleRenderer)
, mSpecificRenderer(nullptr)
{
}

void ModuleRenderer::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core, "Renderer", params);

	core->RegisterMainModuleList(this, RendererModuleCoreIndex);

	RegisterDynamic(PlatformRendererModuleInit(core, params));

	// Init system independant classes
	DECLARE_FULL_CLASS_INFO(core, TextureFileManager, TextureFileManager, Renderer)
	DECLARE_FULL_CLASS_INFO(core, DrawableCallback, DrawableCallback, Renderer)
	DECLARE_FULL_CLASS_INFO(core, DrawableSwitch, DrawableSwitch, Renderer)
	DECLARE_FULL_CLASS_INFO(core, DrawVertice, DrawVertice, Renderer)
	DECLARE_FULL_CLASS_INFO(core, SpriteSheetTexture, SpriteSheetTexture, Renderer)
	DECLARE_FULL_CLASS_INFO(core, KigsBitmap, KigsBitmap, Renderer);
	DECLARE_FULL_CLASS_INFO(core, MultiMesh, MultiMesh, Renderer);
	DECLARE_FULL_CLASS_INFO(core, RenderingCustomizer, RenderingCustomizer, Renderer);
	DECLARE_FULL_CLASS_INFO(core, StencilMask, StencilMask, Renderer);
	DECLARE_FULL_CLASS_INFO(core, TangentSpaceLOD, TangentSpaceLOD, Renderer);
	DECLARE_FULL_CLASS_INFO(core, ModernMesh, ModernMesh, Renderer)
	DECLARE_FULL_CLASS_INFO(core, ModernMeshItemGroup, ModernMeshItemGroup, Renderer)
	DECLARE_FULL_CLASS_INFO(core, MaterialStage, MaterialStage, Renderer);
	DECLARE_FULL_CLASS_INFO(core, Holo3DPanel, Holo3DPanel, Renderer);

	//DECLARE_FULL_CLASS_INFO(core, RendererProfileDrawingObject, ProfileDrawingObject, Renderer)

	kstl::vector<CMSP>	instances=CoreModifiable::GetInstances("ModuleSpecificRenderer");

	if (instances.size())
		mSpecificRenderer = (ModuleSpecificRenderer*)(instances[0].get());

	mTheGlobalRenderer = mSpecificRenderer;
}

void ModuleRenderer::Close()
{
	KigsCore::ReleaseSingleton("TextureFileManager");

	BaseClose();

}

void ModuleRenderer::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer, addParam);
}


IMPLEMENT_CLASS_INFO(ModuleSpecificRenderer)
IMPLEMENT_CONSTRUCTOR(ModuleSpecificRenderer)
, mCurrentState(0)
, mStateStackTop(0)
, mDirtyMatrix(0)
, mCurrentTextureUnit(0)
, mFirstRenderingScreen(0)
, mDefaultUIShader(nullptr)
, mCurrentVBO(-1)
, mUICurrentVBO(-1)
{
	mActivatedScreenList.clear();
}

// init should be called at the end of specific Init
void ModuleSpecificRenderer::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	mWantedState.reserve(4);

	mCurrentState = createNewState();			// default state creation
	mStateStackTop = createNewState();			// default state creation
	mWantedState.push_back(mStateStackTop);	// init stack

	// init matrix stack
	mMatrixStack[0].push_back();
	mMatrixStack[0][0].SetIdentity();
	mMatrixStack[1].push_back();
	mMatrixStack[1][0].SetIdentity();
	mMatrixStack[2].push_back();
	mMatrixStack[2][0].SetIdentity();
}

void	ModuleSpecificRenderer::endFrame(TravState* state)
{
	// frame is finished, do all the rendering screen fade and clear management
	auto it = mActivatedScreenList.begin();
	auto itE = mActivatedScreenList.end();

	while (it != itE)
	{
		((RenderingScreen*)(*it))->ManageFade(state);
		++it;
	}

}


void ModuleSpecificRenderer::Close()
{
	if (mCurrentState)
	{
		delete mCurrentState;
	}
	kstl::vector<RenderingState*>::iterator delit = mWantedState.begin();
	kstl::vector<RenderingState*>::iterator delitend = mWantedState.end();
	while (delit != delitend)
	{
		delete (*delit);
		++delit;
	}

	mVertexBufferManager = nullptr;
}


void	ModuleSpecificRenderer::initVBOs()
{
	if (mCurrentVBO == -1)
	{
		CreateBuffer(PREALLOCATED_VBO_COUNT, mVBO);
		mCurrentVBO = 0;
		mUICurrentVBO = 0;
		VInfo2D vi;
		UIVerticesInfo muiv(&vi);
		// init ui quad vbo
		for (int i = 0; i < 4; i++)
		{
			unsigned int bufferName = getUIVBO();

			BufferData(bufferName, KIGS_BUFFER_TARGET_ARRAY, muiv.Offset* muiv.vertexCount, 0, KIGS_BUFFER_USAGE_STATIC);
		}
	}
}
// # Buffer Section
void ModuleSpecificRenderer::CreateBuffer(int count, unsigned int * id)
{
	mVertexBufferManager->GenBuffer(count, id);
}

void ModuleSpecificRenderer::DeleteBuffer(int count, unsigned int * id)
{
	for (int i = 0; i < count; i++)
	{
		UnsetArrayBuffer(id[i]);
		UnsetElementBuffer(id[i]);
	}
	mVertexBufferManager->DelBufferLater(count, id);
}

void ModuleSpecificRenderer::Update(const Timer& timer, void* addParam)
{
}

void	ModuleSpecificRenderer::FlushState(bool force)
{
	mStateStackTop->FlushState(mCurrentState, force);
}

void	ModuleSpecificRenderer::ClearView(RendererClearMode clearMode)
{
	mStateStackTop->ClearView(clearMode);
}

void	ModuleSpecificRenderer::Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	mStateStackTop->Viewport(x, y, width, height);
}

void	ModuleSpecificRenderer::FlushLightModelfv()
{
	mStateStackTop->FlushLightModelfv(mCurrentState);
}

void	ModuleSpecificRenderer::FlushLightModeli()
{
	mStateStackTop->FlushLightModeli(mCurrentState);
}

void	ModuleSpecificRenderer::PushState()
{

	RenderingState* toadd = createNewState(mStateStackTop);			// default state creation
	mWantedState.push_back(toadd);
	mStateStackTop = toadd;
}

void	ModuleSpecificRenderer::PopState()
{

	//delete myWantedState[myWantedState.size()-1];
	delete *mWantedState.rbegin();
	mWantedState.pop_back();
	mStateStackTop = *mWantedState.rbegin();
}

// Matrix management

void	ModuleSpecificRenderer::LoadIdentity(int mode)
{
	mDirtyMatrix |= (1 << mode);
	mMatrixStack[mode].back().SetIdentity();
}

void	ModuleSpecificRenderer::LoadMatrix(int mode, const kfloat *newMat)
{
	mDirtyMatrix |= (1 << mode);
	Matrix4x4 *mat = &mMatrixStack[mode].back();
	mat->Set(newMat);
}

void	ModuleSpecificRenderer::Translate(int mode, kfloat tx, kfloat ty, kfloat tz)
{
	Matrix4x4&	result = mMatrixStack[mode].back();
	result[12] += (result[0] * tx + result[4] * ty + result[8] * tz);
	result[13] += (result[1] * tx + result[5] * ty + result[9] * tz);
	result[14] += (result[2] * tx + result[6] * ty + result[10] * tz);
	result[15] += (result[3] * tx + result[7] * ty + result[11] * tz);
	mDirtyMatrix |= (1 << mode);
}

void	ModuleSpecificRenderer::Rotate(int mode, kfloat angle, kfloat x, kfloat y, kfloat z)
{

	kfloat sinAngle = (kfloat)sinf(angle * fPI / KFLOAT_CONST(180.0f));
	kfloat cosAngle = (kfloat)cosf(angle * fPI / KFLOAT_CONST(180.0f));
	kfloat oneMinusCos = KFLOAT_CONST(1.0f) - cosAngle;
	kfloat mag = sqrtf(x * x + y * y + z * z);

	if (mag != KFLOAT_CONST(0.0f) && mag != KFLOAT_CONST(1.0f))
	{
		mag = KFLOAT_CONST(1.0f) / mag;
		x *= mag;
		y *= mag;
		z *= mag;
	}

	kfloat xx = x * x;
	kfloat yy = y * y;
	kfloat zz = z * z;
	kfloat xy = x * y;
	kfloat yz = y * z;
	kfloat zx = z * x;
	kfloat xs = x * sinAngle;
	kfloat ys = y * sinAngle;
	kfloat zs = z * sinAngle;

	Matrix4x4 rotationMatrix;

	rotationMatrix[0] = (oneMinusCos * xx) + cosAngle;
	rotationMatrix[1] = (oneMinusCos * xy) - zs;
	rotationMatrix[2] = (oneMinusCos * zx) + ys;
	rotationMatrix[3] = KFLOAT_ZERO;

	rotationMatrix[4] = (oneMinusCos * xy) + zs;
	rotationMatrix[5] = (oneMinusCos * yy) + cosAngle;
	rotationMatrix[6] = (oneMinusCos * yz) - xs;
	rotationMatrix[7] = 0.0f;

	rotationMatrix[8] = (oneMinusCos * zx) - ys;
	rotationMatrix[9] = (oneMinusCos * yz) + xs;
	rotationMatrix[10] = (oneMinusCos * zz) + cosAngle;
	rotationMatrix[11] = KFLOAT_ZERO;

	rotationMatrix[12] = KFLOAT_ZERO;
	rotationMatrix[13] = KFLOAT_ZERO;
	rotationMatrix[14] = KFLOAT_ZERO;
	rotationMatrix[15] = KFLOAT_ONE;

	Multiply(mode, &(rotationMatrix.e[0][0]));

}

void	ModuleSpecificRenderer::Scale(int mode, kfloat sx, kfloat sy, kfloat sz)
{
	Matrix4x4&	result = mMatrixStack[mode].back();
	result[0] *= sx;
	result[1] *= sx;
	result[2] *= sx;
	result[3] *= sx;

	result[4] *= sy;
	result[5] *= sy;
	result[6] *= sy;
	result[7] *= sy;

	result[8] *= sz;
	result[9] *= sz;
	result[10] *= sz;
	result[11] *= sz;
	mDirtyMatrix |= (1 << mode);
}

void	ModuleSpecificRenderer::Frustum(int mode, kfloat left, kfloat right, kfloat bottom, kfloat top, kfloat nearZ, kfloat farZ)
{
	kfloat deltaX = right - left;
	kfloat deltaY = top - bottom;
	kfloat deltaZ = farZ - nearZ;
	Matrix4x4 frust;

	if ((nearZ <= KFLOAT_ZERO) || (farZ <= KFLOAT_ZERO) || (deltaX <= KFLOAT_ZERO) || (deltaY <= KFLOAT_ZERO) || (deltaZ <= KFLOAT_ZERO))
	{
		KIGS_ERROR("Invalid frustrum", 1);
		return;
	}

	frust[0] = KFLOAT_CONST(2.0f) * nearZ / deltaX;
	frust[1] = frust[2] = frust[3] = KFLOAT_ZERO;

	frust[5] = KFLOAT_CONST(2.0f) * nearZ / deltaY;
	frust[4] = frust[6] = frust[7] = KFLOAT_ZERO;

	frust[8] = (right + left) / deltaX;
	frust[9] = (top + bottom) / deltaY;
	frust[10] = -(nearZ + farZ) / deltaZ;
	frust[11] = -KFLOAT_ONE;

	frust[14] = KFLOAT_CONST(-2.0f) * nearZ * farZ / deltaZ;
	frust[12] = frust[13] = frust[15] = KFLOAT_ZERO;

	Multiply(mode, &(frust.e[0][0]));
}

void	ModuleSpecificRenderer::Ortho(int mode, kfloat left, kfloat right, kfloat bottom, kfloat top, kfloat nearZ, kfloat farZ)
{
	kfloat deltaX = right - left;
	kfloat deltaY = top - bottom;
	kfloat deltaZ = farZ - nearZ;

	Matrix4x4 ortho;
	ortho.SetIdentity();

	if ((deltaX == KFLOAT_ZERO) || (deltaY == KFLOAT_ZERO) || (deltaZ == KFLOAT_ZERO))
	{
		//KIGS_ERROR("Invalid ortho", 1);
		return;
	}

	ortho[0] = 2.0f / deltaX;
	ortho[12] = -(right + left) / deltaX;
	ortho[5] = 2.0f / deltaY;
	ortho[13] = -(top + bottom) / deltaY;
#if USE_D3D
	ortho[10] = 1.0f / deltaZ;
	ortho[14] = nearZ  / deltaZ;
#else
	ortho[10] = -2.0f / deltaZ;
	ortho[14] = -(nearZ + farZ) / deltaZ;
#endif
	
	ModuleSpecificRenderer::LoadIdentity(mode);
	Multiply(mode, &(ortho.e[0][0]));
}

void	ModuleSpecificRenderer::Perspective(int mode, kfloat fovy, kfloat aspect, kfloat nearZ, kfloat farZ)
{
	kfloat frustumHeight = tanf(fovy * fPI / 360.0f) * nearZ;
	kfloat frustumWidth = frustumHeight * aspect;

	ModuleSpecificRenderer::LoadIdentity(mode);
	Frustum(mode, -frustumWidth, frustumWidth, -frustumHeight, frustumHeight, nearZ, farZ);
}

void ModuleSpecificRenderer::LookAt(int mode, kfloat eyex, kfloat eyey, kfloat eyez,
	kfloat centerx, kfloat centery, kfloat centerz,
	kfloat upx, kfloat upy, kfloat upz)
{
	ModuleSpecificRenderer::LoadIdentity(mode);

	kfloat m[16];
	kfloat x[3], y[3], z[3];
	kfloat mag;

	/* Make rotation matrix */

	/* Z vector */
	z[0] = eyex - centerx;
	z[1] = eyey - centery;
	z[2] = eyez - centerz;
	mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
	if (mag) {          /* mpichler, 19950515 */
		z[0] /= mag;
		z[1] /= mag;
		z[2] /= mag;
	}

	/* Y vector */
	y[0] = upx;
	y[1] = upy;
	y[2] = upz;

	/* X vector = Y cross Z */
	x[0] = y[1] * z[2] - y[2] * z[1];
	x[1] = -y[0] * z[2] + y[2] * z[0];
	x[2] = y[0] * z[1] - y[1] * z[0];

	/* Recompute Y = Z cross X */
	y[0] = z[1] * x[2] - z[2] * x[1];
	y[1] = -z[0] * x[2] + z[2] * x[0];
	y[2] = z[0] * x[1] - z[1] * x[0];

	/* mpichler, 19950515 */
	/* cross product gives area of parallelogram, which is < 1.0 for
	 * non-perpendicular unit-length vectors; so normalize x, y here
	 */

	mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
	if (mag) {
		x[0] /= mag;
		x[1] /= mag;
		x[2] /= mag;
	}

	mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
	if (mag) {
		y[0] /= mag;
		y[1] /= mag;
		y[2] /= mag;
	}

#define M(row,col)  m[col*4+row]
	M(0, 0) = x[0];
	M(0, 1) = x[1];
	M(0, 2) = x[2];
	M(0, 3) = 0.0;
	M(1, 0) = y[0];
	M(1, 1) = y[1];
	M(1, 2) = y[2];
	M(1, 3) = 0.0;
	M(2, 0) = z[0];
	M(2, 1) = z[1];
	M(2, 2) = z[2];
	M(2, 3) = 0.0;
	M(3, 0) = 0.0;
	M(3, 1) = 0.0;
	M(3, 2) = 0.0;
	M(3, 3) = 1.0;
#undef M
	ModuleSpecificRenderer::Multiply(mode, m);

	/* Translate Eye to Origin */
	ModuleSpecificRenderer::Translate(mode, -eyex, -eyey, -eyez);

}



void	ModuleSpecificRenderer::pushShader(ShaderBase* shad, TravState* state)
{
	bool needActive = false;
	if (mShaderStack.size())
	{
		if ((mCurrentShader != shad) || (mCurrentShaderProgram != shad->GetCurrentShaderProgram())) // need activation only if different shader
		{
			mCurrentShader->Deactive(state);
			needActive = true;
		}
	}
	else
	{
		needActive = true;
	}

	Material*	StateMaterial = 0;
	if (needActive)
	{
		if (state&&state->mCurrentMaterial)
		{
			Material*	toPostDraw = state->mCurrentMaterial;
			state->mCurrentMaterial = 0;
			toPostDraw->CheckPostDraw(state);

		}
	}

	mCurrentShaderProgram = shad->GetCurrentShaderProgram();

	mCurrentShader = shad;
	mShaderStack.push_back(shad); // must be pushed before activation
	//printf("PUSH Shader %p %s(%d)\n", shad, shad->getName().c_str(), shad->Get_ShaderProgram());

	if (needActive)
	{
		mDirtyShaderMatrix = 1;
	
		shad->Active(state, true);
	}

	if (StateMaterial)
		StateMaterial->CheckPreDraw(state);
}

void	ModuleSpecificRenderer::popShader(ShaderBase* shad, TravState* state)
{
	// just make sure shad is the shader on top of the stack
	bool isShaderOK = false;
	if (mShaderStack.size())
	{
		if (mCurrentShader == shad)
		{
			isShaderOK = true;
		}

		

		// pop will be done after deactivation
		// so shaderback is myShaderStack[myShaderStack.size()-2] if exists
		ShaderBase* shaderBack = 0;
		if (mShaderStack.size()>1)
		{
			shaderBack = mShaderStack[mShaderStack.size()-2];
		}
		//printf("POP  Shader %p %s(%d)\n", shad, shad->getName().c_str(), shad->Get_ShaderProgram());


		Material*	StateMaterial = 0;
		if ((shaderBack != shad) || (mCurrentShaderProgram != shaderBack->GetCurrentShaderProgram()))
		{
			if (state&&state->mCurrentMaterial)
			{
				Material*	toPostDraw = state->mCurrentMaterial;
				state->mCurrentMaterial = 0;
				toPostDraw->CheckPostDraw(state);

			}

			shad->Deactive(state);

			mCurrentShader = shaderBack;

			if(shaderBack)
				mCurrentShaderProgram = shaderBack->GetCurrentShaderProgram();

			mShaderStack.pop_back();

			if (mCurrentShader)
			{
				mDirtyShaderMatrix = 1;
				mCurrentShader->Active(state, true);

				if (StateMaterial)
					StateMaterial->CheckPreDraw(state);
			}
			else
			{
				mCurrentShaderProgram = 0;
			}
		}
		else
		{
			mShaderStack.pop_back();
		}

		if (isShaderOK)
		{
			return;
		}
	}
	KIGS_ERROR("bad shader pop", 1);
}

bool	ModuleSpecificRenderer::HasShader()
{
	return !mShaderStack.empty();
}

ShaderBase* ModuleSpecificRenderer::GetActiveShader()
{
	if (mShaderStack.empty())
		return nullptr;
	return mShaderStack.back();
}