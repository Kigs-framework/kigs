#pragma once

#include <DataDrivenBaseApplication.h>

class Sample8 : public DataDrivenBaseApplication
{
public:
	DECLARE_CLASS_INFO(Sample8, DataDrivenBaseApplication, Core);
	DECLARE_CONSTRUCTOR(Sample8);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;

	
	void	ProtectedInitSequence(const std::string& sequence) override;
	void	ProtectedCloseSequence(const std::string& sequence) override;
};
