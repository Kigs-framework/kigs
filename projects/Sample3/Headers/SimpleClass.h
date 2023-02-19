#pragma once
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "maReference.h"

namespace Kigs
{
	using namespace Kigs::Core;
	class SimpleClass : public CoreModifiable
	{
	public:
		DECLARE_CLASS_INFO(SimpleClass, CoreModifiable, Application);
		DECLARE_CONSTRUCTOR(SimpleClass);

		void	DoSomethingFun();

	protected:

		virtual void NotifyUpdate(const u32 labelid) override;

		int								mIntValue = 5;
		std::string						mStringValue = "StringValue";
		v4f								mVector = { 0.0, 0.0, 1.0, 2.0 };
		std::weak_ptr<CoreModifiable>	mReference;

		WRAP_ATTRIBUTES(mIntValue, mStringValue, mVector, mReference);
	};

}