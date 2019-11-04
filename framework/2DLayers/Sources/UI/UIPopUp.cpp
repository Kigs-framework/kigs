#include "UI/UIPopUp.h"
#include "NotificationCenter.h"
#include "Timer.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIPopUp, UIPopUp, 2DLayers);
IMPLEMENT_CLASS_INFO(UIPopUp)

IMPLEMENT_CONSTRUCTOR(UIPopUp)
{
	KigsCore::GetNotificationCenter()->addObserver(this, "HidePopUp", "HidePopUp");
	KigsCore::GetNotificationCenter()->addObserver(this, "ShowPopUp", "ShowPopUp");
}

UIPopUp:: ~UIPopUp()
{
	Hide();
}

DEFINE_METHOD(UIPopUp, HidePopUp)
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

	if (atoi(tmp.c_str()) == myNumSignalPopUp.const_ref())
	{
		Hide();
		// catch if only me to hide
		return true;
	}
	else if (tmp == "true" && sender != this)
	{
		Hide();
	}


	return false;
}

DEFINE_METHOD(UIPopUp, ShowPopUp)
{
	if (!params.empty())
	{
		kstl::string tmp;
		params[0]->getValue(tmp);
		
		if (atoi(tmp.c_str()) == myNumSignalPopUp.const_ref())
		{
			if (!myIsHidden)
			{
				Hide();
				return false;
			}
			kstl::vector<CoreModifiableAttribute*> mySendParams;
			mySendParams.push_back(&myCloseAll);
			KigsCore::GetNotificationCenter()->postNotificationName("HidePopUp", mySendParams, this);

			if (sender)
			{
				Show(sender);
			}
		}
		return false;
	}
	return false;
}

void UIPopUp::Update(const Timer &timer,void* addParam)
{
	kdouble t = timer.GetTime();
	if (myOpenPopup)
	{
		myTimeOpen = t;
		myOpenPopup = false;
	}
	if (t - myTimeOpen >= myTimeStayOpen)
	{
		Hide();
		myTimeOpen = -1.0;
	}
}
void UIPopUp::Show(CoreModifiable * aActivator)
{
	myIsHidden = false;
	myOpenPopup = true;
	if (aActivator)
	{
		myActivator = aActivator;
		aActivator->GetRef();
		myActivator->setArrayValue("Color", &myActiveColor[0], 3);
	}
}

void UIPopUp::Hide()
{
	myIsHidden = true;
	if (myActivator)
	{
		myActivator->setArrayValue("Color", &myUsedColor[0], 3);
		myActivator->Destroy();
		myActivator = nullptr;
	}
}


