#ifndef _MODULEINPUTANDROID_H_
#define _MODULEINPUTANDROID_H_

#include "ModuleBase.h"

class ModuleInputAndroid;
extern ModuleInputAndroid* gInstanceModuleInputAndroid;

#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformInputModuleInit
extern ModuleBase* PlatformGUIModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

class ModuleInputAndroid : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(ModuleInputAndroid,ModuleBase,Input)
	DECLARE_CONSTRUCTOR(ModuleInputAndroid);
	
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
    void Close() override;
    void Update(const Timer& timer,void* /*addParam*/) override;

	bool	isInit(){return myIsInitOK;}

	unsigned int	getJoystickCount(){return myJoystickCount;}

	bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;

protected:
	bool					myIsInitOK;
	unsigned int			myJoystickCount;

}; 

#endif //_MODULEINPUTANDROID_H_
