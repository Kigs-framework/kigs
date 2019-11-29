#ifndef _MODULEINPUTJAVASCRIPT_H_
#define _MODULEINPUTJAVASCRIPT_H_

#include "ModuleBase.h"

class ModuleInputJavascript;
extern ModuleInputJavascript* gInstanceModuleInputJavascript;



#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformInputModuleInit
extern ModuleBase* PlatformInputModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

/*! \defgroup InputJavascript InputJavascript module
 *  manage keyboards, pad, mouse... with Javascript
	\ingroup Input
*/


// ****************************************
// * ModuleInputJavascript class
// * --------------------------------------
/*!  \class ModuleInputJavascript
     this class is the module manager class
	 \ingroup InputJavascript
*/
// ****************************************

class ModuleInputJavascript : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleInputJavascript,ModuleBase,Input)
             
	ModuleInputJavascript(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
    void Close() override;
	void Update(const Timer& timer, void* addParam) override;

	bool	isInit(){return myIsInitOK;}

	unsigned int	getJoystickCount(){return myJoystickCount;}

	bool addItem(CoreModifiable *item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;

protected:
    virtual ~ModuleInputJavascript();

	bool					myIsInitOK;
	unsigned int			myJoystickCount;

}; 

#endif //_MODULEINPUTJAVASCRIPT_H_
