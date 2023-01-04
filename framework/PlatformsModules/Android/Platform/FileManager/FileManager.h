#pragma once

namespace Kigs
{
	namespace Time
	{
		class Timer;
	}
	namespace File
	{
		void 	PlatformFileManagerModuleInit(KigsCore* /*core*/, const std::vector<CoreModifiableAttribute*>* /*params*/) { ; }
		void 	PlatformFileManagerModuleClose(KigsCore* /*core*/) { ; }
		void 	PlatformFileManagerModuleUpdate(const Time::Timer* /*timer*/) { ; }
		void	PlatformFileManagerGetModuleInstance(ModuleBase** /*instance*/) { ; }
	}
}

