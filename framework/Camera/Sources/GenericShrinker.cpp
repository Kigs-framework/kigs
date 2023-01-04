#include "PrecompiledHeaders.h"
#include "GenericShrinker.h"
#include "Core.h"

using namespace Kigs::Camera;

IMPLEMENT_CLASS_INFO(GenericShrinker)

//! constructor
GenericShrinker::GenericShrinker(const std::string& name,CLASS_NAME_TREE_ARG) : FrameBufferStream(name,PASS_CLASS_NAME_TREE_ARG),
mResizeX(*this,true,"ResizeX",64),
mResizeY(*this,true,"ResizeY",64)
{
   
}     

//! destructor
GenericShrinker::~GenericShrinker()
{
	
}    
