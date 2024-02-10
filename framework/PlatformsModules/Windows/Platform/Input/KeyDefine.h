#ifndef _KEY_DEFINE_H_
#define _KEY_DEFINE_H_

#include "CoreTypes.h"

struct KeyEvent
{
public:
	// Taken from adroid API
	static const int ACTION_DOWN = 0;
	static const int ACTION_UP = 1;
	static const int ACTION_MULTIPLE = 2;

	int Action;
	Kigs::u32 KeyCode;
	int Unicode;
	int flag;

public:
	void print()
	{
		printf("Action:%d   KeyCode:%d   Unicode:%d   flag:%d", Action, KeyCode, Unicode, flag);
	}
};


/*
 * Virtual Keys, Standard Set
 */
#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */
#define VK_XBUTTON1       0x05    /* NOT contiguous with L & RBUTTON */
#define VK_XBUTTON2       0x06    /* NOT contiguous with L & RBUTTON */
 /*
  * 0x07 : reserved
  */
#define VK_BACK           0x08
#define VK_TAB            0x09
  /*
   * 0x0A - 0x0B : reserved
   */
#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D
   /*
	* 0x0E - 0x0F : unassigned
	*/
#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14
#define VK_KANA           0x15
#define VK_HANGUL         0x15
	/*
	 * 0x16 : unassigned
	 */
#define VK_JUNJA          0x17
#define VK_FINAL          0x18
#define VK_HANJA          0x19
#define VK_KANJI          0x19
	 /*
	  * 0x1A : unassigned
	  */
#define VK_ESCAPE         0x1B
#define VK_CONVERT        0x1C
#define VK_NONCONVERT     0x1D
#define VK_ACCEPT         0x1E
#define VK_MODECHANGE     0x1F
#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F
#define VK_0			  0x30
#define VK_1			  0x31
#define VK_2			  0x32
#define VK_3			  0x33
#define VK_4			  0x34
#define VK_5			  0x35
#define VK_6			  0x36
#define VK_7			  0x37
#define VK_8			  0x38
#define VK_9			  0x39
	  /*
	   * 0x3A - 0x40 : unassigned
	   */
#define VK_A			  0x41
#define VK_B			  0x42
#define VK_C			  0x43
#define VK_D			  0x44
#define VK_E			  0x45
#define VK_F			  0x46
#define VK_G			  0x47
#define VK_H			  0x48
#define VK_I			  0x49
#define VK_J			  0x4A
#define VK_K			  0x4B
#define VK_L			  0x4C
#define VK_M			  0x4D
#define VK_N			  0x4E
#define VK_O			  0x4F
#define VK_P			  0x50
#define VK_Q			  0x51
#define VK_R			  0x52
#define VK_S			  0x53
#define VK_T			  0x54
#define VK_U			  0x55
#define VK_V			  0x56
#define VK_W			  0x57
#define VK_X			  0x58
#define VK_Y			  0x59
#define VK_Z			  0x5A


#define VK_LWIN           0x5B
#define VK_RWIN           0x5C
#define VK_APPS           0x5D

	   /*
		* 0x5E : reserved
		*/

#define VK_SLEEP          0x5F

#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
#define VK_F13            0x7C
#define VK_F14            0x7D
#define VK_F15            0x7E
#define VK_F16            0x7F
#define VK_F17            0x80
#define VK_F18            0x81
#define VK_F19            0x82
#define VK_F20            0x83
#define VK_F21            0x84
#define VK_F22            0x85
#define VK_F23            0x86
#define VK_F24            0x87
#define VK_NUMLOCK		  0x90
#define VK_SCROLL		  0x91
#define VK_LSHIFT		  0xA0
#define VK_RSHIFT		  0xA1
#define VK_LCONTROL		  0xA2
#define VK_RCONTROL		  0xA3
#define VK_LMENU		  0xA4
#define VK_RMENU		  0xA5
#define VK_OEM_1		  0xBA
#define VK_OEM_PLUS		  0xBB
#define VK_OEM_COMMA	0xBC
#define VK_OEM_MINUS	0xBD
#define VK_OEM_PERIOD	0xBE
#define VK_OEM_2		0xBF 
#define VK_OEM_3		0xC0
#define VK_OEM_4		0xDB
#define VK_OEM_5		0xDC
#define VK_OEM_6		0xDD
#define VK_OEM_7		0xDE


#define CM_KEY_ESCAPE          0x01
#define CM_KEY_1               0x02
#define CM_KEY_2               0x03
#define CM_KEY_3               0x04
#define CM_KEY_4               0x05
#define CM_KEY_5               0x06
#define CM_KEY_6               0x07
#define CM_KEY_7               0x08
#define CM_KEY_8               0x09
#define CM_KEY_9               0x0A
#define CM_KEY_0               0x0B
#define CM_KEY_MINUS           0x0C    
#define CM_KEY_EQUALS          0x0D
#define CM_KEY_BACK            0x0E    
#define CM_KEY_TAB             0x0F
#define CM_KEY_Q               0x10
#define CM_KEY_W               0x11
#define CM_KEY_E               0x12
#define CM_KEY_R               0x13
#define CM_KEY_T               0x14
#define CM_KEY_Y               0x15
#define CM_KEY_U               0x16
#define CM_KEY_I               0x17
#define CM_KEY_O               0x18
#define CM_KEY_P               0x19
#define CM_KEY_LBRACKET        0x1A
#define CM_KEY_RBRACKET        0x1B
#define CM_KEY_RETURN          0x1C    
#define CM_KEY_LCONTROL        0x1D
#define CM_KEY_A               0x1E
#define CM_KEY_S               0x1F
#define CM_KEY_D               0x20
#define CM_KEY_F               0x21
#define CM_KEY_G               0x22
#define CM_KEY_H               0x23
#define CM_KEY_J               0x24
#define CM_KEY_K               0x25
#define CM_KEY_L               0x26
#define CM_KEY_SEMICOLON       0x27
#define CM_KEY_APOSTROPHE      0x28
#define CM_KEY_GRAVE           0x29    
#define CM_KEY_LSHIFT          0x2A
#define CM_KEY_BACKSLASH       0x2B
#define CM_KEY_Z               0x2C
#define CM_KEY_X               0x2D
#define CM_KEY_C               0x2E
#define CM_KEY_V               0x2F
#define CM_KEY_B               0x30
#define CM_KEY_N               0x31
#define CM_KEY_M               0x32
#define CM_KEY_COMMA           0x33
#define CM_KEY_PERIOD          0x34    
#define CM_KEY_SLASH           0x35    
#define CM_KEY_RSHIFT          0x36
#define CM_KEY_MULTIPLY        0x37    
#define CM_KEY_LMENU           0x38    
#define CM_KEY_SPACE           0x39
#define CM_KEY_CAPITAL         0x3A
#define CM_KEY_F1              0x3B
#define CM_KEY_F2              0x3C
#define CM_KEY_F3              0x3D
#define CM_KEY_F4              0x3E
#define CM_KEY_F5              0x3F
#define CM_KEY_F6              0x40
#define CM_KEY_F7              0x41
#define CM_KEY_F8              0x42
#define CM_KEY_F9              0x43
#define CM_KEY_F10             0x44
#define CM_KEY_NUMLOCK         0x45
#define CM_KEY_SCROLL          0x46    
#define CM_KEY_NUMPAD7         0x47
#define CM_KEY_NUMPAD8         0x48
#define CM_KEY_NUMPAD9         0x49
#define CM_KEY_SUBTRACT        0x4A   
#define CM_KEY_NUMPAD4         0x4B
#define CM_KEY_NUMPAD5         0x4C
#define CM_KEY_NUMPAD6         0x4D
#define CM_KEY_ADD             0x4E   
#define CM_KEY_NUMPAD1         0x4F
#define CM_KEY_NUMPAD2         0x50
#define CM_KEY_NUMPAD3         0x51
#define CM_KEY_NUMPAD0         0x52
#define CM_KEY_DECIMAL         0x53    
#define CM_KEY_OEM_102         0x56    
#define CM_KEY_F11             0x57
#define CM_KEY_F12             0x58
#define CM_KEY_F13             0x64    
#define CM_KEY_F14             0x65    
#define CM_KEY_F15             0x66    
#define CM_KEY_KANA            0x70   
#define CM_KEY_ABNT_C1         0x73    
#define CM_KEY_CONVERT         0x79    
#define CM_KEY_NOCONVERT       0x7B   
#define CM_KEY_YEN             0x7D    
#define CM_KEY_ABNT_C2         0x7E   
#define CM_KEY_NUMPADEQUALS    0x8D    
#define CM_KEY_PREVTRACK       0x90    
#define CM_KEY_AT              0x91    
#define CM_KEY_COLON           0x92   
#define CM_KEY_UNDERLINE       0x93  
#define CM_KEY_KANJI           0x94    
#define CM_KEY_STOP            0x95   
#define CM_KEY_AX              0x96    
#define CM_KEY_UNLABELED       0x97   
#define CM_KEY_NEXTTRACK       0x99    
#define CM_KEY_NUMPADENTER     0x9C   
#define CM_KEY_RCONTROL        0x9D
#define CM_KEY_MUTE            0xA0   
#define CM_KEY_CALCULATOR      0xA1    
#define CM_KEY_PLAYPAUSE       0xA2    
#define CM_KEY_MEDIASTOP       0xA4    
#define CM_KEY_VOLUMEDOWN      0xAE   
#define CM_KEY_VOLUMEUP        0xB0 
#define CM_KEY_WEBHOME         0xB2   
#define CM_KEY_NUMPADCOMMA     0xB3    
#define CM_KEY_DIVIDE          0xB5    
#define CM_KEY_SYSRQ           0xB7
#define CM_KEY_RMENU           0xB8    
#define CM_KEY_PAUSE           0xC5    
#define CM_KEY_HOME            0xC7    
#define CM_KEY_UP              0xC8   
#define CM_KEY_PRIOR           0xC9    
#define CM_KEY_LEFT            0xCB   
#define CM_KEY_RIGHT           0xCD    
#define CM_KEY_END             0xCF    
#define CM_KEY_DOWN            0xD0   
#define CM_KEY_NEXT            0xD1   
#define CM_KEY_INSERT          0xD2   
#define CM_KEY_DELETE          0xD3   
#define CM_KEY_LWIN            0xDB   
#define CM_KEY_RWIN            0xDC    
#define CM_KEY_APPS            0xDD   
#define CM_KEY_POWER           0xDE   
#define CM_KEY_SLEEP           0xDF    
#define CM_KEY_WAKE            0xE3    
#define CM_KEY_WEBSEARCH       0xE5    
#define CM_KEY_WEBFAVORITES    0xE6    
#define CM_KEY_WEBREFRESH      0xE7    
#define CM_KEY_WEBSTOP         0xE8    
#define CM_KEY_WEBFORWARD      0xE9    
#define CM_KEY_WEBBACK         0xEA    
#define CM_KEY_MYCOMPUTER      0xEB    
#define CM_KEY_MAIL            0xEC    
#define CM_KEY_MEDIASELECT     0xED    

#endif