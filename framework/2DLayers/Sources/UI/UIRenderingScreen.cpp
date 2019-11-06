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
		CoreModifiable* rs = (CoreModifiable*)myRenderingScreen;
		if (rs)
		{
			TextureFileManager* textureManager = (TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");

			myTexture = NonOwningRawPtrToSmartPtr(rs->as<RenderingScreen>()->GetFBOTexture());
			myTexture->setValue("TransparencyType",2);
			myTexture->setValue("ForceNearest", (bool)myForceNearest);
			myForceNearest.changeNotificationLevel(Owner);

			kfloat sx, sy;

			myTexture->GetSize(sx, sy);

			kfloat ratioX, ratioY;
			myTexture->GetRatio(ratioX, ratioY);

			kfloat dx = 0.5f / sx;
			kfloat dy = 0.5f / sy;

			VInfo2D::Data* buf = static_cast<VInfo2D::Data*>(mVI.Buffer());

			// flip 
			// triangle strip order
			buf[0].setTexUV(dx, ratioY - dy);
			buf[1].setTexUV(dx, dy);
			buf[3].setTexUV(ratioX - dx, dy);
			buf[2].setTexUV(ratioX - dx, ratioY - dy );

			mVI.Flag |= UIVerticesInfo_Texture;

			//myTexture->Init();

			// declare as a touch support potential target 
			// search parent rendering screen
		
			mybIsTouchable.changeNotificationLevel(Owner);

			// Inputs
			if (mybIsTouchable)
			{
				CheckUniqueObject layerRenderingScreen;
				parent2DLayer->getValue("RenderingScreen", layerRenderingScreen);

				ModuleInput* theInputModule = KigsCore::GetModule<ModuleInput>();
				theInputModule->getTouchManager()->addTouchSupport(this, (CoreModifiable*)(RefCountedClass*)layerRenderingScreen);

				// add offscreen rendering screen as touch support with this as parent
				theInputModule->getTouchManager()->addTouchSupport(myRenderingScreen, this);
			}
		}
	}
}

void UIRenderingScreen::NotifyUpdate(unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);
	if (labelid == mybIsTouchable.getID())
	{
		ModuleInput* theInputModule = KigsCore::GetModule<ModuleInput>();
		if (!mybIsTouchable)
		{
			theInputModule->getTouchManager()->removeTouchSupport(this);
		}
		else
		{
			BaseUI2DLayer* parent2DLayer = getParentLayer();

			if (parent2DLayer)
			{
				CheckUniqueObject	layerRenderingScreen;
				parent2DLayer->getValue("RenderingScreen", layerRenderingScreen);
				theInputModule->getTouchManager()->addTouchSupport(this, (CoreModifiable*)(RefCountedClass*)layerRenderingScreen);
				// add offscreen rendering screen as touch support with this as parent
				theInputModule->getTouchManager()->addTouchSupport(myRenderingScreen, this);
			}
		}
	}
	else if (labelid == myForceNearest.getID())
	{
		myTexture->setValue("ForceNearest", (bool)myForceNearest);
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