#include "..\..\..\Headers\CustomUI\SequenceMap\UIMap.h"
#include "..\..\..\Headers\CustomUI\SequenceMap\PointInteretVignette.h"
#include "..\..\..\Headers\CustomUI\SequenceMap\UIPointInteret.h"
#include "Core.h"
#include "UI\UIText.h"
#include "UI\UIButtonImage.h"
#include "NotificationCenter.h"


#ifdef TODO
#include "UI\UIVideo.h"
#endif

using namespace Kigs::Gps;

IMPLEMENT_CLASS_INFO(PointInteretVignette);

PointInteretVignette::PointInteretVignette(const std::string & name, CLASS_NAME_TREE_ARG) :
	UIImage(name, PASS_CLASS_NAME_TREE_ARG)
	, mFontTitleName(*this, false, "FontTitleName", "arial.ttf")
	, mTitleFontSize(*this, false, "TitleFontSize", 40)
	, mFontDescriptionName(*this, false, "FontDescriptionName", "arial.ttf")
	, mDescriptionFontSize(*this, false, "DescriptionFontSize", 30)
	, mDescriptionAlignment(*this, false, "DescriptionAlignment", 1)
	, mDescriptionPositionY(*this, false, "DescriptionPositionY", 60)
	, mDescriptionMaxSizeX(*this, false, "DescriptionMaxSizeX", 300)
	, mPositionOnWindow(*this, false, "PositionOnWindow", "Center")
	, mExitButtonTextureUp(*this, false, "ExitButtonTextureUp", "croix.png")
	, mExitButtonTextureOver(*this, false, "ExitButtonTextureOver", "")
	, mExitButtonTextureDown(*this, false, "ExitButtonTextureDown", "")
	, mTitleColor(*this, false, "TitleColor", 1.0f, 1.0f, 1.0f, 0.0f)
	, mDescriptionColor(*this, false, "DescriptionColor", 1.0f, 1.0f, 1.0f, 0.0f)
	, mPointOfInterest(0)
{
	mSize.changeNotificationLevel(Owner);
	mPositionOnWindow.changeNotificationLevel(Owner);
	mTitleFontSize.changeNotificationLevel(Owner);
	mFontTitleName.changeNotificationLevel(Owner);
	mDescriptionAlignment.changeNotificationLevel(Owner);
	mDescriptionFontSize.changeNotificationLevel(Owner);
	mDescriptionMaxSizeX.changeNotificationLevel(Owner);
	mDescriptionPositionY.changeNotificationLevel(Owner);
	mFontDescriptionName.changeNotificationLevel(Owner);
	mTitleColor.changeNotificationLevel(Owner);
	mDescriptionColor.changeNotificationLevel(Owner);


	mNamePointInteretTextShow = KigsCore::GetInstanceOf(getName() + "_NamePI", "UIText");
	mPriority = -1;
	addItem((CMSP&)mNamePointInteretTextShow);
	
	mDescriptionPointInteretTextShow = KigsCore::GetInstanceOf(getName() + "_DescriptionPI", "UIText");
	addItem((CMSP&)mDescriptionPointInteretTextShow);

	mExitButton = KigsCore::GetInstanceOf(getName() + "_ExitButtonVignette", "UIButtonImage");
	addItem((CMSP&)mExitButton);

}

void PointInteretVignette::SetVignetteTexts(std::string _Name, std::string _Description)
{
	mNameOfInterestPoint = _Name;
	mDescriptionOfInterestPoint = _Description;
}

void PointInteretVignette::PositionVignette()
{
	if (mPositionOnWindow.const_ref() == "Center" || mPointOfInterest == NULL)
	{
		v2f size = getRootFather()->GetSize();
		Set_Position((size.x - mSize[0]) / 2, (size.y - mSize[1]) / 2);
	}
	else if (mPositionOnWindow.const_ref() == "Vignette")
	{
		float xPI, yPI;
		v2f size = mPointOfInterest->GetSize();
		mPointOfInterest->GetPosition(xPI, yPI);
		Set_Position(size.x + xPI, size.y + yPI);
	}
}

void PointInteretVignette::SetParameters(std::string params)
{
	std::vector<std::string> VecStr;

	std::string::size_type stTemp = params.find(',');

	while (stTemp != std::string::npos)
	{
		VecStr.push_back(params.substr(0, stTemp));
		params = params.substr(stTemp + 1);
		stTemp = params.find(',');
	}

	VecStr.push_back(params);

	for (int i = 0; i < VecStr.size(); i++)
	{
		std::string::size_type stTemp = VecStr[i].find(':');
		std::string nameOfItem = VecStr[i].substr(0, stTemp);
		std::string valueOfItem = VecStr[i].substr(stTemp + 1, std::string::npos);

		std::vector<ModifiableItemStruct>::const_iterator it;
		for (it = getItems().begin(); it != getItems().end(); ++it)
		{

			UIItem* vignetteSon = (UIItem*)(*it).mItem.get();
			if (vignetteSon->getName() == nameOfItem)
			{
				if (vignetteSon->isSubType("UIImage"))
				{
					((UIImage *)vignetteSon)->setValue("TextureName", valueOfItem);
				}
				else if (vignetteSon->isSubType("UIText"))
				{
					((UIText *)vignetteSon)->setValue("Text", valueOfItem);
				}
				else if (vignetteSon->isSubType("UIVideo"))
				{
					//VIDEO NOT AVAILABLE YET
				}
			}
		}
	}
}



void PointInteretVignette::PositionTitle()
{
	v2f size = mNamePointInteretTextShow->GetSize();
	mNamePointInteretTextShow->Set_Position((mSize[0] / 2) - (size.x / 2), 5);
}

void PointInteretVignette::PositionDescription()
{
	if (mDescriptionAlignment == 2)
	{
		v2f size = mDescriptionPointInteretTextShow->GetSize();
		mDescriptionPointInteretTextShow->Set_Position(mSize[0] - size.x - 30, mDescriptionPositionY);
	}
	else if (mDescriptionAlignment == 0)
	{
		mDescriptionPointInteretTextShow->Set_Position(30, mDescriptionPositionY);
	}
	else
	{
		v2f size = mDescriptionPointInteretTextShow->GetSize();
		mDescriptionPointInteretTextShow->Set_Position((mSize[0] / 2) - (size.x / 2), mDescriptionPositionY);
	}
}
void PointInteretVignette::NotifyUpdate(const unsigned int 	labelid)
{
	if (labelid == mSize.getLabelID() )
	{
		PositionVignette();
		PositionTitle();
		PositionDescription();
		PositionExitButton();
	}
	else if (labelid == mFontTitleName.getLabelID() || labelid == mTitleFontSize.getLabelID() || labelid == mTitleColor.getLabelID())
	{
		mNamePointInteretTextShow->Set_FontSize(mTitleFontSize);
		mNamePointInteretTextShow->Set_FontName(mFontTitleName);
		if (mNamePointInteretTextShow)
		{
			float R, G, B, A;
			R = mTitleColor[0];
			G = mTitleColor[1];
			B = mTitleColor[2];
			A = 1;
			mNamePointInteretTextShow->SetColor(R, G, B, A);
			mNamePointInteretTextShow->setValue("Text", mNameOfInterestPoint);
		}
		PositionTitle();
	}
	else if (labelid == mDescriptionAlignment.getLabelID() || labelid == mDescriptionFontSize.getLabelID() || labelid == mDescriptionMaxSizeX.getLabelID()
		|| labelid == mDescriptionPositionY.getLabelID() || labelid == mFontDescriptionName.getLabelID() || labelid == mDescriptionColor.getLabelID())
	{
		mDescriptionPointInteretTextShow->setValue("MaxSize", mDescriptionMaxSizeX);
		mDescriptionPointInteretTextShow->Set_FontSize(mDescriptionFontSize);
		mDescriptionPointInteretTextShow->SetAlignment(mDescriptionAlignment);
		mDescriptionPointInteretTextShow->Set_FontName(mFontDescriptionName);
		if (mDescriptionPointInteretTextShow)
		{
			float R, G, B, A;
			R = mDescriptionColor[0];
			G = mDescriptionColor[1];
			B = mDescriptionColor[2];
			A = 1;
			mDescriptionPointInteretTextShow->SetColor(R, G, B, A);
			mDescriptionPointInteretTextShow->setValue("Text", mDescriptionOfInterestPoint);
		}
		PositionDescription();
	}
	else
	{
		UIImage::NotifyUpdate(labelid);
	}
}

void PointInteretVignette::PositionExitButton()
{
	v2f size = mExitButton->GetSize();
	mExitButton->Set_Position(mSize[0] - size.x - 10, 10);

}
void PointInteretVignette::InitModifiable()
{
	UIImage::InitModifiable();

	if (_isInit && mNamePointInteretTextShow->IsInit() && mDescriptionPointInteretTextShow->IsInit() && mExitButton->IsInit())
	{
		if (mPointOfInterest)
		{
			mPointOfInterest->Get_TitleAndDescription(mNameOfInterestPoint, mDescriptionOfInterestPoint);
		}
		else
		{
			mNameOfInterestPoint = "Name Of Point of Interest";
			mDescriptionOfInterestPoint = "";
			for (int i = 0; i < 2; i++)
			{
				mDescriptionOfInterestPoint += "Description description description description description description description description description description";
			}
		}

		PositionVignette();
		float R, G, B, A;
		R = mTitleColor[0];
		G = mTitleColor[1];
		B = mTitleColor[2];
		A = 1;
		mNamePointInteretTextShow->Set_FontSize(mTitleFontSize);
		mNamePointInteretTextShow->Set_FontName(mFontTitleName.const_ref().c_str());
		mNamePointInteretTextShow->SetColor(R, G, B, A);
		mNamePointInteretTextShow->setValue("Text", mNameOfInterestPoint);
		PositionTitle();

		mDescriptionPointInteretTextShow->setValue("MaxSize", mDescriptionMaxSizeX);
		R = mDescriptionColor[0];
		G = mDescriptionColor[1];
		B = mDescriptionColor[2];
		A = 1;
		mDescriptionPointInteretTextShow->Set_FontSize(mDescriptionFontSize);
		mDescriptionPointInteretTextShow->SetAlignment(mDescriptionAlignment);


		mDescriptionPointInteretTextShow->Set_FontName(mFontDescriptionName.const_ref().c_str());
		mDescriptionPointInteretTextShow->SetColor(R, G, B, A);
		mDescriptionPointInteretTextShow->setValue("Text", mDescriptionOfInterestPoint);
		PositionDescription();

		mExitButton->setValue("Size", v2f( 30,30 ));
		mExitButton->ChangeTexture(mExitButtonTextureUp, mExitButtonTextureOver, mExitButtonTextureDown);
		mExitButton->setValue("ClickUpAction","HideVignette");
		PositionExitButton();
	}
	else
	{
		UninitModifiable();
	}
}
