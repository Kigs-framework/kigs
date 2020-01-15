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

CMSP RessourceFileManager::GetRessource(const kstl::string &ClassName, const kstl::string& fileName)
{
	// already loaded ?
	if(myRessourceMap.find(fileName) != myRessourceMap.end())
	{
		return myRessourceMap[fileName];
	}

	// create a new one
	CMSP pRessource = KigsCore::GetInstanceOf(ClassName+"["+fileName+"]",ClassName);
	pRessource->setValue(LABEL_TO_ID(FileName),fileName);
	pRessource->Init();
	addItem(pRessource);

	return pRessource;
}

//! add item. 
bool	RessourceFileManager::addItem(CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
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
bool	RessourceFileManager::removeItem(CMSP& item DECLARE_LINK_NAME)
{
	kstl::string filename;
	if(item->getValue(LABEL_TO_ID(FileName),filename))
	{
		if(filename!="")
		{
			if(myRessourceMap.find(filename)!=myRessourceMap.end())
			{
				kstl::map<kstl::string, CMSP>::iterator	it=myRessourceMap.find(filename);
				myRessourceMap.erase(it);				
			}
		}
	}
	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));
}


void	RessourceFileManager::UnloadRessource(CMSP& pRessource)
{
	// search this Ressource
	removeItem(pRessource);
}

void	RessourceFileManager::UnloadRessource(kstl::string ressourcename)
{
	if(myRessourceMap.find(ressourcename)!=myRessourceMap.end())
	{
		kstl::map<kstl::string, CMSP>::iterator	it=myRessourceMap.find(ressourcename);
		removeItem((*it).second);
	}
}
