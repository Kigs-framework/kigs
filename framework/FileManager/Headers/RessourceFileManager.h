#pragma once

#include "CoreModifiable.h"

namespace Kigs
{
	namespace File
	{
		using namespace Kigs::Core;
		// ****************************************
		// * RessourceFileManager class
		// * --------------------------------------
		/*!  \class RessourceFileManager
			 this class is used to load / unload Ressources

			 RessourceFileManager*	fileManager=(RessourceFileManager*)KigsCore::GetSingleton("RessourceFileManager");

			 to retreive the unique RessourceFileManager to use...

			a Ressource is any class derived from CoreModifiable which has a var maString with name "FileName"
				exemple : Texture
						  Mesh...

			\ingroup FileManager

		*/
		// ****************************************


		class RessourceFileManager : public CoreModifiable
		{
		public:

			DECLARE_CLASS_INFO(RessourceFileManager, CoreModifiable, Renderer)

				//! constructor
				RessourceFileManager(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			/*! return an instance of the type ClassName initialised from the given filename (xml)
				if the file was already read, and an instance already exist, return a reference on
				the instance
			*/
			CMSP GetRessource(const std::string& ClassName, const std::string& fileName);

			/*! release one reference
			*/
			void		UnloadRessource(const CMSP& res);
			void		UnloadRessource(std::string);

			//! add item. 
			bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
			//! remove item. 
			bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

			//! destructor
			virtual ~RessourceFileManager();

		protected:


			//! For each extension, a path list.
			std::map<std::string, CMSP>		mRessourceMap;
		};

	}
}
