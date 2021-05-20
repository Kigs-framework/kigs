#pragma once
#include "CoreModifiable.h"
#include "Upgrador.h"
#include "Timer.h"

class TickerUpgrador : public Upgrador<CoreModifiable>
{
public:

	// create and init Upgrador if needed and add dynamic attributes
	virtual void	Init(CoreModifiable* toUpgrade) override;

	// destroy UpgradorData and remove dynamic attributes 
	virtual void	Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted) override;

	START_UPGRADOR(TickerUpgrador);

	UPGRADOR_METHODS(TickerNotifyUpdate);
protected:
	void	Update(const Timer& _timer, CoreModifiable* parent);
	void	NotifyUpdate(const unsigned int /* labelid */, CoreModifiable* parent);

	maFloat*	mTickerFrequency = nullptr;
	maString*	mTickerFunction = nullptr;
	
	double		mLastUpdate=-1.0;

	bool		mWasdAutoUpdate = false;
};
