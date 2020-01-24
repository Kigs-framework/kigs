#pragma once

#include <DataDrivenBaseApplication.h>

class Sample6 : public DataDrivenBaseApplication
{
public:
	DECLARE_CLASS_INFO(Sample6, DataDrivenBaseApplication, Core);
	DECLARE_CONSTRUCTOR(Sample6);

protected:

	void	MethodWithParams(float p1, float p2);
	WRAP_METHODS(MethodWithParams);

	DECLARE_METHOD(CatchNotifMethod);

	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;

	
	void	ProtectedInitSequence(const kstl::string& sequence) override;
	void	ProtectedCloseSequence(const kstl::string& sequence) override;
};
