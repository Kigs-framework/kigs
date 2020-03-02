#include "UI/UIPopUp.h"
#include "NotificationCenter.h"
#include "Timer.h"


// connect pop up to events and create attributes
void	PopUpUpgrador::Init(CoreModifiable* toUpgrade)
{
	KigsCore::GetNotificationCenter()->addObserver(toUpgrade, "HidePopUp", "HidePopUp");
	KigsCore::GetNotificationCenter()->addObserver(toUpgrade, "ShowPopUp", "ShowPopUp");

	toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::INT, "NumSignal", 0);
	toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::BOOL, "CloseAll", true);
	toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::INT, "StayOpen", 0);
	toUpgrade->AddDynamicVectorAttribute("ActiveColor",(float*) &Point3D( 0.2f, 1.0f, 0.2f ),3);
	toUpgrade->AddDynamicVectorAttribute("UsedColor", (float*)&Point3D(0.4f, 0.4f, 0.4f), 3);

}

//  remove dynamic attributes and disconnect events
void	PopUpUpgrador::Destroy(CoreModifiable* toDowngrade)
{
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
	kstl::string tmp = "";
	if (privateParams != nullptr)
	{
		tmp = static_cast<usString*>(privateParams)->ToString();
	}
	else if (!params.empty())
	{
		// should search for the good param
		params[0]->getValue(tmp);
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
		kstl::string tmp;
		params[0]->getValue(tmp);
		
		if (atoi(tmp.c_str()) == getValue<int>("NumSignal"))
		{
			if (!myIsHidden)
			{
				GetUpgrador()->Hide(this);
				return false;
			}
			kstl::vector<CoreModifiableAttribute*> mySendParams;
			mySendParams.push_back(getAttribute("CloseAll"));
			KigsCore::GetNotificationCenter()->postNotificationName("HidePopUp", mySendParams, this);

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
	kdouble t = timer.GetTime();
	if (GetUpgrador()->myOpenPopup)
	{
		GetUpgrador()->myTimeOpen = t;
		GetUpgrador()->myOpenPopup = false;
	}
	if (t - GetUpgrador()->myTimeOpen >= getValue<int>("StayOpen"))
	{
		GetUpgrador()->Hide(this);
		GetUpgrador()->myTimeOpen = -1.0;
	}
}

// internal show method
void PopUpUpgrador::Show(UIItem* localthis, CoreModifiable * aActivator)
{
	localthis->setValue("IsHidden",false);
	myOpenPopup = true;
	if (aActivator)
	{
		myActivator = aActivator;
		aActivator->GetRef();

		v3f activeColor;
		localthis->getValue("ActiveColor", activeColor);
		myActivator->setValue("Color", activeColor);
	}
}


// internal hide method
void PopUpUpgrador::Hide(UIItem* localthis)
{
	localthis->setValue("IsHidden", true);
	if (myActivator)
	{
		v3f usedColor;
		localthis->getValue("UsedColor", usedColor);
		myActivator->setValue("Color", usedColor);
		myActivator->Destroy();
		myActivator = nullptr;
	}
}


