#pragma once

#include "CoreBaseApplication.h"
// for MeshConvertParams
#include "ModernMesh.h"
#include "Base3DImporter.h"


namespace Kigs
{
	namespace Scene
	{
		class	ModuleSceneGraph;
	}
	namespace Draw
	{
		class	ModuleRenderer;
	}
	namespace Gui
	{
		class	ModuleGUI;
	}
	using namespace Kigs::Core;

	class MeshConvert : public CoreBaseApplication
	{
	public:

		DECLARE_CLASS_INFO(MeshConvert, CoreBaseApplication, Core)

			MeshConvert(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
		virtual		~MeshConvert();

	protected:

		void	RetreiveShortNameAndExt(const std::string& filename, std::string& shortname, std::string& fileext);

		void	InitExternClasses();


		//Virtual methods
		virtual void									ProtectedInit();
		virtual void									ProtectedUpdate();
		virtual void									ProtectedClose();
		virtual bool									ProtectedExternAskExit();
		float											myStartTime;

		SP<Scene::ModuleSceneGraph>		theSceneGraph;
		SP<Draw::ModuleRenderer>		theRenderer;
		SP<Gui::ModuleGUI>				theGUI;

		Utils::MeshConvertParams		myParams;

		std::vector<CMSP>	myObjectsToDestroy;
	};


}