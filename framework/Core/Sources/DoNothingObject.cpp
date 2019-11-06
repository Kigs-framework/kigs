#include "PrecompiledHeaders.h"

#include "DoNothingObject.h"

//! a do nothing object is just an instance of CoreModifiable with empty methods
IMPLEMENT_CLASS_INFO(DoNothingObject)

//! constructor
DoNothingObject::DoNothingObject(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{
	KIGS_WARNING((((kstl::string)"Do Nothing Object created : ") + name),2); 
}     
