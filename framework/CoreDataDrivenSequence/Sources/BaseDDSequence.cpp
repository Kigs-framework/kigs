#include "BaseDDSequence.h"


IMPLEMENT_CLASS_INFO(BaseDDSequence)

BaseDDSequence::BaseDDSequence(const std::string& name, CLASS_NAME_TREE_ARG) 
: DataDrivenSequence(name, PASS_CLASS_NAME_TREE_ARG)
, mTransitionData(*this, true, "TransitionData","")
{

}

BaseDDSequence::~BaseDDSequence()
{

}