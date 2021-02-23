#ifndef _ANONYMOUSMODULE_H_
#define _ANONYMOUSMODULE_H_

#include "ModuleBase.h"

// ****************************************
// * AnonymousModule class
// * --------------------------------------
/**
 * \file	AnonymousModule.h
 * \class	AnonymousModule
 * \ingroup Core
 * \brief	Base class for anonymous module
 *
 * this class is used to load modules from dll without having a base module in a static lib 
 * an anonymous module generally register very specific classes (sfx, 3D objects, ...) which are seen from
 * the application as basic objects 
 * Example : a runtime generated and rendered tree class (forest) is used by the scenegraph like a CoordinateSystem
 * there's no need for a base class for it (other than CoordinateSystem) so an anonymous module is fine 
 */
// ****************************************
class AnonymousModule : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(AnonymousModule,ModuleBase,KigsCore)

	/**
	 * \fn 		AnonymousModule(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 *
	 * an anonymous module can be instanciated
	 */
	AnonymousModule(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/**
	 * \fn 		~AnonymousModule();
	 * \brief	destructor
	 *
	 * virtual destructor for inherited module
	 */
	virtual ~AnonymousModule();

	/**
	 * \fn			void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);  
	 * \brief		initialize module 
	 * \param		core : link to the core, NOT NULL
	 * \param		params : list of parameters, CAN BE NULL
	 */
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;  

	/**
	 * \fn			void Close();
	 * \brief		close module
	 *
	 * call BaseClose();
	 */
	void Close() override;

	/**
	 * \fn			virtual void Update(const Timer& timer, void* addParam); 
	 * \brief		update module 
	 * \param		timer : timer global
	 *
	 * call BaseUpdate(timer);
	 */ 
	void Update(const Timer& timer, void* addParam) override;


	CoreModifiable*	operator->() const {
            return mDynamicModule;
    }

protected:

	CoreModifiable*		mDynamicModule;
}; 

// use this base class for Dll modules instead of ModuleBase
class BaseDllModule : public ModuleBase
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(BaseDllModule, ModuleBase, DllModule)
	DECLARE_INLINE_CONSTRUCTOR(BaseDllModule)
	{
		
	}

	virtual void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override
	{
		ModuleBase::StaticInitCore(core);
	}
};

#endif //_ANONYMOUSMODULE_H_
