#pragma once

#include <DataDrivenBaseApplication.h>

class Sample5 : public DataDrivenBaseApplication
{
public:
	DECLARE_CLASS_INFO(Sample5, DataDrivenBaseApplication, Core);
	DECLARE_CONSTRUCTOR(Sample5);

protected:

	WRAP_METHODS(randomNumber);

	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;

	void	ProtectedInitSequence(const kstl::string& sequence) override;
	void	ProtectedCloseSequence(const kstl::string& sequence) override;

	maCoreItem myFunction = BASE_ATTRIBUTE(function,"eval(12)");
	maFloat	myTestFloat = BASE_ATTRIBUTE(TestFloat, 50.0f);

	maVect2DF myEvalResult = BASE_ATTRIBUTE(EvalResult, 0,0);

	float	randomNumber(float min, float max);
};
