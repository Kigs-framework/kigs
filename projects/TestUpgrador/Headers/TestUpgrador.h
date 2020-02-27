#pragma once

#include <CoreBaseApplication.h>

class TestUpgrador : public CoreBaseApplication
{
public:
	DECLARE_CLASS_INFO(TestUpgrador, CoreBaseApplication, Core);
	DECLARE_CONSTRUCTOR(TestUpgrador);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;
};
