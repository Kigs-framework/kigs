#pragma once

#include "CoreModifiable.h"
#include "Upgrador.h"
#include "Timer.h"

class DelayedFuncUpgrador : public Upgrador<CoreModifiable>
{
public:

	// create and init Upgrador if needed and add dynamic attributes
	virtual void	Init(CoreModifiable* toUpgrade) override;

	// destroy UpgradorData and remove dynamic attributes 
	virtual void	Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted) override;

	START_UPGRADOR(DelayedFuncUpgrador);

	UPGRADOR_WITHOUT_METHODS();

protected:
	bool	Update(const Timer& _timer, CoreModifiable* parent);
	

	maFloat* mDelay = nullptr;
	maString* mDelayedFunction = nullptr;

	double		mInitTime = -1.0;

	bool		mWasdAutoUpdate = false;
};
