#pragma once

#include <DataDrivenBaseApplication.h>

class Sample7 : public DataDrivenBaseApplication
{
public:
	DECLARE_CLASS_INFO(Sample7, DataDrivenBaseApplication, Core);
	DECLARE_CONSTRUCTOR(Sample7);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;

	
	void	ProtectedInitSequence(const kstl::string& sequence) override;
	void	ProtectedCloseSequence(const kstl::string& sequence) override;

	// method called from Lua script
	void	HelloFromLua();
	WRAP_METHODS(HelloFromLua)

};
