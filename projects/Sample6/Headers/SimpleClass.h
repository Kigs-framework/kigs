#pragma once
#include "CoreModifiable.h"
#include "AttributePacking.h"

class SimpleClass : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(SimpleClass, CoreModifiable, Application);
	DECLARE_CONSTRUCTOR(SimpleClass);

protected:

	void InitModifiable() override;

	// declare two signals
	SIGNALS(SendSignal1,SendSignal2);

	// Wrapped method
	void	doSomethingElse();
	WRAP_METHODS(doSomethingElse);

	// fixed prototype CoreModifiable method
	DECLARE_METHOD(doSomething);

};

