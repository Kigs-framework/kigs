#pragma once

#include <DataDrivenBaseApplication.h>

class Sample5bis : public DataDrivenBaseApplication
{
public:
	DECLARE_CLASS_INFO(Sample5bis, DataDrivenBaseApplication, Core);
	DECLARE_CONSTRUCTOR(Sample5bis);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;

	
	void	ProtectedInitSequence(const kstl::string& sequence) override;
	void	ProtectedCloseSequence(const kstl::string& sequence) override;
};
