#ifndef _MODULEINPUTWUP_H_
#define _MODULEINPUTWUP_H_

#include "ModuleBase.h"
#include "ModuleInput.h"

class ModuleInputWUP : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleInputWUP,ModuleBase,Input)
             
	ModuleInputWUP(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params); 
    void Close();
	virtual void Update(const Timer& timer, void* addParam);
	    
	bool	isInit(){return myIsInitOK;}

	unsigned int	getJoystickCount(){return myJoystickCount;}

	virtual bool	addItem(CoreModifiable *item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME);


protected:
    virtual ~ModuleInputWUP();
  
	bool					myIsInitOK;
	unsigned int			myJoystickCount;

}; 

#endif //_MODULEINPUTWUP_H_
