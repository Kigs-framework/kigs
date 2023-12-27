#include "PrecompiledHeaders.h"

#include "RenderingScreen.h"
#include "ModuleRenderer.h"
#include "Window.h"
#include "TextureFileManager.h"
#include "NotificationCenter.h"
#include "TravState.h"
#include "Texture.h"

using namespace Kigs::Draw;
using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(RenderingScreen)
IMPLEMENT_CONSTRUCTOR(RenderingScreen)
	, mWasActivated(false)
	, mFBOTexture(0)
{
	setInitParameter("ParentWindowName", true);
	setInitParameter("BitsForStencil", true);
	setInitParameter("BitsPerZ", true);
	setInitParameter("BitsPerPixel", true);
	setInitParameter("UseFBO", true);
	setInitParameter("VSync", true);
	setInitParameter("IsOffScreen", true);

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
	std::vector<CMSP>	instances;

    std::string parentW=mParentWindowName;

	if(parentW != "")
	{
		instances=CoreModifiable::GetInstancesByName("Window",parentW);
		if(instances.size())
			mParentWindow=(Gui::Window*)(instances[0].get());
	}

	// I want to be notified on brightness change
	setOwnerNotification("Brightness", true);

	if (mDesignSize[0] == 0 && mDesignSize[1] == 0)
	{
		mDesignSize = (v2f)mSize;
	}
	else
	{
		if (mDesignSize[0] == 0)
		{
			mDesignSize[0] = mDesignSize[1] * mSize[0] / mSize[1];
		}
		else if (mDesignSize[1] == 0)
		{
			mDesignSize[1] = mDesignSize[0] * mSize[1] / mSize[0];
		}
	}

	// recompute mDesignCoefX,mDesignCoefY;
	setOwnerNotification("Size", true);
	setOwnerNotification("DesignSize", true);

	RecomputeDesignCoef();
	CoreModifiable::InitModifiable();
}

void RenderingScreen::NotifyUpdate(const unsigned int  labelid )
{
	if(	(labelid==KigsID("Size")._id ) ||
		(labelid== KigsID("DesignSize")._id ))
	{
		RecomputeDesignCoef();
	}

	CoreModifiable::NotifyUpdate(labelid);
}

void RenderingScreen::RecomputeDesignCoef()
{
	mDesignCoef[0] = mDesignSize[0] / mSize[0];
	mDesignCoef[1] = mDesignSize[1] / mSize[1];
}

v2f RenderingScreen::GlobalMousePositionToDesignPosition(v2i pos)
{
	v2f result{ -1, -1 };
	if (mParentWindow != 0)
	{
		mParentWindow->GetMousePosInWindow(pos.x, pos.y, result.x, result.y);
		auto size = mParentWindow->GetSize();
		result.x = mDesignSize[0] * result.x / size.x;
		result.y = mDesignSize[1] * result.y / size.y;
	}
	return result;
}

void RenderingScreen::GetMousePosInScreen(int posx,int posy,float& sposx,float& sposy)
{
	if(mParentWindow !=0)
		mParentWindow->GetMousePosInDesignWindow(posx,posy,sposx,sposy);
}

void RenderingScreen::GetMouseRatioInScreen(int posx,int posy,float& sposx,float& sposy)
{
	if(mParentWindow !=0)
	{
		mParentWindow->GetMousePosInDesignWindow(posx, posy, sposx, sposy);

		sposx -= ((int)mDesignSize[0])*0.5f;
		sposy -= ((int)mDesignSize[1])*0.5f;
		sposx /= (float)mDesignSize[0];
		sposy /= (float)mDesignSize[1];
	}
}

void RenderingScreen::GetMouseRatio(int posx,int posy,float& sposx,float& sposy)
{
	if(mParentWindow !=0)
	{
		v2f L_WinSize( 0,0);
		mParentWindow->getValue("Size", L_WinSize);
		sposx = posx-(int)L_WinSize.x*0.5f;
		sposy = posy-(int)L_WinSize.y*0.5f;
		sposx /= L_WinSize.x;
		sposy /= L_WinSize.y;
	}
}

bool RenderingScreen::MouseIsInScreen(int posx,int posy)
{
	if(mParentWindow !=0)
	{
		v2f L_WinSize, L_Pos;
		mParentWindow->getValue("Position",L_Pos);
		mParentWindow->getValue("Size",L_WinSize);

		if( (posx < L_Pos.x) || (posx > (L_Pos.x+L_WinSize.x)) ||
			(posy < L_Pos.y) || (posy > (L_Pos.y+L_WinSize.y)) )
			return false;
	}
	return true;
}

void RenderingScreen::GetMousePosInDesignScreen(int posx,int posy,float& sposx,float& sposy)
{
	sposx=((float)posx)*mDesignCoef[0];
	sposy=((float)posy)*mDesignCoef[1];
}

void RenderingScreen::GetMouseMoveInScreen(float posx,float posy,float& sposx,float& sposy)
{
	sposx=posx*mDesignCoef[0];
	sposy=posy*mDesignCoef[1];
}

bool RenderingScreen::SetActive(TravState* state)
{
	if (mSize[0] == -1 || mSize[1] == -1)
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

bool RenderingScreen::GetDataInTouchSupport(const Input::touchPosInfos& posin, Input::touchPosInfos& pout)
{
	pout.pos.z = 0;

	if (mIsOffScreen)
	{
		float cx = mDesignSize[0];
		float cy = mDesignSize[1];

		pout.pos.x = posin.pos.x*cx;
		pout.pos.y = posin.pos.y*cy;
	}
	else
	{
		//GetMousePosInScreen((s32)posin.pos.x, (s32)posin.pos.y, pout.pos.x, pout.pos.y);
		pout.pos.xy = GlobalMousePositionToDesignPosition(posin.pos.xy);
		pout.pos.z = 0;
	}

	if ((pout.pos.x >= 0.0f) && (pout.pos.x < mDesignSize[0]))
	{
		if ((pout.pos.y >= 0.0f) && (pout.pos.y < mDesignSize[1]))
		{
			return true;
		}
	}
	return false;
}

bool RenderingScreen::IsValidTouchSupport(v3f posin)
{
	if (mIsOffScreen) // offscreen can not be a root touch support
	{
		return false;
	}
	v3f pout;
	GetMousePosInScreen((s32)posin.x, (s32)posin.y, pout.x, pout.y);

	// return true if pos is inside screen
	if ((pout.x >= 0) && (pout.x < mDesignSize[0]) && (pout.y >= 0) && (pout.y < mDesignSize[1]))
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
		renderer->Ortho(MATRIX_MODE_PROJECTION, 0.0f, (float)mDesignSize[0], 0.0f, (float)mDesignSize[1], -1.0f, 1.0f);
		renderer->LoadIdentity(MATRIX_MODE_MODEL);
		renderer->LoadIdentity(MATRIX_MODE_VIEW);

		v2f size;
		GetSize(size.x, size.y);
		renderer->SetScissorValue(0, 0, (s32)size.x, (s32)size.y);
		renderer->Viewport(0, 0, (u32)size.x, (u32)size.y);

		VInfo2D vi;
		UIVerticesInfo mVI = UIVerticesInfo(&vi);
		VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(mVI.Buffer());

		// triangle strip order
		buf[0].setVertex(0.0f, 0.0f);
		buf[1].setVertex((float)mDesignSize[0], 0.0);
		buf[3].setVertex((float)mDesignSize[0], (float)mDesignSize[1]);
		buf[2].setVertex(0.0, (float)mDesignSize[1]);

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