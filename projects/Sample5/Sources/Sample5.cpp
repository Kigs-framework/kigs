#include <Sample5.h>
#include <FilePathManager.h>
#include <NotificationCenter.h>
#include "CoreItem.h"
#include "CoreVector.h"
#include "JSonFileParser.h"
#include <iostream>

// Kigs framework Sample5 project
// CoreItem features :
// - import / export JSON objects 
// - create/evaluate mathematical expression
// - animate values 


IMPLEMENT_CLASS_INFO(Sample5);

IMPLEMENT_CONSTRUCTOR(Sample5)
{

}

float	Sample5::randomNumber(float min, float max)
{
	float rnd = ((float)rand()) / (float)RAND_MAX;

	rnd *= max - min;

	return min + rnd;
}

void	Sample5::ProtectedInit()
{
	// Base modules have been created at this point

	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	SP<FilePathManager> pathManager = KigsCore::GetSingleton("FilePathManager");
	pathManager->AddToPath(".", "xml");

	// CoreItem can be used to manage JSon slyle objects
	// main CoreItem types are CoreValue, CoreMap and CoreVector
	// A CoreItem hierarchy can be created from json file (or json string)

	JSonFileParser L_JsonParser;
	CoreItemSP item = L_JsonParser.Get_JsonDictionaryFromString(R"====(
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

	// create a CoreValue<int> with value 52
	CoreItemSP toInsert = CoreItemSP::getCoreValue(52);

	// add it to item with key val2
	item->set("val2", toInsert);

	// print values accessing map at val1 and val2
	std::cout << "val 1 : " << (int)item["val1"] << std::endl;
	std::cout << "val 2 : " << (int)item["val2"] << std::endl;

	// retreive obj1
	CoreItemSP obj1 = item["obj1"];

	// obj3 is an array, add a CoreValue<std::string> at the end of the array (for an array, set with no key <=> push_back) 
	obj1["obj3"]->set("", CoreItemSP::getCoreValue("lastElem"));

	// print all obj 3 values
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

	// push back another vector on obj3, created from a Point3D
	Point3D	toPush(5.0f, 4.0f, 3.0f);
	obj1["obj3"]->set("", CoreItemSP(toPush));

	// Export JSON item as attribute of an empty object
	CMSP donothing = KigsCore::GetInstanceOf("useless", "DoNothing");
	donothing->AddDynamicAttribute(COREITEM, "item");
	donothing->setValue("item", item.get());
#ifdef KIGS_TOOLS
	Export("testCoreItemExport.xml", donothing.get());
#endif

	// CoreItem can also be used to evaluate mathematical expressions
	// with the eval keywork
	// (basic string operation are also available with evalStr keyword)
	// create a mathematic expression : 
	CoreItemSP	tsteval("eval(12.0*sin(4.0))");

	// evaluate the expression and print it
	std::cout << "Expression : 12.0*sin(4.0) = " << (float)tsteval << std::endl;

	// for maCoreItem, or using absolute path, you can use CoreModifiable attributes in more complex expressions
	myFunction.setValue("eval(12.0*sin(#TestFloat#+#/Timer:ApplicationTimer->Time#))");
	std::cout << "Expression : eval(12.0*sin(#TestFloat#+#/Timer:ApplicationTimer->Time#)) = " << (float)(CoreItem&)myFunction << std::endl;
	// expression is evaluated again at each cast to float
	std::cout << "Expression : eval(12.0*sin(#TestFloat#+#/Timer:ApplicationTimer->Time#)) = " << (float)(CoreItem&)myFunction << std::endl;

	// it's also possible to call CoreModifiable methods, set CoreModifiable attributes and make some tests
	tsteval = std::string("eval(if(([/Sample5->randomNumber(0.0,2.0)]>1.0),#/Sample5->EvalResult.x#=(#/Sample5->EvalResult.x#+1);1,#/Sample5->EvalResult.y#=(#/Sample5->EvalResult.y#+1);2))");

	std::cout << "tsteval result = " << (float)tsteval << std::endl;
	std::cout << "EvalResult = [ " << myEvalResult[0] << "," << myEvalResult[1] << " ]" << std::endl;
	std::cout << "tsteval result = " << (float)tsteval << std::endl;
	std::cout << "EvalResult = [ " << myEvalResult[0] << "," << myEvalResult[1] << " ]" << std::endl;
	std::cout << "tsteval result = " << (float)tsteval << std::endl;
	std::cout << "EvalResult = [ " << myEvalResult[0] << "," << myEvalResult[1] << " ]" << std::endl;
	std::cout << "tsteval result = " << (float)tsteval << std::endl;
	std::cout << "EvalResult = [ " << myEvalResult[0] << "," << myEvalResult[1] << " ]" << std::endl;
	std::cout << "tsteval result = " << (float)tsteval << std::endl;
	std::cout << "EvalResult = [ " << myEvalResult[0] << "," << myEvalResult[1] << " ]" << std::endl;

	// Load AppInit, GlobalConfig then launch first sequence
	DataDrivenBaseApplication::ProtectedInit();
}

void	Sample5::ProtectedUpdate()
{
	DataDrivenBaseApplication::ProtectedUpdate();

	if (GetApplicationTimer()->GetTime() > 5.0f)
	{
		myNeedExit=true;
	}
}

void	Sample5::ProtectedClose()
{
	DataDrivenBaseApplication::ProtectedClose();
}

void	Sample5::ProtectedInitSequence(const kstl::string& sequence)
{
	if (sequence == "sequencemain")
	{
		
	}
}
void	Sample5::ProtectedCloseSequence(const kstl::string& sequence)
{
	if (sequence == "sequencemain")
	{
		
	}
}

