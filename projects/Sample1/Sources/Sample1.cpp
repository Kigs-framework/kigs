#include <Sample1.h>
// include sample class
#include "SimpleSampleClass.h"
// include Core.h to access KigsCore functions
#include "Core.h"
// access CoreModifiable
#include "CoreModifiable.h"
// access timer 
#include "Timer.h"

// Kigs framework Sample1 project
// Overall features :
// - instance factory
// - CoreModifiable trees
// - CoreModifiable attributes
// - CoreModifiable methods
// - serialization

IMPLEMENT_CLASS_INFO(Sample1);

IMPLEMENT_CONSTRUCTOR(Sample1)
{

}

void	Sample1::ProtectedInit()
{
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	// declare project class to instance factory
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), SimpleSampleClass, SimpleSampleClass, Application);

	// create an instance of SimpleSampleClass
	CMSP simpleclass = KigsCore::GetInstanceOf("simpleclass", "SimpleSampleClass");


	// Ask for an instance of class Timer called "localtimer"
	CMSP localtimer = KigsCore::GetInstanceOf("localtimer", "Timer");

	// add localtimer to this (this must inherit CoreModifiable too of course)
	simpleclass->addItem(localtimer);
	
	// init localtimer (timer is started)
	localtimer->Init();

	// search all instances of Timer
	std::set<CoreModifiable*> alltimers;
	GetInstances("Timer", alltimers);

	for (auto i : alltimers)
	{
		printf("Timer %s found \n", i->getName().c_str());
	}

	// add a dynamic attribute on instance of localtimer
	localtimer->AddDynamicAttribute(ATTRIBUTE_TYPE::FLOAT, "floatValue", 12.0f);

	// only if export is supported
#ifdef KIGS_TOOLS 
	// export Sample1 and its sons in Sample1.xml file
	CoreModifiable::Export("Sample1.xml", simpleclass.get(), true);
#endif // KIGS_TOOLS

	// import instances from file "Sample1.xml"
	CMSP imported=CoreModifiable::Import("Sample1.xml");

	// if file was found, add a ref so that imported will not be destroyed when exiting ProtectedInit
	if (imported)
	{
		imported->GetRef();
	}
	else
	{
		// if the file was not found, get a ref on simpleclass to keep it alive after exiting this method
		simpleclass->GetRef();
	}
	
}

void	Sample1::ProtectedUpdate()
{
	// search first instance of SimpleSampleClass
	CoreModifiable* simpleclass = GetFirstInstance("SimpleSampleClass");


	// call SimpleSampleClass AddValue method directly on CoreModifiable
	float result = simpleclass->SimpleCall<float>("AddValue", 10, 12.0f);
	printf("result of calling AddValue = %f\n", result);
	
	// search son with given name
	Timer* localtimer= simpleclass->GetFirstSonByName("Timer", "localtimer")->as<Timer>();
	double currentTime=localtimer->GetTime();
	double currentTimeWithGetVal=localtimer->getValue<double>("Time");

	// print current timer value in console
	printf("current time is %lf with GetTime\n", currentTime);
	printf("current time is %lf with getValue<double>(\"Time\");\n", currentTimeWithGetVal);

	// if timer was init more than 0.1 seconde before, then exit app
	if (currentTime > 0.1)
	{
		myNeedExit = true;
	}

	// retreive "Sample1Value" value on this
	int _value;
	simpleclass->getValue("Sample1Value", _value);

	printf("value = %d\n", _value);

	_value = 2 * _value*_value - 70;
	// change "Sample1Value" value with _value 
	simpleclass->setValue("Sample1Value",  _value);

	// retrieve dynamic attribute value on localtimer
	float timervalue=localtimer->getValue<float>("floatValue");
	printf("timer get float value = %f\n", timervalue);

	// set dynamic float attribute with string 
	localtimer->setValue("floatValue","24");
}

void	Sample1::ProtectedClose()
{
	// destroy simpleclass (and recursively  all its sons) 
	CoreModifiable* simpleclass = GetFirstInstance("SimpleSampleClass");
	// here we need to destroy simpleclass only because we get a ref on it in ProtectedInit
	// to keep it alive even if it was not add to another instance (with addItem)
	simpleclass->Destroy();
}
