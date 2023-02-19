#pragma once

#include "CoreBaseApplication.h"
#include "CoreModifiableAttribute.h"

namespace Kigs
{
	using namespace Kigs::Core;
	class Sample4 : public CoreBaseApplication
	{
	public:
		DECLARE_CLASS_INFO(Sample4, CoreBaseApplication, Core);
		DECLARE_CONSTRUCTOR(Sample4);

	protected:
		void	ProtectedInit() override;
		void	ProtectedUpdate() override;
		void	ProtectedClose() override;

		// add a string attribute
		std::string		mStringParam = "Sample4 string attribute";

		WRAP_ATTRIBUTES(mStringParam);

	};
}