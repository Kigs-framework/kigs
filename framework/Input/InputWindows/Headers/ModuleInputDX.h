#ifndef _MODULEINPUTDX_H_
#define _MODULEINPUTDX_H_

#include "ModuleBase.h"

#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <commctrl.h>
#include <basetsd.h>
#include <dinput.h>

class ModuleInputDX;

#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformInputModuleInit
extern ModuleBase* PlatformInputModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

/*! \defgroup InputDX InputDX module
 *  manage keyboards, pad, mouse... with direct X
	\ingroup Input
*/


// ****************************************
// * ModuleInputDX class
// * --------------------------------------
/*!  \class ModuleInputDX
     this class is the module manager class
	 \ingroup InputDX
*/
// ****************************************

class ModuleInputDX : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleInputDX,ModuleBase,Input)
             
	ModuleInputDX(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params); 
    void Close();
	virtual void Update(const Timer& timer, void* addParam);

	LPDIRECTINPUT8	getDirectInput(){return myDirectInput;}
    
	bool	isInit(){return myIsInitOK;}

	unsigned int	getJoystickCount(){return myJoystickCount;}

	virtual bool	addItem(CMSP& item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME);


protected:
    virtual ~ModuleInputDX();

	LPDIRECTINPUT8			myDirectInput;   
	bool					myIsInitOK;
	unsigned int			myJoystickCount;

}; 

#endif //_MODULEINPUTDX_H_
