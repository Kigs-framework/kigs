#include "PrecompiledHeaders.h"
#include "RessourceFileManager.h"
#include <stdio.h>
#include "Core.h"

IMPLEMENT_CLASS_INFO(RessourceFileManager)

//! constructor
RessourceFileManager::RessourceFileManager(const std::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{
   
}     

//! destructor
RessourceFileManager::~RessourceFileManager()
{

}    

CMSP RessourceFileManager::GetRessource(const std::string &ClassName, const std::string& fileName)
{
	// already loaded ?
	if(mRessourceMap.find(fileName) != mRessourceMap.end())
	{
		return mRessourceMap[fileName];
	}

	// create a new one
	CMSP pRessource = KigsCore::GetInstanceOf(ClassName+"["+fileName+"]",ClassName);
	pRessource->setValue("FileName",fileName);
	pRessource->Init();
	addItem(pRessource);

	return pRessource;
}

//! add item. 
bool	RessourceFileManager::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	std::string filename;
	if(item->getValue("FileName",filename))
	{
		if(filename!="")
		{
			if(mRessourceMap.find(filename)!=mRessourceMap.end())
			{
				removeItem(item PASS_LINK_NAME( linkName ));
			}
			mRessourceMap[filename]=item;
		}
	}
	return CoreModifiable::addItem(item, pos PASS_LINK_NAME(linkName));
}

//! remove item. 
bool	RessourceFileManager::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	std::string filename;
	if(item->getValue("FileName",filename))
	{
		if(filename!="")
		{
			if(mRessourceMap.find(filename)!=mRessourceMap.end())
			{
				std::map<std::string, CMSP>::iterator	it=mRessourceMap.find(filename);
				mRessourceMap.erase(it);				
			}
		}
	}
	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));
}


void	RessourceFileManager::UnloadRessource(const CMSP& pRessource)
{
	// search this Ressource
	removeItem(pRessource);
}

void	RessourceFileManager::UnloadRessource(std::string ressourcename)
{
	if(mRessourceMap.find(ressourcename)!=mRessourceMap.end())
	{
		std::map<std::string, CMSP>::iterator	it=mRessourceMap.find(ressourcename);
		removeItem((*it).second);
	}
}
