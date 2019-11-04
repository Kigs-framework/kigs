#ifndef _KEYBOARDDX_H_
#define _KEYBOARDDX_H_

#include "KeyboardDevice.h"
#include "ModuleInputDX.h"
#include "DeviceItem.h"

// ****************************************
// * KeyboardDX class
// * --------------------------------------
/*!  \class KeyboardDX
     DirectX keyboard management
	 \ingroup InputDX
*/
// ****************************************

class	KeyboardDX : public KeyboardDevice
{
public:
    DECLARE_CLASS_INFO(KeyboardDX,KeyboardDevice,Input)

    KeyboardDX(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	LPDIRECTINPUTDEVICE8& getDirectInputKeyboard(){return myDirectInputKeyboard;}

	virtual const DeviceItemBaseState&	getKeyState(int key_id){return *myDeviceItems[key_id]->getState();}
    
	virtual void	UpdateDevice();

	virtual bool	Aquire();
	virtual bool	Release();

	void	DoInputDeviceDescription();

	u16 ScanToChar(u32 scanCode) override;
	byte* getKeys() {return myKeys.state;}
    
protected:
    virtual ~KeyboardDX();

	typedef struct tagKeyboard
	{
		byte state[256];
	} keyboard_t;
	
	keyboard_t myKeys;

	LPDIRECTINPUTDEVICE8 myDirectInputKeyboard;  
  
};    

#endif //_KEYBOARDDX_H_
