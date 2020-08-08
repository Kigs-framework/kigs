#include <KigsDocs.h>

IMPLEMENT_CLASS_INFO(KigsDocs);

IMPLEMENT_CONSTRUCTOR(KigsDocs)
{

}

void	KigsDocs::ProtectedInit()
{
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);
}

void	KigsDocs::ProtectedUpdate()
{
}

void	KigsDocs::ProtectedClose()
{
}
