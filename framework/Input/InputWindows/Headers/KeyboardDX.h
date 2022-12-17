#ifndef _KEYBOARDDX_H_
#define _KEYBOARDDX_H_

#include "KeyboardDevice.h"
#include "ModuleInputDX.h"
#include "DeviceItem.h"

// ****************************************
// * KeyboardDX class
// * --------------------------------------
/**
* \file	KeyboardDX.h
* \class	KeyboardDX
* \ingroup Input
* \brief Specific DirectX keyboard device.
*
*/
// ****************************************
class	KeyboardDX : public KeyboardDevice
{
public:
    DECLARE_CLASS_INFO(KeyboardDX,KeyboardDevice,Input)
    KeyboardDX(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~KeyboardDX();

	LPDIRECTINPUTDEVICE8& getDirectInputKeyboard(){return mDirectInputKeyboard;}

	virtual const DeviceItemBaseState&	getKeyState(int key_id){return *mDeviceItems[key_id]->getState();}
    
	virtual void	UpdateDevice();

	virtual bool	Aquire();
	virtual bool	Release();

	void	DoInputDeviceDescription();

	u16 ScanToChar(u32 scanCode, u32* vkCode) override;
	byte* getKeys() {return mKeys.state;}
    
protected:
	typedef struct tagKeyboard
	{
		byte state[256];
	} keyboard_t;
	
	keyboard_t				mKeys;

	LPDIRECTINPUTDEVICE8	mDirectInputKeyboard;  

	HKL						mLayout;
  
};    

#endif //_KEYBOARDDX_H_
