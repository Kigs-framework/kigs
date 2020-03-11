#pragma once

#include <DataDrivenBaseApplication.h>

class GenericAnimationModule;

class Sample8 : public DataDrivenBaseApplication
{
public:
	DECLARE_CLASS_INFO(Sample8, DataDrivenBaseApplication, Core);
	DECLARE_CONSTRUCTOR(Sample8);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;

	
	void	ProtectedInitSequence(const kstl::string& sequence) override;
	void	ProtectedCloseSequence(const kstl::string& sequence) override;

	GenericAnimationModule* theGenericAnimationModule=nullptr;
};
