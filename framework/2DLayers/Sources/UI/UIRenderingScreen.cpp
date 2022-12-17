#include "UI/UIRenderingScreen.h"
#include "TextureFileManager.h"
#include "ModuleInput.h"
#include "RenderingScreen.h"

#include "BaseUI2DLayer.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIRenderingScreen, UIRenderingScreen, 2DLayers);
IMPLEMENT_CLASS_INFO(UIRenderingScreen)

IMPLEMENT_CONSTRUCTOR(UIRenderingScreen)
{
}

void UIRenderingScreen::SetTexUV(UIVerticesInfo * aQI)
{
	aQI->Flag |= UIVerticesInfo_Texture;
}

void	UIRenderingScreen::InitModifiable()
{
	BaseUI2DLayer* parent2DLayer = getParentLayer();
	if (parent2DLayer == 0)
	{
		return;
	}

	UITexturedItem::InitModifiable();

	if (IsInit())
	{
		CoreModifiable* rs = (CoreModifiable*)mRenderingScreen;
		if (rs)
		{
			auto textureManager = KigsCore::Singleton<TextureFileManager>();

			auto texture = rs->as<RenderingScreen>()->GetFBOTexture();
			if (!texture)
			{
				UninitModifiable();
				return;
			}
			mTexturePointer->setTexture(texture);
			mTexturePointer->setValue("TransparencyType",2);
			mTexturePointer->setValue("ForceNearest", (bool)mForceNearest);
			mTexturePointer->refreshTextureInfos();
			mForceNearest.changeNotificationLevel(Owner);

			float sx, sy;
			mTexturePointer->GetSize(sx, sy);

			float ratioX, ratioY;
			mTexturePointer->GetRatio(ratioX, ratioY);

			float dx = 0.5f / sx;
			float dy = 0.5f / sy;

			VInfo2D::Data* buf = static_cast<VInfo2D::Data*>(mVI.Buffer());

			// flip 
			// triangle strip order
			buf[0].setTexUV(dx, ratioY - dy);
			buf[1].setTexUV(dx, dy);
			buf[3].setTexUV(ratioX - dx, dy);
			buf[2].setTexUV(ratioX - dx, ratioY - dy );

			mVI.Flag |= UIVerticesInfo_Texture;

			setUserFlag(Node2D::Node2D_NeedVerticeInfoUpdate);

			//mTexturePointer->Init();

			// declare as a touch support potential target 
			// search parent rendering screen
		
			mIsTouchable.changeNotificationLevel(Owner);

			// Inputs
			if (mIsTouchable)
			{
				CoreModifiable* layerRenderingScreen;
				parent2DLayer->getValue("RenderingScreen", layerRenderingScreen);

				auto theInputModule = KigsCore::GetModule<ModuleInput>();
				theInputModule->getTouchManager()->addTouchSupport(this, layerRenderingScreen);

				// add offscreen rendering screen as touch support with this as parent
				theInputModule->getTouchManager()->addTouchSupport(mRenderingScreen, this);
			}
		}
	}
}

void UIRenderingScreen::NotifyUpdate(unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);
	if (labelid == mIsTouchable.getID())
	{
		auto theInputModule = KigsCore::GetModule<ModuleInput>();
		if (!mIsTouchable)
		{
			theInputModule->getTouchManager()->removeTouchSupport(this);
		}
		else
		{
			BaseUI2DLayer* parent2DLayer = getParentLayer();

			if (parent2DLayer)
			{
				CoreModifiable* layerRenderingScreen;
				parent2DLayer->getValue("RenderingScreen", layerRenderingScreen);
				theInputModule->getTouchManager()->addTouchSupport(this, layerRenderingScreen);
				// add offscreen rendering screen as touch support with this as parent
				theInputModule->getTouchManager()->addTouchSupport(mRenderingScreen, this);
			}
		}
	}
	else if (labelid == mForceNearest.getID())
	{
		mTexturePointer->setValue("ForceNearest", (bool)mForceNearest);
	}
}


bool	UIRenderingScreen::GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout)
{
	SetUpNodeIfNeeded();

	Point2D pt[4];
	GetTransformedPoints(pt);

	// compute pos
	Point2D L_PA(posin.pos.x - pt[0].x, posin.pos.y - pt[0].y);
	Point2D L_PQ = pt[3] - pt[0];
	Point2D L_PR = pt[1] - pt[0];

	float d = (L_PQ.x*L_PR.y - L_PR.x*L_PQ.y);
	float n = -(L_PA.x*L_PQ.y - L_PQ.x*L_PA.y) / d;
	float m = (L_PA.x*L_PR.y - L_PR.x*L_PA.y) / d;

	pout.pos.z = 0.0f;

	pout.pos.x = m;
	pout.pos.y = n;


	if ((pout.pos.x >= 0.0f) && (pout.pos.x < 1.0f))
	{
		if ((pout.pos.y >= 0.0f) && (pout.pos.y < 1.0f))
		{
			return true;
		}
	}
	return false;
}