#pragma once

#include <CoreBaseApplication.h>

class Sample5 : public CoreBaseApplication
{
public:
	DECLARE_CLASS_INFO(Sample5, CoreBaseApplication, Core);
	DECLARE_CONSTRUCTOR(Sample5);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;
};
