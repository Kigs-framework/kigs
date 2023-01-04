#include "..\Headers\ActiveZone.h"

using namespace Kigs::Gps;
//-------------------------------------------------------------------------
//Constructor
IMPLEMENT_CLASS_INFO(ActiveZone)

ActiveZone::ActiveZone(const std::string& name, CLASS_NAME_TREE_ARG)
: CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
}