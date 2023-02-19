#pragma once

#include "DataDrivenBaseApplication.h"

namespace Kigs
{
	using namespace Kigs::DDriven;

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

		void	ProtectedInitSequence(const std::string& sequence) override;
		void	ProtectedCloseSequence(const std::string& sequence) override;

		maCoreItem mFunction = BASE_ATTRIBUTE(Function, "eval(12)");

		float	mTestFloat = 50.0f;
		v2f		mEvalResult = { 0.0f,0.0f };

		WRAP_ATTRIBUTES(mTestFloat, mEvalResult);

		float	randomNumber(float min, float max);
	};
}
