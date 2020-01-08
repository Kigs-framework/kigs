#pragma once

#include <CoreBaseApplication.h>

class Sample3 : public CoreBaseApplication
{
public:
	DECLARE_CLASS_INFO(Sample3, CoreBaseApplication, Core);
	DECLARE_CONSTRUCTOR(Sample3);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;
};
