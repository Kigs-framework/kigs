#pragma once


namespace Kigs
{
	namespace File
	{
		using namespace Kigs::Core;
		// no specific Windows Filemanager

		void 	PlatformFileManagerModuleInit(KigsCore* /*core*/, const std::vector<CoreModifiableAttribute*>* /*params*/) { ; }
		void 	PlatformFileManagerModuleClose(KigsCore* /*core*/) { ; }
		void 	PlatformFileManagerModuleUpdate(const Timer* /*timer*/) { ; }
		void	PlatformFileManagerGetModuleInstance(ModuleBase** /*instance*/) { ; }
	}
}