#pragma once
#include "CoreModifiable.h"
#include <iostream>
#include "Core.h"
#include "AttributePacking.h"

namespace Kigs
{
	using namespace Kigs::Core;

	class SimpleMaterialClass : public CoreModifiable
	{
	public:
		DECLARE_CLASS_INFO(SimpleMaterialClass, CoreModifiable, Application);
		DECLARE_INLINE_CONSTRUCTOR(SimpleMaterialClass) { std::cout << "SimpleMaterialClass constructor" << std::endl; }

	protected:

		// RGB color 
		v3f			mColor = { 1.0, 0.0, 0.0 };
		// shininess 
		float		mShininess = 0.5f;

		WRAP_ATTRIBUTES(mColor, mShininess);

	};
}