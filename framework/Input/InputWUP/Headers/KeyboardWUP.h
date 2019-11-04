#ifndef _KEYBOARDWUP_H_
#define _KEYBOARDWUP_H_

#include "KeyboardDevice.h"
#include "ModuleInputWUP.h"
#include "DeviceItem.h"
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Devices.Input.h>

#include <mutex>

class	KeyboardWUP : public KeyboardDevice
{
public:
	DECLARE_CLASS_INFO(KeyboardWUP, KeyboardDevice, Input);
	KeyboardWUP(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	const DeviceItemBaseState&	getKeyState(int key_id)override {return *myDeviceItems[key_id]->getState();}
    
	void	UpdateDevice()override;

	bool	Aquire()override;
	bool	Release()override;
	
	void	DoInputDeviceDescription();
	
	unsigned short ScanToChar(u32 scanCode) override;
    
	protected:
    virtual ~KeyboardWUP();
	
	std::vector<std::pair<int, winrt::Windows::UI::Core::KeyEventArgs>> mEvents;
};    

#endif //_KEYBOARDWUP_H_
