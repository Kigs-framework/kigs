#include "..\..\..\Headers\CustomUI\SequenceMap\UIMap.h"
#include "..\..\..\Headers\CustomUI\SequenceMap\UIPointInteret.h"
#include "..\..\..\Headers\CustomUI\SequenceMap\PointInteretVignette.h"
#include "Core.h"
#include "InstanceFactory.h"
#include "UI\UIText.h"
#include "NotificationCenter.h"
#include "2DLayersIncludes.h"
#include "NotificationCenter.h"

IMPLEMENT_CLASS_INFO(UIPointInteret);

UIPointInteret::UIPointInteret(const kstl::string & name, CLASS_NAME_TREE_ARG) :
	UIButtonImage(name, PASS_CLASS_NAME_TREE_ARG)
	, mCoordGPS_Lat(*this, false, LABEL_AND_ID(GPSCoord_Lat), 0)
	, mCoordGPS_Long(*this, false, LABEL_AND_ID(GPSCoord_Long), 0)
	, mNameOfInterestPoint(*this, false, LABEL_AND_ID(NameOfInterestPoint), "")
	, mDescriptionOfInterestPoint(*this, false, LABEL_AND_ID(DescriptionOfInterestPoint), "")
	, mConditionOfActivation(*this, false, LABEL_AND_ID(ConditionOfActivation), "Distance")
	, mDistanceOfActivation(*this, false, LABEL_AND_ID(DistanceOfActivation), 20)
	, mActionOfActivation(*this, false, LABEL_AND_ID(ActionOfActivation), "")
	, mActionOfDeactivation(*this, false, LABEL_AND_ID(ActionOfDeactivation), "")
	, mParameterOfActivation(*this, false, LABEL_AND_ID(ParameterOfActivation), (kstl::string)"")
	, mParameterOfDesactivation(*this, false, LABEL_AND_ID(ParameterOfDesactivation), (kstl::string) "")
	, mTitleColor(*this, false, LABEL_AND_ID(TitleColor), KFLOAT_ONE, KFLOAT_ONE, KFLOAT_ONE, KFLOAT_ZERO)
	, mTitleFontName(*this, false, LABEL_AND_ID(TitleFontName), "arial.ttf")
	, mTitleFontSize(*this, false, LABEL_AND_ID(TitleFontSize), 30)
	, mShowTitle(*this, false, LABEL_AND_ID(ShowTitle), true)
	, mNameText(nullptr)
	, mVignette(nullptr)
	, mVignetteParameters(*this, false, LABEL_AND_ID(VignetteParameters), "")
	, mFeedbackOnActivation(*this, false, LABEL_AND_ID(FeedbackOnActivation), false)
	, mInactivate(false)
	, mIsTitleShown(false)
	, mCanBeActivatedByClick(false)
{
	mCoordGPS_Lat.changeNotificationLevel(Owner);
	mCoordGPS_Long.changeNotificationLevel(Owner);
	mDescriptionOfInterestPoint.changeNotificationLevel(Owner);


	mNameText = KigsCore::GetInstanceOf(getName() + "_" + (kstl::string)mNameOfInterestPoint + "_NamePI", "UIText");
	mNameText->setValue(LABEL_TO_ID(IsTouchable), false);

	mAnchor.setValue("{0.5,0.5}");

	KigsCore::Connect(this, "ClickUp", this, "ClickAction");
}

bool UIPointInteret::ClickAction(CoreModifiable* sender, usString param)
{
	if (mCanBeActivatedByClick)
	{
		((UIMap*)mParent)->ActivatePointInteret(this);
		return true;
	}
	else
		return false;
}

void UIPointInteret::InitModifiable()
{
	UIButtonImage::InitModifiable();

	if (_isInit && mParent && mParent->IsInit())
	{

		if (mActionOfActivation.const_ref() == "ShowDescription")
		{
			KigsCore::GetNotificationCenter()->addObserver(this, "HideVignette", "HideVignette");
		}

		((UIMap*)mParent)->AddPointOfInterest(this);
		RecalculatePosition();

		if (mShowTitle)
		{

			mNameText->setValue("IsHidden",false);
			mNameText->SetColor(mTitleColor[0], mTitleColor[1], mTitleColor[2], 1);
			mNameText->Set_FontSize(mTitleFontSize);
			mNameText->Set_FontName(mTitleFontName);
			mNameText->setValue("Text", mNameOfInterestPoint);

			mNameText->SetAlignment(1);
			mNameText->setArrayValue(LABEL_TO_ID(Dock), 0.5, 0.5);
			mNameText->setArrayValue(LABEL_TO_ID(Anchor), 0.5, 0.5);

			addItem((CMSP&)mNameText);
			mIsTitleShown = true;
		}

		mTitleColor.changeNotificationLevel(Owner);
		mTitleFontSize.changeNotificationLevel(Owner);
		mTitleFontName.changeNotificationLevel(Owner);
		mShowTitle.changeNotificationLevel(Owner);
		mNameOfInterestPoint.changeNotificationLevel(Owner);
	}
	else
	{
		UninitModifiable();
	}
}

void    UIPointInteret::ProtectedDestroy()
{
	mNameText = nullptr;
	UIButtonImage::ProtectedDestroy();
}

kdouble UIPointInteret::DistanceToUser(kdouble _lat, kdouble _long)
{
	kdouble rlat1 = PI_180 * mCoordGPS_Lat;
	kdouble rlat2 = PI_180 * _lat;
	kdouble rlon1 = PI_180 * mCoordGPS_Long;
	kdouble rlon2 = PI_180 * _long;

	kdouble theta = mCoordGPS_Long - _long;
	kdouble rtheta = PI_180 * theta;

	kdouble dist = sin(rlat1) * sin(rlat2) + cos(rlat1) * cos(rlat2) * cos(rtheta);
	dist = acos(dist);
	dist = dist * 180 / PI;
	dist = dist * 60 * 1.1515;


	dist = dist * 1.609344 * 1000;

	return dist;
}

bool UIPointInteret::Get_VignetteIsHidden() { return mVignette->Get_IsHidden(); }

void UIPointInteret::NotifyUpdate(const unsigned int 	labelid)
{
	if ((labelid == mCoordGPS_Lat.getLabelID()) || (labelid == mCoordGPS_Long.getLabelID()))
	{
		RecalculatePosition();
	}
	else if (labelid == mTitleColor.getLabelID()
		|| labelid == mTitleFontName.getLabelID()
		|| labelid == mTitleFontSize.getLabelID()
		|| labelid == mShowTitle.getLabelID()
		|| labelid == mNameOfInterestPoint.getLabelID())
	{
		if (mShowTitle)
		{
			if (!mIsTitleShown)
				addItem((CMSP&)mNameText);

			mNameText->setValue("IsHidden", false);
			mNameText->SetColor(mTitleColor[0], mTitleColor[1], mTitleColor[2], 1);
			mNameText->Set_FontSize(mTitleFontSize);
			mNameText->Set_FontName(mTitleFontName);
			mNameText->setValue("Text", mNameOfInterestPoint);

			v2f size = mNameText->GetSize();
			mNameText->Set_Position(-(size.x / 2) + ((int)mSizeX / 2), -size.y + 5);
			mIsTitleShown = true;
		}
		else if (mIsTitleShown)
			removeItem((CMSP&)mNameText);

		mIsTitleShown = mShowTitle;
	}
	else if (labelid == mActionOfActivation.getLabelID())
	{
		if (mActionOfActivation.const_ref() == "ShowDescription")
		{
			KigsCore::GetNotificationCenter()->addObserver(this, "HideVignette", "HideVignette");
		}
		else
		{
			KigsCore::GetNotificationCenter()->removeObserver(this, "HideVignette");
		}
	}

	Node2D::NotifyUpdate(labelid);
}

void UIPointInteret::SetParent(CoreModifiable* value)
{
	UIButtonImage::SetParent(value);
	RecalculatePosition();
}

/*bool UIPointInteret::TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)
{
	bool down = buttonState&UIInputEvent::LEFT;
	if (down == true)
	{
		bool b = UIButtonImage::TriggerMouseClick(buttonState, buttonEvent, X, Y, catchClick);
		if (b == true)
			return b;
	}

	kstl::string condition;
	mConditionOfActivation.getValue(condition);
	if (myIsDown && !down && condition == "Click" && mCanBeActivatedByClick)
	{
		((UIMap*)mParent)->ActivatePointInteret(this);
		return UIButtonImage::TriggerMouseClick(buttonState, buttonEvent, X, Y, catchClick);
	}
	else
		return false;
}
*/
void UIPointInteret::ActivePoint(bool active)
{
	if (mActionOfActivation.const_ref() == "ShowDescription")
	{
		if (active)
		{
			if (mFeedbackOnActivation)
			{
				KigsCore::Instance()->GetNotificationCenter()->postNotificationName("AskVibration");
			}

			if (mVignette)
			{
				getRootFather()->removeItem((CMSP&)mVignette);
				mVignette = nullptr;

			}
			else
			{
				mVignette = Import(mParameter.ToString(), false, true);
				if (mVignette)
				{
					mVignette->setName(getName() + "_myVignette");
					getRootFather()->addItem((CMSP&)mVignette);
					mVignette->SetPointInteret(this);
					mVignette->SetVignetteTexts(mNameOfInterestPoint, mDescriptionOfInterestPoint);
					mVignette->SetParameters(mVignetteParameters.const_ref());
				}
			}
		}
		else
		{
			if (mVignette)
			{
				getRootFather()->removeItem((CMSP&)mVignette);
				mVignette = nullptr;

			}
		}
	}
	else if ((mActionOfActivation.const_ref() == "StackSequence" || mActionOfActivation.const_ref() == "ChangeSequence" || mActionOfActivation.const_ref() == "ShowPopUp") && active)
	{
		kstl::vector<kstl::string>	L_ActionVect = SplitStringByCharacter(mActionOfActivation.const_ref(), ':');
		kstl::vector<CoreModifiableAttribute*> mySendParams;
		mySendParams.push_back(&mParameterOfActivation);
		for (unsigned int i = 0; i < L_ActionVect.size(); i++)
		{
			KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], mySendParams, this);

			if (mFeedbackOnActivation)
			{
				KigsCore::Instance()->GetNotificationCenter()->postNotificationName("AskVibration");
			}
		}
	}
	else if (mActionOfActivation.const_ref() == "ShowPopUp" && !active)
	{
		kstl::vector<kstl::string>	L_ActionVect = SplitStringByCharacter("HidePopUp:", ':');
		kstl::vector<CoreModifiableAttribute*> mySendParams;
		mySendParams.push_back(&mParameterOfActivation);
		for (unsigned int i = 0; i < L_ActionVect.size(); i++)
		{
			KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], mySendParams, this);
		}
	}
	else if (active)
	{
		kstl::vector<kstl::string>	L_ActionVect = SplitStringByCharacter(mActionOfActivation.const_ref(), ':');
		kstl::vector<CoreModifiableAttribute*> mySendParams;
		mySendParams.push_back(&mParameterOfActivation);
		for (unsigned int i = 0; i < L_ActionVect.size(); i++)
		{
			KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], mySendParams, this);

			if (mFeedbackOnActivation)
			{
				KigsCore::Instance()->GetNotificationCenter()->postNotificationName("AskVibration");
			}
		}
	}
}

DEFINE_METHOD(UIPointInteret, HideVignette)
{
	if (mVignette)
	{
		getRootFather()->removeItem((CMSP&)mVignette);
		mVignette = nullptr;

	}
	return false;
}

void UIPointInteret::ActivePoint(kdouble coordUserLat, kdouble coordUserLong)
{
	kstl::string condition;
	mConditionOfActivation.getValue(condition);
	if (condition == "Distance")
	{
		double dist = DistanceToUser(coordUserLat, coordUserLong);
		//printf("User at %0.3f m from %s\n", dist, getName().c_str());

		if (mDistanceOfActivation >= dist)
		{
			if (mInactivate)
			{
				ActivePoint(true);
				mInactivate = false;
			}
		}
		else
		{
			if (!mInactivate)
			{
				mInactivate = true;
				ActivePoint(false);
			}
		}
	}
	else
	{
		double dist = DistanceToUser(coordUserLat, coordUserLong);

		if (mDistanceOfActivation >= dist)
		{
			mCanBeActivatedByClick = true;
		}
		else
		{
			mCanBeActivatedByClick = false;
		}
	}
}

void UIPointInteret::RecalculatePosition()
{
	if (mParent && mParent->isSubType("UIMap"))
	{
		//unsigned int xMap = 0, yMap = 0;
		//mParent->GetSize(xMap, yMap);

		double topLeftLong, botRightLong, topLeftLat, bopRightLat;
		((UIMap*)mParent)->UIMap::GetPositionsGPS(topLeftLong, topLeftLat, botRightLong, bopRightLat);

		mDock[0] = (topLeftLong - mCoordGPS_Long) / (topLeftLong - botRightLong);
		mDock[1] = (topLeftLat - mCoordGPS_Lat) / (topLeftLat - bopRightLat);


		/*myPosition[0] = (kfloat)((mCoordGPS_Long - topLeftLong)*xMap / (botRightLong - topLeftLong)) - (mSizeX / 2);
		myPosition[1] = (kfloat)((mCoordGPS_Lat - bopRightLat)*yMap / (topLeftLat - bopRightLat)) - (mySizeY / 2);
		myPosition[1] = yMap - myPosition[1];*/
		SetNodeFlag(Node2D_NeedUpdatePosition);
		PropagateNeedUpdateToFather();
	}
}
