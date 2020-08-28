#pragma once
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "maReference.h"


class SimpleClass : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(SimpleClass, CoreModifiable, Application);
	DECLARE_CONSTRUCTOR(SimpleClass);

	void	DoSomethingFun();

protected:

	virtual void NotifyUpdate(const u32 labelid) override;

	maInt			mIntValue;
	maString		mStringValue;
	maReference		mRef = BASE_ATTRIBUTE(Reference,"");

	maVect4DF		mVector4D = BASE_ATTRIBUTE(Vector, 0.0,0.0,1.0,2.0);
};

