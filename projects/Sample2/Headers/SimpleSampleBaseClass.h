#pragma once
#include "CoreModifiable.h"
#include <iostream>


namespace Kigs
{
	using namespace Kigs::Core;
	class SimpleSampleBaseClass : public CoreModifiable
	{
	public:
		DECLARE_ABSTRACT_CLASS_INFO(SimpleSampleBaseClass, CoreModifiable, Application);
		DECLARE_INLINE_CONSTRUCTOR(SimpleSampleBaseClass) { std::cout << "SimpleSampleBaseClass constructor" << std::endl; }

	protected:

		void InitModifiable() override;
		virtual void Update(const Time::Timer& timer, void* addParam)  override;

		// method that add 1 to the given parameter
		DECLARE_METHOD(incrementParam);
		// list all CoreModifiable methods
		COREMODIFIABLE_METHODS(incrementParam);
	};
}