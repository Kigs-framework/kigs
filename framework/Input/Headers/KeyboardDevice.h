#ifndef _KEYBOARDDEVICE_H_
#define _KEYBOARDDEVICE_H_

#include "InputDevice.h"
#include "Platform/Input/KeyDefine.h"

#include "AttributePacking.h"
#include <mutex>

class DeviceItemBaseState;

class KeyboardDevice : public InputDevice
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(KeyboardDevice, InputDevice, Input)
	DECLARE_INLINE_CONSTRUCTOR(KeyboardDevice) {}

	SIGNALS(KeyboardEvent);
	WRAP_METHODS(GetKey, Show, Hide, ScanToChar);

	virtual const DeviceItemBaseState&	getKeyState(int key_id) = 0;
	virtual u16 ScanToChar(u32 scanCode) { return u'0'; }

	int GetKey(int key_id);

	std::u16string ConsumeTypedCharacters()
	{
		std::lock_guard lk{ mMutex };
		std::u16string result;
		std::swap(mTyped, result);
		return result;
	}

	std::u16string PeekTypedCharacters() const
	{
		std::lock_guard lk{ mMutex };
		return mTyped;
	}

	inline kstl::vector<KeyEvent>&		Get_KeyUpList() { return m_KeyUpList; }
	inline kstl::vector<KeyEvent>&		Get_KeyDownList() { return m_KeyDownList; }

	virtual void Show() {};
	virtual void Hide() {};

protected:
	kstl::vector<KeyEvent> m_KeyUpList;
	kstl::vector<KeyEvent> m_KeyDownList;

	mutable std::mutex mMutex;
	std::u16string mTyped;
};

#endif //_KEYBOARDDEVICE_H_
