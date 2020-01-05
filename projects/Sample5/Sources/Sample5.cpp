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

	CoreItemSP toInsert = CoreItemSP((CoreItem*)new CoreValue(52));

	((CoreMap<std::string>*)item.get())->set("val2", toInsert);

	CoreItemSP obj1 = item["obj1"];
	
	std::cout << "obj 3 : " << (float)(CoreItem&)obj1["obj3"][0] << "," << (float)(CoreItem&)obj1["obj3"][1] << "," << (std::string)(CoreItem&)obj1["obj3"][2] << "," << (std::string)(CoreItem&)obj1["obj3"][3] << std::endl;
	std::cout << "val 1 : " << (int)(CoreItem&)item["val1"] << std::endl;

	std::cout << "val 2 : " << (int)(CoreItem&)item["val2"] << std::endl;

	CoreModifiable* donothing = KigsCore::GetInstanceOf("useless", "DoNothing");

	donothing->AddDynamicAttribute(COREITEM,"item");

	donothing->setValue("item", item.get());

#ifdef KIGS_TOOLS
	Export("testCoreItemExport.xml", donothing);
#endif

	donothing->Destroy();
}

void	Sample5::ProtectedUpdate()
{
	// ask exit 
	myNeedExit = true;
}

void	Sample5::ProtectedClose()
{
}
