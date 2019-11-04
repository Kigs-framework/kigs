#include "PrecompiledHeaders.h"
#include "MiniInstanceFactory.h"

IMPLEMENT_CLASS_INFO(MiniInstanceFactory)

MiniInstanceFactory::MiniInstanceFactory(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{
	myFactoryMap.clear();
}


MiniInstanceFactory::~MiniInstanceFactory()
{
	// destroy myFactoryMap
	kstl::map<kstl::string,	FactoryCreateStruct*>::iterator	itmap;
	for(itmap = myFactoryMap.begin();itmap != myFactoryMap.end();itmap++)
	{
		delete (*itmap).second;
	}

	myFactoryMap.clear();
}