#pragma once

#include "CoreBaseApplication.h"

namespace Kigs
{
	using namespace Core;

	class Sample6 : public CoreBaseApplication
	{
	public:
		DECLARE_CLASS_INFO(Sample6, CoreBaseApplication, Core);
		DECLARE_CONSTRUCTOR(Sample6);

	protected:

		// Wrapped methods
		void	MethodWithParams(float p1, float p2);
		void    OnSimpleClassPreInit();

		WRAP_METHODS(MethodWithParams, OnSimpleClassPreInit);

		// Fixed prototype 
		DECLARE_METHOD(CatchNotifMethod);
		COREMODIFIABLE_METHODS(CatchNotifMethod);

		void	ProtectedInit() override;
		void	ProtectedUpdate() override;
		void	ProtectedClose() override;

	};
}