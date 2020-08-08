#pragma once

#include <CoreBaseApplication.h>

class KigsDocs : public CoreBaseApplication
{
public:
	DECLARE_CLASS_INFO(KigsDocs, CoreBaseApplication, Core);
	DECLARE_CONSTRUCTOR(KigsDocs);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;
};
