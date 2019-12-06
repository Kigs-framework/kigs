#ifndef _KEYBOARDJAVASCRIPT_H_
#define _KEYBOARDJAVASCRIPT_H_

#include "KeyboardDevice.h"
#include "ModuleInputJavascript.h"
#include "DeviceItem.h"

// ****************************************
// * KeyboardJavascript class
// * --------------------------------------
/*!  \class KeyboardJavascript
     JavaScript keyboard management
  \ingroup InputJavaScript
*/
// ****************************************

class	KeyboardJavascript : public KeyboardDevice
{
	public:
    DECLARE_CLASS_INFO(KeyboardJavascript,KeyboardDevice,InputJavaScript)
	
		KeyboardJavascript(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
	const DeviceItemBaseState&	getKeyState(int key_id) override {return *myDeviceItems[key_id]->getState();}
	
	void	UpdateDevice() override;
	
	void	DoInputDeviceDescription() override;
	
	void	Set_RecordingString(bool _value);
	
	inline kstl::string	Get_RecordingString() const {return MyString;}
    
	protected:
    virtual ~KeyboardJavascript();
	
	char Tab[256];
	bool RecordingString_Enable;
	kstl::string MyString;
};    

#endif //_KEYBOARDJAVASCRIPT_H_
