#include "PrecompiledHeaders.h"
#include "MiniInstanceFactory.h"

using namespace Kigs::Core;

IMPLEMENT_CLASS_INFO(MiniInstanceFactory)

MiniInstanceFactory::MiniInstanceFactory(const std::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{
	mFactoryMap.clear();
}


MiniInstanceFactory::~MiniInstanceFactory()
{
	// destroy mFactoryMap
	for(auto itmap = mFactoryMap.begin();itmap != mFactoryMap.end();itmap++)
	{
		delete (*itmap).second;
	}

	mFactoryMap.clear();
}