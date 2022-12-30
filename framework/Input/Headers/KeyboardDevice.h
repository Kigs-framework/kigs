#pragma once

#include "InputDevice.h"
#include "Platform/Input/KeyDefine.h"

#include "AttributePacking.h"
#include <mutex>

namespace Kigs
{
	namespace Input
	{
		class DeviceItemBaseState;

		// ****************************************
		// * KeyboardDevice class
		// * --------------------------------------
		/**
		 * \class	KeyboardDevice
		 * \file	KeyboardDevice.h
		 * \ingroup Input
		 * \brief	Base class to manage generic Keyboard.
		 */
		 // ****************************************

		class KeyboardDevice : public InputDevice
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(KeyboardDevice, InputDevice, Input)
				DECLARE_INLINE_CONSTRUCTOR(KeyboardDevice) {}

			SIGNALS(KeyboardEvent);
			WRAP_METHODS(GetKey, Show, Hide, ScanToChar);

			virtual const DeviceItemBaseState& getKeyState(int key_id) = 0;

			virtual u16 ScanToChar(u32 scanCode, u32* vkCode) { return u'0'; };

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

			inline std::vector<KeyEvent>& Get_KeyUpList() { return mKeyUpList; }
			inline std::vector<KeyEvent>& Get_KeyDownList() { return mKeyDownList; }

			inline bool IsKeyPressed(int key_id)
			{
				for (auto& ev : mKeyUpList)
				{
					if (ev.KeyCode == key_id) return true;
				}
				return false;
			}

			virtual void Show() {};
			virtual void Hide() {};

		protected:
			std::vector<KeyEvent> mKeyUpList;
			std::vector<KeyEvent> mKeyDownList;

			mutable std::mutex mMutex;
			std::u16string mTyped;
		};

	}
}
