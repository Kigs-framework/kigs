#include "GenericDDSequence.h"
#include "Timer.h"
#include "NotificationCenter.h"

IMPLEMENT_CLASS_INFO(GenericDDSequence)

GenericDDSequence::GenericDDSequence(const kstl::string& name, CLASS_NAME_TREE_ARG)
: BaseDDSequence(name, PASS_CLASS_NAME_TREE_ARG)
,myNextSequence(*this, true, LABEL_AND_ID(NextSequence))
,myDuration(*this, true, LABEL_AND_ID(Duration), 0.0f)
, mStartTime(0)
{
}

GenericDDSequence::~GenericDDSequence()
{
}
