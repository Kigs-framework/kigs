#include "PrecompiledHeaders.h"
#include "GenericShrinker.h"
#include "Core.h"


IMPLEMENT_CLASS_INFO(GenericShrinker)

//! constructor
GenericShrinker::GenericShrinker(const kstl::string& name,CLASS_NAME_TREE_ARG) : FrameBufferStream(name,PASS_CLASS_NAME_TREE_ARG),
mResizeX(*this,true,LABEL_AND_ID(ResizeX),64),
mResizeY(*this,true,LABEL_AND_ID(ResizeY),64)
{
   
}     

//! destructor
GenericShrinker::~GenericShrinker()
{
	
}    
