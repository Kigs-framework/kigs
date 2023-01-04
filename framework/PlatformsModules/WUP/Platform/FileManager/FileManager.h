#pragma once

namespace Kigs
{
	namespace Core
	{
		class KigsCore;
	}

	namespace File
	{
		// no specific Windows Filemanager

		void 	PlatformFileManagerModuleInit(Core::KigsCore* /*core*/, const std::vector<Core::CoreModifiableAttribute*>* /*params*/) { ; }
		void 	PlatformFileManagerModuleClose(Core::KigsCore* /*core*/) { ; }
		void 	PlatformFileManagerModuleUpdate(const Time::Timer* /*timer*/) { ; }
		void	PlatformFileManagerGetModuleInstance(Core::ModuleBase** /*instance*/) { ; }
	}
}
