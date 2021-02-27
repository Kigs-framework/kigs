#include "..\Headers\ActiveZone.h"

//-------------------------------------------------------------------------
//Constructor
IMPLEMENT_CLASS_INFO(ActiveZone)

ActiveZone::ActiveZone(const kstl::string& name, CLASS_NAME_TREE_ARG)
: CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
}