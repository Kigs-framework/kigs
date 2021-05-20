#include "PrecompiledHeaders.h"
#include "InstanceFactory.h"
#include "Core.h"
#include "NotificationCenter.h"
#include "CoreModifiable.h"

#include <algorithm>

//! constructor
InstanceFactory::InstanceFactory(KigsCore* core) 
{
    mCore=core;
}

//! create an instance of the given class type with the given instance name    
bool  InstanceFactory::GetModuleIDFromClassName(const KigsID& className, KigsID& ModuleName)
{
	KigsCore::Instance()->GetSemaphore();

	//! for each module try to find the class type
	for(auto it = mModuleList.begin(); it != mModuleList.end(); ++it)
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
CMSP InstanceFactory::GetInstance(const std::string& instancename,const KigsID& className, std::vector<CoreModifiableAttribute*>* args)
{
	KigsID	realClassName(className);
	// check alias
	auto aliasFound = mAliasList.find(className);
	if (aliasFound != mAliasList.end())
	{
		realClassName = (*aliasFound).second[0];
	}

	//! for each module try to find the class type
	KigsCore::Instance()->GetSemaphore();
	createMethod method=0;
	for(auto& it : mModuleList)
	{
		method = it.second.GetCreateMethod(realClassName);
		//! if class type is found then return a new instance
		if(method)
		{
			break;
		}      
	}
	KigsCore::Instance()->ReleaseSemaphore();
	if (method)
	{
		CMSP L_tmp = (method)(instancename, args);
		
		// add upgrador if needed
		if (aliasFound != mAliasList.end())
		{
			for (int i = 1; i < (*aliasFound).second.size(); i++)
			{
				L_tmp->Upgrade((*aliasFound).second[i]);
			}
		}

		if (L_tmp && mEventClassList.find(realClassName) != mEventClassList.end())
			KigsCore::GetNotificationCenter()->postNotificationName("Create_CoreModifiable", 0, L_tmp.get());

		// add callback on objects
		if (mModifiableCallbackMap.size())
		{
			registerCallbackList(L_tmp.get());
		}

		return L_tmp;
	}   
	//! no corresponding class was found, return 0
	return nullptr;
}    

//! utility method : add a class (in fact a pointer to the create method) to to class map for this module
void    InstanceFactory::ModuleAssociation::RegisterClass(createMethod method,const KigsID& className)
{
	mClassMap[className] = method;
}

void	InstanceFactory::addModifiableCallback(const KigsID& signal, CoreModifiable* target,const KigsID& slot,KigsID filter)
{
	CallbackStruct	toAdd;
	toAdd.target = target;
	toAdd.slot = slot;
	toAdd.filter = filter;
	mModifiableCallbackMap[signal].push_back(toAdd);

}
void	InstanceFactory::removeModifiableCallback(const KigsID& signal, CoreModifiable* target, const KigsID& slot)
{
	auto itfound = mModifiableCallbackMap.find(signal);

	if (itfound != mModifiableCallbackMap.end())
	{
		auto iterase = std::find_if(itfound->second.begin(), itfound->second.end(), 
			[target, slot](const CallbackStruct& cb) { return cb.slot == slot && cb.target == target; });
		if(iterase != itfound->second.end())
			itfound->second.erase(iterase);
		if (itfound->second.size() == 0)
		{
			mModifiableCallbackMap.erase(itfound);
		}
	}
}

void InstanceFactory::registerCallbackList(CoreModifiable* created)
{
	for (const auto& it : mModifiableCallbackMap)
	{
		for (const auto& cbstruct : it.second)
		{
			if(created->isSubType(cbstruct.filter))
				KigsCore::Connect(created, it.first, cbstruct.target, cbstruct.slot);
		}
	}
}

createMethod  InstanceFactory::ModuleAssociation::GetCreateMethod(const KigsID& classname) const
{
	auto it = mClassMap.find(classname);
	if (it != mClassMap.end())
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
void    InstanceFactory::RegisterClass(createMethod method,const KigsID& className, const std::string& moduleName)
{
	mModuleList[moduleName].RegisterClass(method, className);
}  
