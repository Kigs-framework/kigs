#pragma once

#include <CoreBaseApplication.h>

namespace Kigs
{
	using namespace Kigs::Core;
	class Sample2 : public CoreBaseApplication
	{
	public:
		DECLARE_CLASS_INFO(Sample2, CoreBaseApplication, Core);
		DECLARE_CONSTRUCTOR(Sample2);

	protected:
		void	ProtectedInit() override;
		void	ProtectedUpdate() override;
		void	ProtectedClose() override;
	};
}