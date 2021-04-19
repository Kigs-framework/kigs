#include "PrecompiledHeaders.h"

#include "RenderingScreen.h"
#include "ModuleRenderer.h"
#include "Window.h"
#include "TextureFileManager.h"
#include "NotificationCenter.h"
#include "TravState.h"
#include "Texture.h"

IMPLEMENT_CLASS_INFO(RenderingScreen)
IMPLEMENT_CONSTRUCTOR(RenderingScreen)
	, mBitsPerPixel(*this, true, "BitsPerPixel", 32)
	, mBitsPerZ(*this, true, "BitsPerZ", 16)
	, mBitsForStencil(*this, true, "BitsForStencil", 0)
	, mSizeX(*this, false, "SizeX", 256)
	, mSizeY(*this, false, "SizeY", 256)
	, mDesignSizeX(*this, false, "DesignSizeX", 256)
	, mDesignSizeY(*this, false, "DesignSizeY", 256)
	, mIsOffScreen(*this, true, "IsOffScreen", false)
	, mParentWindowName(*this, true, "ParentWindowName", "")
	, mVSync(*this, true, "VSync", false)
	, mBrightness(*this, false, "Brightness", 0)
	, mBackDropColor(*this, false, "BackDropColor", 0.0, 0.0, 0.0)
	, mRotation(*this, false, "Rotation", 0.0f)
	, mDontClear(*this, false, "DontClear", false)
	, mNeedDoubleBuffer(*this, false, "NeedDoubleBuffer", true)
	, mWasActivated(false)
	, mUseFBO(*this, true, "UseFBO", false)
	, mFBOSizeX(*this, true, "FBOSizeX", 640)
	, mFBOSizeY(*this, true, "FBOSizeY", 480)
	, mFBOTexture(0)
{
	// retreive renderer
	ModuleSpecificRenderer* renderer =  ((ModuleRenderer*)KigsCore::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->RegisterRenderingScreen(this);

	mOldBrightness = mBrightness;

	mIsInit=false;

	mParentWindow=0;
}

void RenderingScreen::Update(const Timer&  timer, void* addParam)
{
	ModuleSpecificRenderer* renderer = ((ModuleRenderer*)KigsCore::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->SetClearColorValue(mBackDropColor[0], mBackDropColor[1], mBackDropColor[2], 1.0);
	renderer->FlushState();
	// if wasn't activated, no camera or layer was drawn, clear the screen
	if (!mWasActivated)
	{
		if(!mDontClear)
			renderer->ClearView(RENDERER_CLEAR_COLOR);
	}
	mWasActivated = false;
}


// recompute mDesignCoefX,mDesignCoefY;
void RenderingScreen::InitModifiable()
{
	kstl::vector<CMSP>	instances;

    kstl::string parentW=mParentWindowName.const_ref();

	if(parentW != "")
	{
		instances=CoreModifiable::GetInstancesByName("Window",parentW);
		if(instances.size())
			mParentWindow=(Window*)(instances[0].get());
	}

	// I want to be notified on brightness change
	mBrightness.changeNotificationLevel(Owner);

	if (mDesignSizeX == 0 && mDesignSizeY == 0)
	{
		mDesignSizeX = mSizeX;
		mDesignSizeY = mSizeY;
	}
	else
	{
		if (mDesignSizeX == 0)
		{
			mDesignSizeX = mDesignSizeY * mSizeX / mSizeY;
		}
		else if (mDesignSizeY == 0)
		{
			mDesignSizeY = mDesignSizeX * mSizeY / mSizeX;
		}
	}

	// recompute mDesignCoefX,mDesignCoefY;
	mSizeX.changeNotificationLevel(Owner);
	mSizeY.changeNotificationLevel(Owner);
	mDesignSizeX.changeNotificationLevel(Owner);
	mDesignSizeY.changeNotificationLevel(Owner);

	RecomputeDesignCoef();
	CoreModifiable::InitModifiable();
}

void RenderingScreen::NotifyUpdate(const unsigned int  labelid )
{
	if(	(labelid==mSizeX.getLabelID()) ||
		(labelid==mSizeY.getLabelID()) ||
		(labelid==mDesignSizeX.getLabelID()) ||
		(labelid==mDesignSizeY.getLabelID()) )
	{
		RecomputeDesignCoef();
	}

	CoreModifiable::NotifyUpdate(labelid);
}

void RenderingScreen::RecomputeDesignCoef()
{
	mDesignCoefX=((kfloat)(int)mDesignSizeX)/((kfloat)(int)mSizeX);
	mDesignCoefY=((kfloat)(int)mDesignSizeY)/((kfloat)(int)mSizeY);
}

v2f RenderingScreen::GlobalMousePositionToDesignPosition(v2i pos)
{
	v2f result{ -1, -1 };
	if (mParentWindow != 0)
	{
		mParentWindow->GetMousePosInWindow(pos.x, pos.y, result.x, result.y);
		auto size = mParentWindow->GetSize();
		result.x = mDesignSizeX * result.x / size.x;
		result.y = mDesignSizeY * result.y / size.y;
	}
	return result;
}

void RenderingScreen::GetMousePosInScreen(int posx,int posy,kfloat& sposx,kfloat& sposy)
{
	if(mParentWindow !=0)
		mParentWindow->GetMousePosInDesignWindow(posx,posy,sposx,sposy);
}

void RenderingScreen::GetMouseRatioInScreen(int posx,int posy,kfloat& sposx,kfloat& sposy)
{
	if(mParentWindow !=0)
	{
		mParentWindow->GetMousePosInDesignWindow(posx, posy, sposx, sposy);
		/*int L_WinSizeX, L_WinSizeY = 0;
		mParentWindow->getValue(LABEL_TO_ID(SizeX),L_WinSizeX);
		mParentWindow->getValue(LABEL_TO_ID(SizeY),L_WinSizeY);*/
		sposx -= ((int)mDesignSizeX)*0.5f;
		sposy -= ((int)mDesignSizeY)*0.5f;
		sposx /= (float)mDesignSizeX;
		sposy /= (float)mDesignSizeY;
	}
}

void RenderingScreen::GetMouseRatio(int posx,int posy,kfloat& sposx,kfloat& sposy)
{
	if(mParentWindow !=0)
	{
		int L_WinSizeX, L_WinSizeY = 0;
		mParentWindow->getValue(LABEL_TO_ID(SizeX),L_WinSizeX);
		mParentWindow->getValue(LABEL_TO_ID(SizeY),L_WinSizeY);
		sposx = posx-(int)L_WinSizeX*0.5f;
		sposy = posy-(int)L_WinSizeY*0.5f;
		sposx /= L_WinSizeX;
		sposy /= L_WinSizeY;
	}
}

bool RenderingScreen::MouseIsInScreen(int posx,int posy)
{
	if(mParentWindow !=0)
	{
		int L_WinSizeX, L_WinSizeY, L_PosX, L_PosY;
		mParentWindow->getValue(LABEL_TO_ID(PositionX),L_PosX);
		mParentWindow->getValue(LABEL_TO_ID(PositionY),L_PosY);
		mParentWindow->getValue(LABEL_TO_ID(SizeX),L_WinSizeX);
		mParentWindow->getValue(LABEL_TO_ID(SizeY),L_WinSizeY);

		if( (posx < L_PosX) || (posx > (L_PosX+L_WinSizeX)) ||
			(posy < L_PosY) || (posy > (L_PosY+L_WinSizeY)) )
			return false;
	}
	return true;
}

void RenderingScreen::GetMousePosInDesignScreen(int posx,int posy,kfloat& sposx,kfloat& sposy)
{
	sposx=((kfloat)posx)*mDesignCoefX;
	sposy=((kfloat)posy)*mDesignCoefY;
}

void RenderingScreen::GetMouseMoveInScreen(kfloat posx,kfloat posy,kfloat& sposx,kfloat& sposy)
{
	sposx=posx*mDesignCoefX;
	sposy=posy*mDesignCoefY;
}

bool RenderingScreen::SetActive(TravState* state)
{
	if (mSizeX == -1 || mSizeY == -1)
		return false;

	mWasActivated = true;

	if (state)
	{
		state->GetRenderer()->addActivatedRenderingScreen(this);
	}
	return true;
}

DEFINE_METHOD(RenderingScreen, ResetContext)
{
	ReInit();

	ModuleSpecificRenderer* renderer = ModuleRenderer::mTheGlobalRenderer; //((ModuleRenderer*)KigsCore::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->FlushState(true);

	auto textureManager = KigsCore::Singleton<TextureFileManager>();
	textureManager->ResetAllTexture();

	KigsCore::GetNotificationCenter()->postNotificationName("ResetContext", this);
	return true;
}

bool RenderingScreen::GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout)
{
	pout.pos.z = 0;

	if (mIsOffScreen)
	{
		kfloat cx = (kfloat)(unsigned int)mDesignSizeX;
		kfloat cy = (kfloat)(unsigned int)mDesignSizeY;

		pout.pos.x = posin.pos.x*cx;
		pout.pos.y = posin.pos.y*cy;
	}
	else
	{
		//GetMousePosInScreen((s32)posin.pos.x, (s32)posin.pos.y, pout.pos.x, pout.pos.y);
		pout.pos.xy = GlobalMousePositionToDesignPosition(posin.pos.xy);
		pout.pos.z = 0;
	}

	if ((pout.pos.x >= 0.0f) && (pout.pos.x < (kfloat)(unsigned int)mDesignSizeX))
	{
		if ((pout.pos.y >= 0.0f) && (pout.pos.y < (kfloat)(unsigned int)mDesignSizeY))
		{
			return true;
		}
	}
	return false;
}

bool RenderingScreen::IsValidTouchSupport(Vector3D posin)
{
	if (mIsOffScreen) // offscreen can not be a root touch support
	{
		return false;
	}
	Vector3D pout;
	GetMousePosInScreen((s32)posin.x, (s32)posin.y, pout.x, pout.y);

	// return true if pos is inside screen
	if ((pout.x >= 0) && (pout.x < mDesignSizeX) && (pout.y >= 0) && (pout.y < mDesignSizeY))
	{
		return true;
	}
	return false;
}

void RenderingScreen::ManageFade(TravState* state)
{
	if (mBrightness != 0)
	{
		setCurrentContext();

		ModuleSpecificRenderer* renderer = static_cast<ModuleSpecificRenderer*>(ModuleRenderer::mTheGlobalRenderer);

		renderer->pushShader((ShaderBase*)renderer->getDefaultUiShader().get(), state);

		unsigned int shader_flag = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;
		shader_flag |= ModuleRenderer::COLOR_ARRAY_MASK;

		renderer->GetActiveShader()->ChooseShader(state, shader_flag);

		renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA, RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
		renderer->SetBlendMode(RENDERER_BLEND_ON);
		renderer->SetLightMode(RENDERER_LIGHT_OFF);
		renderer->SetDepthTestMode(false);
		renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_OFF);
		renderer->Ortho(MATRIX_MODE_PROJECTION, 0.0f, (float)mDesignSizeX, 0.0f, (float)mDesignSizeY, -1.0f, 1.0f);
		renderer->LoadIdentity(MATRIX_MODE_MODEL);
		renderer->LoadIdentity(MATRIX_MODE_VIEW);

		Point2D size;
		GetSize(size.x, size.y);
		renderer->SetScissorValue(0, 0, (s32)size.x, (s32)size.y);
		renderer->Viewport(0, 0, (u32)size.x, (u32)size.y);

		VInfo2D vi;
		UIVerticesInfo mVI = UIVerticesInfo(&vi);
		VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(mVI.Buffer());

		// triangle strip order
		buf[0].setVertex(0.0f, 0.0f);
		buf[1].setVertex((float)mDesignSizeX, 0.0);
		buf[3].setVertex((float)mDesignSizeX, (float)mDesignSizeY);
		buf[2].setVertex(0.0, (float)mDesignSizeY);

		//Draw Quad that fits the screen
		if (mBrightness > 0)
		{
			unsigned char alpha = (unsigned char)((((float)mBrightness) / 16.0f) * 255.0f);
			buf[0].setColor(255, 255, 255, alpha);
			buf[1].setColor(255, 255, 255, alpha);
			buf[2].setColor(255, 255, 255, alpha);
			buf[3].setColor(255, 255, 255, alpha);
		}
		else
		{
			unsigned char alpha = (unsigned char)((((float)-mBrightness) / 16.0f) * 255.0f);
			buf[0].setColor(0, 0, 0, alpha);
			buf[1].setColor(0, 0, 0, alpha);
			buf[2].setColor(0, 0, 0, alpha);
			buf[3].setColor(0, 0, 0, alpha);
		}
		mVI.SetFlag(UIVerticesInfo_Vertex | UIVerticesInfo_Color);

		renderer->DrawUIQuad(state, &mVI);
		renderer->popShader((ShaderBase*)renderer->getDefaultUiShader().get(), state);
	}

	//PRINTPROFILES;
}