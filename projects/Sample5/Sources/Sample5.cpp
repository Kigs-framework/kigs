#include <Sample5.h>
#include "CoreItem.h"
#include "CoreVector.h"
#include "JSonFileParser.h"
#include <iostream>


IMPLEMENT_CLASS_INFO(Sample5);

IMPLEMENT_CONSTRUCTOR(Sample5)
{

}

void	Sample5::ProtectedInit()
{


	//_CrtSetBreakAlloc(1887);
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	// main CoreItem types are CoreValue, CoreMap and CoreVector
	// A CoreItem hierarchy can be created from json file (or json string)

	JSonFileParser L_JsonParser;
	CoreItemSP item= L_JsonParser.Get_JsonDictionaryFromString(R"====(
	{
		"obj1" : { "obj3": [0.4, 0.9, 0.0, "str"] },
		"val1" : 15
	}
)====");

	// check if val2 exist
	if (item["val2"].isNil())
	{
		std::cout << "val 2 not found " << std::endl;
	}
	else
	{
		std::cout << "val 2 : " << (int)item["val2"] << std::endl;
	}

	CoreItemSP toInsert = CoreItemSP::getCoreValue(52);

	item->set("val2",toInsert);

	CoreItemSP obj1 = item["obj1"];
	
	std::cout << "val 1 : " << (int)item["val1"] << std::endl;
	std::cout << "val 2 : " << (int)item["val2"] << std::endl;

	obj1["obj3"]->set("",CoreItemSP::getCoreValue("lastElem"));

	bool first = true;
	std::cout << "obj 3 : [";
	for (auto tst : obj1["obj3"])
	{
		if (!first)
		{
			std::cout << " , ";
		}
		first = false;
		std::cout << (std::string)tst;
	}
	std::cout << "]" << std::endl;

	CMSP donothing = KigsCore::GetInstanceOf("useless", "DoNothing");

	donothing->AddDynamicAttribute(COREITEM,"item");

	donothing->setValue("item", item.get());

#ifdef KIGS_TOOLS
 	Export("testCoreItemExport.xml", donothing.get());
#endif

}

void	Sample5::ProtectedUpdate()
{
	// ask exit 
	myNeedExit = true;
}

void	Sample5::ProtectedClose()
{
	
 }
