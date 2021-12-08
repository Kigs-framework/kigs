#include "Ticker.h"
#include "CoreBaseApplication.h"

// connect to events and create attributes
void	TickerUpgrador::Init(CoreModifiable* toUpgrade)
{
	// catch notifyUpdate
	KigsCore::Connect(toUpgrade, "NotifyUpdate", toUpgrade, "TickerNotifyUpdate");

	mTickerFunction = (maString*)toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::STRING, "TickerFunction", "");
	mTickerFunction->changeNotificationLevel(Owner);
	mTickerFrequency = (maFloat*)toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::FLOAT, "TickerFrequency", 1.0f);

	// check if already in auto update mode
	if (toUpgrade->isFlagAsAutoUpdateRegistered())
		mWasdAutoUpdate = true;
	else
		KigsCore::GetCoreApplication()->AddAutoUpdate(toUpgrade);

	std::string func = (*mTickerFunction);
	if (func.length())
	{
		mLastUpdate = KigsCore::GetCoreApplication()->GetApplicationTimer()->GetTime();
		toUpgrade->SimpleCall(func);
	}

}

//  remove dynamic attributes and disconnect events
void	TickerUpgrador::Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted)
{
	if (toDowngradeDeleted) return;

	KigsCore::Disconnect(toDowngrade, "NotifyUpdate", toDowngrade, "TickerNotifyUpdate");

	toDowngrade->RemoveDynamicAttribute("TickerFunction");
	toDowngrade->RemoveDynamicAttribute("TickerFrequency");

	if (!mWasdAutoUpdate)
		KigsCore::GetCoreApplication()->RemoveAutoUpdate(toDowngrade);
}

void	TickerUpgrador::NotifyUpdate(const unsigned int  labelid , CoreModifiable* parent)
{
	if (labelid == mTickerFunction->getLabelID())
	{
		std::string func = (*mTickerFunction);
		if (func.length())
		{
			if (mLastUpdate < 0.0)
			{
				mLastUpdate = KigsCore::GetCoreApplication()->GetApplicationTimer()->GetTime();
				parent->SimpleCall(func);
			}
		}
	}
}


void	TickerUpgrador::Update(const Timer& _timer, CoreModifiable* parent)
{
	std::string func = (*mTickerFunction);
	if (func.length())
	{
		double currentTime = _timer.GetTime();
		double L_delta = currentTime - mLastUpdate;
		float dt = 1.0f / (float)(*mTickerFrequency);
		
		double LastUpdateDt = 0.0;
		
		while (L_delta > dt)
		{
			parent->SimpleCall(func);
			L_delta -= dt;
			LastUpdateDt += dt;
		}
		mLastUpdate += LastUpdateDt;
	}

}



DEFINE_UPGRADOR_UPDATE(TickerUpgrador)
{
	TickerUpgrador* currentTicker = static_cast<TickerUpgrador*>(GetUpgrador());
	currentTicker->Update(timer, this);
	return false;
}

DEFINE_UPGRADOR_METHOD(TickerUpgrador, TickerNotifyUpdate)
{
	if (!params.empty())
	{
		u32 labelID;
		params[1]->getValue(labelID);
		TickerUpgrador* currentTicker = static_cast<TickerUpgrador*>(GetUpgrador());
		currentTicker->NotifyUpdate(labelID, this);
	}
	return false;
}