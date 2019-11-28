#pragma once

#include <CoreBaseApplication.h>
#include "maNumeric.h"

class Sample1 : public CoreBaseApplication
{
public:
	DECLARE_CLASS_INFO(Sample1, CoreBaseApplication, Core);
	DECLARE_CONSTRUCTOR(Sample1);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;

	
};
