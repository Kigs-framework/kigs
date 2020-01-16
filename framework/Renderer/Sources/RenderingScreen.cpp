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
	, myBitsPerPixel(*this, true, "BitsPerPixel", 32)
	, myBitsPerZ(*this, true, "BitsPerZ", 16)
	, myBitsForStencil(*this, true, "BitsForStencil", 0)
	, mySizeX(*this, false, "SizeX", 256)
	, mySizeY(*this, false, "SizeY", 256)
	, myDesignSizeX(*this, false, "DesignSizeX", 256)
	, myDesignSizeY(*this, false, "DesignSizeY", 256)
	, myIsOffScreen(*this, true, "IsOffScreen", false)
	, myScreenOnTop(*this, false, "ScreenOnTop", false)
	, myParentWindowName(*this, true, "ParentWindowName", "")
	, myWaitVSync(*this, true, "VSync", false)
	, myBrightness(*this, false, "Brightness", 0)
	, myBackDropColor(*this, false, "BackDropColor", 0.0, 0.0, 0.0)
	, myRotation(*this, false, "Rotation", 0.0f)
	, myDontClear(*this, false, "DontClear", false)
	, myNeedDoubleBuffer(*this, false, "NeedDoubleBuffer", true)
	, myWasActivated(false)
	, myUseFBO(*this, true, "UseFBO", false)
	, myFBOSizeX(*this, true, "FBOSizeX", 640)
	, myFBOSizeY(*this, true, "FBOSizeY", 480)
	, myFBOTexture(0)
{
	// retreive renderer
	ModuleSpecificRenderer* renderer =  ((ModuleRenderer*)KigsCore::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->RegisterRenderingScreen(this);

	myOldBrightness = myBrightness;

	myIsInit=false;

	MyParentWindow=0;
}

void RenderingScreen::Update(const Timer&  timer, void* addParam)
{
	ModuleSpecificRenderer* renderer = ((ModuleRenderer*)KigsCore::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->SetClearColorValue(myBackDropColor[0], myBackDropColor[1], myBackDropColor[2], 0.0);
	renderer->FlushState();
	// if wasn't activated, no camera or layer was drawn, clear the screen
	if (!myWasActivated)
	{
		renderer->ClearView(RENDERER_CLEAR_COLOR);
	}
	myWasActivated = false;
}


// recompute myDesignCoefX,myDesignCoefY;
void RenderingScreen::InitModifiable()
{
	kstl::set<CoreModifiable*>	instances;

    kstl::string parentW=myParentWindowName.const_ref();

	if(parentW != "")
	{
		CoreModifiable::GetInstancesByName("Window",parentW,instances);
		MyParentWindow=(Window*)(*instances.begin());
	}

	// I want to be notified on brightness change
	myBrightness.changeNotificationLevel(Owner);

	if (myDesignSizeX == 0 && myDesignSizeY == 0)
	{
		myDesignSizeX = mySizeX;
		myDesignSizeY = mySizeY;
	}
	else
	{
		if (myDesignSizeX == 0)
		{
			myDesignSizeX = myDesignSizeY * mySizeX / mySizeY;
		}
		else if (myDesignSizeY == 0)
		{
			myDesignSizeY = myDesignSizeX * mySizeY / mySizeX;
		}
	}

	// recompute myDesignCoefX,myDesignCoefY;
	mySizeX.changeNotificationLevel(Owner);
	mySizeY.changeNotificationLevel(Owner);
	myDesignSizeX.changeNotificationLevel(Owner);
	myDesignSizeY.changeNotificationLevel(Owner);

	RecomputeDesignCoef();
	CoreModifiable::InitModifiable();
}

void RenderingScreen::NotifyUpdate(const unsigned int  labelid )
{
	if(	(labelid==mySizeX.getLabelID()) ||
		(labelid==mySizeY.getLabelID()) ||
		(labelid==myDesignSizeX.getLabelID()) ||
		(labelid==myDesignSizeY.getLabelID()) )
	{
		RecomputeDesignCoef();
	}

	CoreModifiable::NotifyUpdate(labelid);
}

void RenderingScreen::RecomputeDesignCoef()
{
	myDesignCoefX=((kfloat)(int)myDesignSizeX)/((kfloat)(int)mySizeX);
	myDesignCoefY=((kfloat)(int)myDesignSizeY)/((kfloat)(int)mySizeY);
}

v2f RenderingScreen::GlobalMousePositionToDesignPosition(v2i pos)
{
	v2f result{ -1, -1 };
	if (MyParentWindow != 0)
	{
		MyParentWindow->GetMousePosInWindow(pos.x, pos.y, result.x, result.y);
		auto size = MyParentWindow->GetSize();
		result.x = myDesignSizeX * result.x / size.x;
		result.y = myDesignSizeY * result.y / size.y;
	}
	return result;
}

void RenderingScreen::GetMousePosInScreen(int posx,int posy,kfloat& sposx,kfloat& sposy)
{
	if(MyParentWindow !=0)
		MyParentWindow->GetMousePosInDesignWindow(posx,posy,sposx,sposy);
}

void RenderingScreen::GetMouseRatioInScreen(int posx,int posy,kfloat& sposx,kfloat& sposy)
{
	if(MyParentWindow !=0)
	{
		MyParentWindow->GetMousePosInDesignWindow(posx, posy, sposx, sposy);
		/*int L_WinSizeX, L_WinSizeY = 0;
		MyParentWindow->getValue(LABEL_TO_ID(SizeX),L_WinSizeX);
		MyParentWindow->getValue(LABEL_TO_ID(SizeY),L_WinSizeY);*/
		sposx -= ((int)myDesignSizeX)*0.5f;
		sposy -= ((int)myDesignSizeY)*0.5f;
		sposx /= (float)myDesignSizeX;
		sposy /= (float)myDesignSizeY;
	}
}

void RenderingScreen::GetMouseRatio(int posx,int posy,kfloat& sposx,kfloat& sposy)
{
	if(MyParentWindow !=0)
	{
		int L_WinSizeX, L_WinSizeY = 0;
		MyParentWindow->getValue(LABEL_TO_ID(SizeX),L_WinSizeX);
		MyParentWindow->getValue(LABEL_TO_ID(SizeY),L_WinSizeY);
		sposx = posx-(int)L_WinSizeX*0.5f;
		sposy = posy-(int)L_WinSizeY*0.5f;
		sposx /= L_WinSizeX;
		sposy /= L_WinSizeY;
	}
}

bool RenderingScreen::MouseIsInScreen(int posx,int posy)
{
	if(MyParentWindow !=0)
	{
		int L_WinSizeX, L_WinSizeY, L_PosX, L_PosY;
		MyParentWindow->getValue(LABEL_TO_ID(PositionX),L_PosX);
		MyParentWindow->getValue(LABEL_TO_ID(PositionY),L_PosY);
		MyParentWindow->getValue(LABEL_TO_ID(SizeX),L_WinSizeX);
		MyParentWindow->getValue(LABEL_TO_ID(SizeY),L_WinSizeY);

		if( (posx < L_PosX) || (posx > (L_PosX+L_WinSizeX)) ||
			(posy < L_PosY) || (posy > (L_PosY+L_WinSizeY)) )
			return false;
	}
	return true;
}

void RenderingScreen::GetMousePosInDesignScreen(int posx,int posy,kfloat& sposx,kfloat& sposy)
{
	sposx=((kfloat)posx)*myDesignCoefX;
	sposy=((kfloat)posy)*myDesignCoefY;
}

void RenderingScreen::GetMouseMoveInScreen(kfloat posx,kfloat posy,kfloat& sposx,kfloat& sposy)
{
	sposx=posx*myDesignCoefX;
	sposy=posy*myDesignCoefY;
}

bool RenderingScreen::SetActive(TravState* state)
{
	if (mySizeX == -1 || mySizeY == -1)
		return false;

	myWasActivated = true;

	if (state)
	{
		state->GetRenderer()->addActivatedRenderingScreen(this);
	}
	return true;
}

DEFINE_METHOD(RenderingScreen, ResetContext)
{
	ReInit();

	ModuleSpecificRenderer* renderer = ModuleRenderer::theGlobalRenderer; //((ModuleRenderer*)KigsCore::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->FlushState(true);

	SP<TextureFileManager>	fileManager = KigsCore::GetSingleton("TextureFileManager");
	fileManager->ResetAllTexture();

	KigsCore::GetNotificationCenter()->postNotificationName("ResetContext", this);
	return true;
}

bool RenderingScreen::GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout)
{
	pout.pos.z = 0;

	if (myIsOffScreen)
	{
		kfloat cx = (kfloat)(unsigned int)myDesignSizeX;
		kfloat cy = (kfloat)(unsigned int)myDesignSizeY;

		pout.pos.x = posin.pos.x*cx;
		pout.pos.y = posin.pos.y*cy;
	}
	else
	{
		//GetMousePosInScreen((s32)posin.pos.x, (s32)posin.pos.y, pout.pos.x, pout.pos.y);
		pout.pos.xy = GlobalMousePositionToDesignPosition(posin.pos.xy);
		pout.pos.z = 0;
	}

	if ((pout.pos.x >= 0.0f) && (pout.pos.x < (kfloat)(unsigned int)myDesignSizeX))
	{
		if ((pout.pos.y >= 0.0f) && (pout.pos.y < (kfloat)(unsigned int)myDesignSizeY))
		{
			return true;
		}
	}
	return false;
}

bool RenderingScreen::IsValidTouchSupport(Vector3D posin)
{
	if (myIsOffScreen) // offscreen can not be a root touch support
	{
		return false;
	}
	Vector3D pout;
	GetMousePosInScreen((s32)posin.x, (s32)posin.y, pout.x, pout.y);

	// return true if pos is inside screen
	if ((pout.x >= 0) && (pout.x < myDesignSizeX) && (pout.y >= 0) && (pout.y < myDesignSizeY))
	{
		return true;
	}
	return false;
}

void RenderingScreen::ManageFade(TravState* state)
{
	if (myBrightness != 0)
	{
		setCurrentContext();

		ModuleSpecificRenderer* renderer = static_cast<ModuleSpecificRenderer*>(ModuleRenderer::theGlobalRenderer);

		renderer->pushShader((ShaderBase*)renderer->getDefaultUiShader().get(), state);

		unsigned int shader_flag = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;
		shader_flag |= ModuleRenderer::COLOR_ARRAY_MASK;

		renderer->GetActiveShader()->ChooseShader(state, shader_flag);

		renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA, RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
		renderer->SetBlendMode(RENDERER_BLEND_ON);
		renderer->SetLightMode(RENDERER_LIGHT_OFF);
		renderer->SetDepthTestMode(false);
		renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_OFF);
		renderer->Ortho(MATRIX_MODE_PROJECTION, 0.0f, (float)myDesignSizeX, 0.0f, (float)myDesignSizeY, -1.0f, 1.0f);
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
		buf[1].setVertex((float)myDesignSizeX, 0.0);
		buf[3].setVertex((float)myDesignSizeX, (float)myDesignSizeY);
		buf[2].setVertex(0.0, (float)myDesignSizeY);

		//Draw Quad that fits the screen
		if (myBrightness > 0)
		{
			unsigned char alpha = (unsigned char)((((float)myBrightness) / 16.0f) * 255.0f);
			buf[0].setColor(255, 255, 255, alpha);
			buf[1].setColor(255, 255, 255, alpha);
			buf[2].setColor(255, 255, 255, alpha);
			buf[3].setColor(255, 255, 255, alpha);
		}
		else
		{
			unsigned char alpha = (unsigned char)((((float)-myBrightness) / 16.0f) * 255.0f);
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