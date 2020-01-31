#ifndef _KEY_DEFINE_H_
#define _KEY_DEFINE_H_

struct KeyEvent
{
public:
	// Taken from adroid API
	static const int ACTION_DOWN = 0;
	static const int ACTION_UP = 1;
	static const int ACTION_MULTIPLE = 2;



	int Action;
	int KeyCode;
	int Unicode;
	int flag;

public:
	void print()
	{
		printf("Action:%d   KeyCode:%d   Unicode:%d   flag:%d", Action, KeyCode, Unicode, flag);
	}
};

#define CM_KEY_ESCAPE          0x1B
#define CM_KEY_0               0x30
#define CM_KEY_1               0x31
#define CM_KEY_2               0x32
#define CM_KEY_3               0x33
#define CM_KEY_4               0x34
#define CM_KEY_5               0x35
#define CM_KEY_6               0x36
#define CM_KEY_7               0x37
#define CM_KEY_8               0x38
#define CM_KEY_9               0x39
#define CM_KEY_MINUS           0xBD    /* - on main keyboard */
#define CM_KEY_EQUALS          0xBB
#define CM_KEY_BACK            int(winrt::Windows::System::VirtualKey::Back)    /* backspace */
#define CM_KEY_TAB             0x0F

#define CM_KEY_A               0x41
#define CM_KEY_B               0x42
#define CM_KEY_C               0x43
#define CM_KEY_D               0x44
#define CM_KEY_E               0x45
#define CM_KEY_F               0x46
#define CM_KEY_G               0x47
#define CM_KEY_H               0x48
#define CM_KEY_I               0x49
#define CM_KEY_J               0x4A
#define CM_KEY_K               0x4B
#define CM_KEY_L               0x4C
#define CM_KEY_M               0x4D
#define CM_KEY_N               0x4E
#define CM_KEY_O               0x4F
#define CM_KEY_P               0x50
#define CM_KEY_Q               0x51
#define CM_KEY_R               0x52
#define CM_KEY_S               0x53
#define CM_KEY_T               0x54
#define CM_KEY_U               0x55
#define CM_KEY_V               0x56
#define CM_KEY_W               0x57
#define CM_KEY_X               0x58
#define CM_KEY_Y               0x59
#define CM_KEY_Z               0x5A

#define CM_KEY_RETURN          0x0D    /* Enter on main keyboard */
#define CM_KEY_LCONTROL        0x11
#define CM_KEY_RCONTROL        0xFF // not sure it exist
//#define CM_KEY_SEMICOLON       0xBE
//#define CM_KEY_APOSTROPHE      0x28
//#define CM_KEY_GRAVE           0x29    /* accent grave */
#define CM_KEY_LSHIFT          0xA0
#define CM_KEY_BACKSLASH       0xE2
#define CM_KEY_COMMA           0xBC
#define CM_KEY_PERIOD          0xBE    /* . on main keyboard */
//#define CM_KEY_SLASH           0x35    /* / on main keyboard */
#define CM_KEY_RSHIFT          0xA1
#define CM_KEY_MULTIPLY        0x6A    /* * on numeric keypad */
#define CM_KEY_LMENU           0xA4    /* left Alt */
#define CM_KEY_SPACE           0x20
#define CM_KEY_CAPITAL         0x14
#define CM_KEY_F1              0x70
#define CM_KEY_F2              0x71
#define CM_KEY_F3              0x72
#define CM_KEY_F4              0x73
#define CM_KEY_F5              0x74
#define CM_KEY_F6              0x75
#define CM_KEY_F7              0x76
#define CM_KEY_F8              0x77
#define CM_KEY_F9              0x78
#define CM_KEY_F10             0x79
#define CM_KEY_F11             0x7A
#define CM_KEY_F12             0x7B
#define CM_KEY_F13             0x7C    /*                     (NEC PC98) */
#define CM_KEY_F14             0x7D    /*                     (NEC PC98) */
#define CM_KEY_F15             0x7E    /*                     (NEC PC98) */
#define CM_KEY_NUMLOCK         0x90
#define CM_KEY_SCROLL          0x91    /* Scroll Lock */
#define CM_KEY_NUMPAD0         0x60
#define CM_KEY_NUMPAD1         0x61
#define CM_KEY_NUMPAD2         0x62
#define CM_KEY_NUMPAD3         0x63
#define CM_KEY_NUMPAD4         0x64
#define CM_KEY_NUMPAD5         0x65
#define CM_KEY_NUMPAD6         0x66
#define CM_KEY_NUMPAD7         0x67
#define CM_KEY_NUMPAD8         0x68
#define CM_KEY_NUMPAD9         0x69
#define CM_KEY_NUMPADENTER     0x0D
#define CM_KEY_SUBTRACT        0x6D    /* - on numeric keypad */
#define CM_KEY_ADD             0x6B    /* + on numeric keypad */
#define CM_KEY_DECIMAL         0x6E    /* . on numeric keypad */
#define CM_KEY_OEM_102         0xE2    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define CM_KEY_KANA            0x15    /* (Japanese keyboard)            */
#define CM_KEY_CONVERT         0x1C    /* (Japanese keyboard)            */
#define CM_KEY_NOCONVERT       0x1D    /* (Japanese keyboard)            */
#define CM_KEY_RMENU           0xA5    /* right Alt */
#define CM_KEY_PAUSE           0xB3    /* Pause */
#define CM_KEY_HOME            0x24    /* Home on arrow keypad */
#define CM_KEY_UP              0x26    /* UpArrow on arrow keypad */
#define CM_KEY_PRIOR           0xC9    /* PgUp on arrow keypad */
#define CM_KEY_LEFT            0x25    /* LeftArrow on arrow keypad */
#define CM_KEY_RIGHT           0x27    /* RightArrow on arrow keypad */
#define CM_KEY_END             0xCF    /* End on arrow keypad */
#define CM_KEY_DOWN            0x28    /* DownArrow on arrow keypad */
#define CM_KEY_NEXT            0xD1    /* PgDn on arrow keypad */
#define CM_KEY_INSERT          0x2D    /* Insert on arrow keypad */
#define CM_KEY_DELETE          0x2E    /* Delete on arrow keypad */
#define CM_KEY_LWIN            0x5B    /* Left Windows key */
#define CM_KEY_RWIN            0x5C    /* Right Windows key */
#define CM_KEY_APPS            0x5D    /* AppMenu key */
#define CM_KEY_SLEEP           0x5F    /* System Sleep */

#endif