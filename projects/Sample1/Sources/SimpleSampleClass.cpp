#include "SimpleSampleClass.h"
#include "AttributePacking.h"

using namespace Kigs;
using namespace Kigs::Core;


IMPLEMENT_CLASS_INFO(SimpleSampleClass)

void SimpleSampleClass::InitModifiable()
{
	
}


DEFINE_METHOD(SimpleSampleClass,AddValue)
{
	float total = 0.0f;
	CoreModifiableAttribute* presult=nullptr;
	for (auto p : params)
	{
		if (p->getLabelID() == KigsID("Result"))
		{
			presult = p;
		}
		else
		{
			float val=0.f;
			p->getValue(val);
			total += val;
		}
	}

	if (!presult)
	{
		PUSH_RETURN_VALUE(total);
	}
	else
	{
		presult->setValue(total);
	}

	return true;
}