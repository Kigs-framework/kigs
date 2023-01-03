#pragma once

#include "CoreBaseApplication.h"

namespace Kigs
{
	using namespace Kigs::Core;

	class KigsPackager : public CoreBaseApplication
	{
	public:

		DECLARE_CLASS_INFO(KigsPackager, CoreBaseApplication, Core)

			KigsPackager(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
		virtual		~KigsPackager();

	protected:

		void	RetreiveShortNameAndExt(const std::string& filename, std::string& shortname, std::string& fileext);

		void	InitExternClasses();


		//Virtual methods
		virtual void									ProtectedInit();
		virtual void									ProtectedUpdate();
		virtual void									ProtectedClose();
		virtual bool									ProtectedExternAskExit();
		float											myStartTime;

	};


}