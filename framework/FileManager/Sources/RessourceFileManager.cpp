#include "PrecompiledHeaders.h"
#include "RessourceFileManager.h"
#include <stdio.h>
#include "Core.h"

IMPLEMENT_CLASS_INFO(RessourceFileManager)

//! constructor
RessourceFileManager::RessourceFileManager(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{
   
}     

//! destructor
RessourceFileManager::~RessourceFileManager()
{

}    

CoreModifiable*	RessourceFileManager::GetRessource(const kstl::string &ClassName, const kstl::string& fileName)
{
	// already loaded ?
	if(myRessourceMap.find(fileName) != myRessourceMap.end())
	{
		CoreModifiable *pRessource = myRessourceMap[fileName];
		pRessource->GetRef();
		return pRessource;
	}

	// create a new one
	CoreModifiable* pRessource = (CoreModifiable*)(KigsCore::GetInstanceOf(ClassName+"["+fileName+"]",ClassName));
	pRessource->setValue(LABEL_TO_ID(FileName),fileName);
	pRessource->Init();
	addItem(pRessource);

	return pRessource;
}

//! add item. 
bool	RessourceFileManager::addItem(CoreModifiable *item, ItemPosition pos DECLARE_LINK_NAME)
{
	kstl::string filename;
	if(item->getValue(LABEL_TO_ID(FileName),filename))
	{
		if(filename!="")
		{
			if(myRessourceMap.find(filename)!=myRessourceMap.end())
			{
				removeItem(item PASS_LINK_NAME( linkName ));
			}
			myRessourceMap[filename]=item;
		}
	}
	return CoreModifiable::addItem(item, pos PASS_LINK_NAME(linkName));
}

//! remove item. 
bool	RessourceFileManager::removeItem(CoreModifiable* item DECLARE_LINK_NAME)
{
	kstl::string filename;
	if(item->getValue(LABEL_TO_ID(FileName),filename))
	{
		if(filename!="")
		{
			if(myRessourceMap.find(filename)!=myRessourceMap.end())
			{
				kstl::map<kstl::string, CoreModifiable*>::iterator	it=myRessourceMap.find(filename);
				myRessourceMap.erase(it);				
			}
		}
	}
	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));
}


void	RessourceFileManager::UnloadRessource(CoreModifiable* pRessource)
{
	// search this Ressource
	removeItem(pRessource);
}

void	RessourceFileManager::UnloadRessource(kstl::string ressourcename)
{
	if(myRessourceMap.find(ressourcename)!=myRessourceMap.end())
	{
		kstl::map<kstl::string, CoreModifiable*>::iterator	it=myRessourceMap.find(ressourcename);
		removeItem((*it).second);
	}
}
