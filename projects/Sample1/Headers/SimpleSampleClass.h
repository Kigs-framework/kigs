#pragma once
#include "CoreModifiable.h"
#include "maNumeric.h"

namespace Kigs
{
	using namespace Kigs::Core;

	class SimpleSampleClass : public CoreModifiable
	{
	public:
		DECLARE_CLASS_INFO(SimpleSampleClass, CoreModifiable, Application);
		DECLARE_INLINE_CONSTRUCTOR(SimpleSampleClass) {}

	protected:
		void InitModifiable() override;


		int mSample1Value = 6;

		// add an int attribute named "Sample1Value" with value 6
		//maInt	mTestValue = BASE_ATTRIBUTE(Sample1Value, 6);

		WRAP_ATTRIBUTES(mSample1Value);

		// add given params to mTestValue
		DECLARE_METHOD(AddValue);

		// list CoreModifiable methods
		COREMODIFIABLE_METHODS(AddValue);
	};
}