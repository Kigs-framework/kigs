#include "UI/UIPopUp.h"
#include "NotificationCenter.h"
#include "Timer.h"

using namespace Kigs::Draw2D;

void	PopUpUpgrador::Init(CoreModifiable* toUpgrade)
{
	KigsCore::GetNotificationCenter()->addObserver(toUpgrade, "HidePopUp", "HidePopUp");
	KigsCore::GetNotificationCenter()->addObserver(toUpgrade, "ShowPopUp", "ShowPopUp");

	toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::INT, "NumSignal", 0);
	toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::BOOL, "CloseAll", true);
	toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::INT, "StayOpen", 0);
	Point3D p(0.2f, 1.0f, 0.2f);
	toUpgrade->AddDynamicVectorAttribute("ActiveColor",(float*) &p.x,3);
	p.Set(0.4f, 0.4f, 0.4f);
	toUpgrade->AddDynamicVectorAttribute("UsedColor", (float*)&p.x, 3);

}

//  remove dynamic attributes and disconnect events
void	PopUpUpgrador::Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted)
{
	if (toDowngradeDeleted) return;

	Hide((UIItem*)toDowngrade);

	KigsCore::GetNotificationCenter()->removeObserver(toDowngrade, "HidePopUp");
	KigsCore::GetNotificationCenter()->removeObserver(toDowngrade, "ShowPopUp");

	toDowngrade->RemoveDynamicAttribute("NumSignal");
	toDowngrade->RemoveDynamicAttribute("CloseAll");
	toDowngrade->RemoveDynamicAttribute("StayOpen");
	toDowngrade->RemoveDynamicAttribute("ActiveColor");
	toDowngrade->RemoveDynamicAttribute("UsedColor");

}

// HidePopUp slot
DEFINE_UPGRADOR_METHOD(PopUpUpgrador, HidePopUp)
{
	std::string tmp = "";
	if (privateParams != nullptr)
	{
		tmp = static_cast<usString*>(privateParams)->ToString();
	}
	else if (!params.empty())
	{
		// should search for the good param
		params[0]->getValue(tmp, this);
	}

	if (atoi(tmp.c_str()) == getValue<int>("NumSignal"))
	{
		GetUpgrador()->Hide(this);
		// catch if only me to hide
		return true;
	}
	else if (tmp == "true" && sender != this)
	{
		GetUpgrador()->Hide(this);
	}


	return false;
}


// ShowPopUp slot
DEFINE_UPGRADOR_METHOD(PopUpUpgrador, ShowPopUp)
{
	if (!params.empty())
	{
		std::string tmp;
		params[0]->getValue(tmp, this);
		
		if (atoi(tmp.c_str()) == getValue<int>("NumSignal"))
		{
			if (!mIsHidden)
			{
				GetUpgrador()->Hide(this);
				return false;
			}
			std::vector<CoreModifiableAttribute*> sendParams;
			sendParams.push_back(getAttribute("CloseAll"));
			KigsCore::GetNotificationCenter()->postNotificationName("HidePopUp", sendParams, this);

			if (sender)
			{
				GetUpgrador()->Show(this,sender);
			}
		}
		return false;
	}
	return false;
}

// update ( hide popup if it was open for too long ) 
DEFINE_UPGRADOR_UPDATE(PopUpUpgrador)
{
	double t = timer.GetTime();
	if (GetUpgrador()->mOpenPopup)
	{
		GetUpgrador()->mTimeOpen = t;
		GetUpgrador()->mOpenPopup = false;
	}
	if (t - GetUpgrador()->mTimeOpen >= getValue<int>("StayOpen"))
	{
		GetUpgrador()->Hide(this);
		GetUpgrador()->mTimeOpen = -1.0;
	}
	return false;
}

// internal show method
void PopUpUpgrador::Show(UIItem* localthis, CoreModifiable * aActivator)
{
	localthis->setValue("IsHidden",false);
	mOpenPopup = true;
	if (aActivator)
	{
		mActivator = aActivator->SharedFromThis();
		v3f activeColor;
		localthis->getValue("ActiveColor", activeColor);
		mActivator->setValue("Color", activeColor);
	}
}


// internal hide method
void PopUpUpgrador::Hide(UIItem* localthis)
{
	localthis->setValue("IsHidden", true);
	if (mActivator)
	{
		v3f usedColor;
		localthis->getValue("UsedColor", usedColor);
		mActivator->setValue("Color", usedColor);
		mActivator = nullptr;
	}
}


