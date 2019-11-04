#include "PrecompiledHeaders.h"
#include "InstanceFactory.h"
#include "Core.h"
#include "NotificationCenter.h"
#include "CoreModifiable.h"

#include <algorithm>

//! constructor
InstanceFactory::InstanceFactory(KigsCore* core) 
{
    myCore=core;
}

//! create an instance of the given class type with the given instance name    
bool  InstanceFactory::GetModuleIDFromClassName(KigsID className, KigsID& ModuleName)
{
	KigsCore::Instance()->GetSemaphore();

	//! for each module try to find the class type
	for(auto it = myModuleList.begin(); it != myModuleList.end(); ++it)
	{
		ModuleAssociation& moduleassoc = it->second;
		if(moduleassoc.GetCreateMethod(className))
		{
			ModuleName = it->first;
			KigsCore::Instance()->ReleaseSemaphore();
			return true;
		}
	}

	//! no corresponding class was found, return false and empty string
	ModuleName = "";
	KigsCore::Instance()->ReleaseSemaphore();
	return false;
}    

//! create an instance of the given class type with the given instance name    
CoreModifiable*    InstanceFactory::GetInstance(const kstl::string& instancename, KigsID className, kstl::vector<CoreModifiableAttribute*>* args)
{
	//! for each module try to find the class type
	KigsCore::Instance()->GetSemaphore();
	createMethod method=0;
	for(auto& it : myModuleList)
	{
		method = it.second.GetCreateMethod(className);
		//! if class type is found then return a new instance
		if(method)
		{
			break;
		}      
	}
	KigsCore::Instance()->ReleaseSemaphore();
	if (method)
	{
		CoreModifiable* L_tmp = (method)(instancename, args);
		if (L_tmp && myEventClassList.find(className) != myEventClassList.end())
			KigsCore::GetNotificationCenter()->postNotificationName("Create_CoreModifiable", 0, L_tmp);

		// add callback on objects
		if (myModifiableCallbackMap.size())
		{
			registerCallbackList(L_tmp);
		}

		return L_tmp;
	}   
	//! no corresponding class was found, return 0
	return nullptr;
}    

//! utility method : add a class (in fact a pointer to the create method) to to class map for this module
void    InstanceFactory::ModuleAssociation::RegisterClass(createMethod method, KigsID className)
{
	myClassMap[className] = method;
}

void	InstanceFactory::addModifiableCallback(const KigsID& signal, CoreModifiable* target,const KigsID& slot,KigsID filter)
{
	CallbackStruct	toAdd;
	toAdd.target = target;
	toAdd.slot = slot;
	toAdd.filter = filter;
	myModifiableCallbackMap[signal].push_back(toAdd);

}
void	InstanceFactory::removeModifiableCallback(const KigsID& signal, CoreModifiable* target, const KigsID& slot)
{
	auto itfound = myModifiableCallbackMap.find(signal);

	if (itfound != myModifiableCallbackMap.end())
	{
		auto iterase = std::find_if(itfound->second.begin(), itfound->second.end(), 
			[target, slot](const CallbackStruct& cb) { return cb.slot == slot && cb.target == target; });
		if(iterase != itfound->second.end())
			itfound->second.erase(iterase);
		if (itfound->second.size() == 0)
		{
			myModifiableCallbackMap.erase(itfound);
		}
	}
}

void InstanceFactory::registerCallbackList(CoreModifiable* created)
{
	for (const auto& it : myModifiableCallbackMap)
	{
		for (const auto& cbstruct : it.second)
		{
			if(created->isSubType(cbstruct.filter))
				KigsCore::Connect(created, it.first, cbstruct.target, cbstruct.slot);
		}
	}
}

createMethod  InstanceFactory::ModuleAssociation::GetCreateMethod(KigsID classname) const
{
	auto it = myClassMap.find(classname);
	if (it != myClassMap.end())
	{
		#ifdef _DEBUG
		if(it->second == NULL)
		{
			printf("Unknown create method\n");
		}
		#endif
		return it->second;
	}
	return nullptr;
}    

//! register a new class to instance factory
void    InstanceFactory::RegisterClass(createMethod method, KigsID className, const kstl::string& moduleName)
{
	myModuleList[moduleName].RegisterClass(method, className);
}  
