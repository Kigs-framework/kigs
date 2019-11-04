#ifndef _RESSOURCEFILEMANAGER_H_
#define _RESSOURCEFILEMANAGER_H_

#include "CoreModifiable.h"

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

    DECLARE_CLASS_INFO(RessourceFileManager,CoreModifiable,Renderer)

	//! constructor
    RessourceFileManager(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/*! return an instance of the type ClassName initialised from the given filename (xml)
		if the file was already read, and an instance already exist, return a reference on
		the instance
	*/
	CoreModifiable*	GetRessource(const kstl::string &ClassName, const kstl::string& fileName);

	/*! release one reference
	*/
	void		UnloadRessource(CoreModifiable*);
	void		UnloadRessource(kstl::string);

	//! add item. 
	bool	addItem(CoreModifiable *item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override;
	//! remove item. 
	bool	removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME) override;


protected:
	//! destructor
    virtual ~RessourceFileManager();

	//! For each extension, a path list.
	kstl::map<kstl::string, CoreModifiable*>		myRessourceMap;
};

#endif //_RessourceFILEMANAGER_H_
