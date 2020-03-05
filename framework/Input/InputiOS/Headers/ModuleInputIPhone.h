#ifndef _MODULEINPUTIPHONE_H_
#define _MODULEINPUTIPHONE_H_

#include "ModuleBase.h"

class ModuleInputIPhone;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformInputModuleInit
extern ModuleBase* PlatformInputModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

/*! \defgroup InputIPhone InputIPhone module
 *  manage keyboards, pad, mouse... with IPhone
	\ingroup Input
*/


// ****************************************
// * ModuleInputIPhone class
// * --------------------------------------
/*!  \class ModuleInputIPhone
     this class is the module manager class
	 \ingroup InputIPhone
*/
// ****************************************

class ModuleInputIPhone : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleInputIPhone,ModuleBase,Input)
             
	ModuleInputIPhone(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params); 
    void Close();
    virtual void Update(const Timer& timer, void* addParam);

	bool	isInit(){return myIsInitOK;}

	unsigned int	getJoystickCount(){return myJoystickCount;}

	virtual bool	addItem(CoreModifiable *item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME);

protected:
    virtual ~ModuleInputIPhone();

	bool					myIsInitOK;
	unsigned int			myJoystickCount;

}; 

#endif //_MODULEINPUTIPHONE_H_
