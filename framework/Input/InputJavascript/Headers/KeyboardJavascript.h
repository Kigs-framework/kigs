#ifndef _KEYBOARDJAVASCRIPT_H_
#define _KEYBOARDJAVASCRIPT_H_

#include "KeyboardDevice.h"
#include "ModuleInputJavascript.h"
#include "DeviceItem.h"

// ****************************************
// * KeyboardJavascript class
// * --------------------------------------
/**
 * \class	KeyboardJavascript
 * \file	KeyboardJavascript.h
 * \ingroup Input
 * \brief	Javascript Keyboard management.
 */
 // ****************************************
class	KeyboardJavascript : public KeyboardDevice
{
	public:
    DECLARE_CLASS_INFO(KeyboardJavascript,KeyboardDevice,InputJavaScript)
	
		KeyboardJavascript(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
	const DeviceItemBaseState&	getKeyState(int key_id) override {return *mDeviceItems[key_id]->getState();}
	
	void	UpdateDevice() override;
	
	void	DoInputDeviceDescription() override;
	
	void	Set_RecordingString(bool _value);
	
	inline kstl::string	Get_RecordingString() const {return mString;}
     virtual ~KeyboardJavascript();
  
	protected:
 	
	char mTab[256];
	bool mRecordingStringEnable;
	kstl::string mString;
};    

#endif //_KEYBOARDJAVASCRIPT_H_
