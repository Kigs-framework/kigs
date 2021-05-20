#ifndef _INSTANCEFACTORY_H_
#define _INSTANCEFACTORY_H_

#include "CoreModifiable.h"
#include <unordered_map>

class KigsCore;
class RegisterClassToInstanceFactory;
class ModuleAssociation;

// ****************************************
// * InstanceFactory class
// * --------------------------------------
/**
 * \file	InstanceFactory.h
 * \class	InstanceFactory
 * \ingroup Core
 * \ingroup Manager
 * \brief	register and instanciate classes
 */
 // ****************************************
class InstanceFactory
{
public:
	/**
	 * \fn		InstanceFactory(Core* core);
	 * \brief	constructor
	 * \param	core : instance of Core
	 */
	InstanceFactory(KigsCore* core);


	/**
	 * \fn 		CoreModifiable*    GetInstance(const std::string& instancename,const STRINGS_NAME_TYPE& classname);
	 * \brief	create an instance with the given name of the given class type
	 * \param	instancename : name of the new instance
	 * \param	classname : class of the new instance
	 * \return	the instance of 'classname' named 'instancename', or a DoNothingObject if failed
	 */
	CMSP GetInstance(const std::string& instancename,const KigsID& classname, std::vector<CoreModifiableAttribute*>* args = nullptr);

	/**
	 * \fn 		void  RegisterClass(createMethod method,const STRINGS_NAME_TYPE& className,const std::string& moduleName);
	 * \brief	register a new class in the instance factory
	 * \param	method : create method of the class
	 * \param	className : class to register
	 * \param	moduleName : name of the module which contain the class
	 */
	void  RegisterClass(createMethod method,const KigsID& className, const std::string& moduleName);

	/**
	 * \fn 		bool  GetModuleNameFromClassName(const STRINGS_NAME_TYPE& className, std::string &ModuleName);
	 * \brief	Get a module name from a class name
	 * \param	className : class to register
	 * \param	ModuleName : name of the module which contain the class (out param)
	 * \return	TRUE if the module is found, FALSE otherwise
	 */
	bool  GetModuleIDFromClassName(const KigsID& className, KigsID& ModuleID);

	/*
	* \fn 		void  SubcribeToEventForClassName(const STRINGS_NAME_TYPE& className);
	* \brief	add class name to event class name list
	* \param	className : class to register
	*/
	inline void  SubcribeToEventForClassName(const KigsID& className)
	{
		if (mEventClassList.find(className) == mEventClassList.end())
			mEventClassList.insert(className);
	}
	// ****************************************
	// * ModuleAssociation class
	// * --------------------------------------
	/**
	 * \class	ModuleAssociation
	 * \ingroup Core
	 * \ingroup Manager
	 * \brief	Utility class to store "createInstance" method for each class of a module.
	 */
	 // ****************************************
	class ModuleAssociation
	{
	public:
		/**
		 * \fn 		void RegisterClass(createMethod method,const STRINGS_NAME_TYPE& className);
		 * \brief	register the create method for the given class name
		 * \param	method : create method of the class
		 * \param	className : class to register
		 */
		void              RegisterClass(createMethod method,const KigsID& className);

		/**
		 * \fn 		createMethod      GetCreateMethod(const STRINGS_NAME_TYPE& classname) const;
		 * \brief	get the create method for the given class name
		 * \param	classname : class to look for
		 * \return	the create method of the class
		 */
		createMethod      GetCreateMethod(const KigsID& classname) const;

		//!  registered class list for this module
		kigs::unordered_map<KigsID, createMethod> mClassMap;
	};

	//! get the registered module list
	const kigs::unordered_map<KigsID, ModuleAssociation>& GetModuleList() { return mModuleList; }

	// auto add a callback to all created object
	void	addModifiableCallback(const KigsID& signal, CoreModifiable* target, const KigsID& slot,KigsID filter="CoreModifiable");
	void	removeModifiableCallback(const KigsID& signal, CoreModifiable* target, const KigsID& slot);

	void	addAlias(const std::string& alias, const std::vector<std::string>& list)
	{
		std::vector<std::string> validateList;
		for (const auto& a : list)
		{
			if(a != "")
			{
				validateList.push_back(a);
			}
		}
		if(validateList.size())
			mAliasList[alias] = validateList;
	}

protected:

	void registerCallbackList(CoreModifiable* created);

	//! pointer on core module singleton
	KigsCore*     mCore;

	//! map of registered modules
	kigs::unordered_map<KigsID, ModuleAssociation> mModuleList;

	std::set<KigsID> mEventClassList;

	struct CallbackStruct
	{
		KigsID						filter;	// apply callback only if instance is of this type (default = CoreModifiable)
		CoreModifiable*				target; // instance being notified when signal is send
		KigsID						slot; // slot being called when signal is send
	};

	kigs::unordered_map<KigsID, std::vector<CallbackStruct>>	mModifiableCallbackMap;
	kigs::unordered_map<KigsID, std::vector<std::string> >	mAliasList;
};

#endif //_INSTANCEFACTORY_H_
