#pragma once
#include "SimpleSampleBaseClass.h"
#include "Core.h"
#include "AttributePacking.h"

class SimpleSampleClass : public SimpleSampleBaseClass
{
public:
	DECLARE_CLASS_INFO(SimpleSampleClass, SimpleSampleBaseClass, Application);
	DECLARE_CONSTRUCTOR(SimpleSampleClass);

protected:
	// simple method
	void	printMessage();
	// ask possible call by name
	WRAP_METHODS(printMessage);
	
	void InitModifiable() override;

	virtual bool addItem(const CMSP& item, ItemPosition pos = Last) override;

	// int CoreModifiable attribute named CountWhenAdded
	maInt	mCountWhenAdded = BASE_ATTRIBUTE(CountWhenAdded,0);
};
