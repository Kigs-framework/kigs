#include "PrecompiledHeaders.h"
#include "Semaphore.h"

using namespace Kigs::Thread;

IMPLEMENT_CLASS_INFO(Semaphore)

Semaphore::Semaphore(const std::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{
}     

Semaphore::~Semaphore()
{
	
}    
