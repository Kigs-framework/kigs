#include "BaseDDSequence.h"


IMPLEMENT_CLASS_INFO(BaseDDSequence)

BaseDDSequence::BaseDDSequence(const kstl::string& name, CLASS_NAME_TREE_ARG) 
: DataDrivenSequence(name, PASS_CLASS_NAME_TREE_ARG)
, mTransitionData(*this, true, LABEL_AND_ID(TransitionData),"")
{

}

BaseDDSequence::~BaseDDSequence()
{

}