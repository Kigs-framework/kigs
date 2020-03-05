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

#define CM_KEY_ESCAPE          111
#define CM_KEY_1               145
#define CM_KEY_2               146
#define CM_KEY_3               147
#define CM_KEY_4               148
#define CM_KEY_5               149
#define CM_KEY_6               150
#define CM_KEY_7               151
#define CM_KEY_8               152
#define CM_KEY_9               153
#define CM_KEY_0               144
#define CM_KEY_MINUS           69    /* - on main keyboard */
#define CM_KEY_EQUALS          70
#define CM_KEY_BACK            67    /* backspace */
#define CM_KEY_TAB             61
#define CM_KEY_A               29
#define CM_KEY_B               30
#define CM_KEY_C               31
#define CM_KEY_D               32
#define CM_KEY_E               33
#define CM_KEY_F               34
#define CM_KEY_G               35
#define CM_KEY_H               36
#define CM_KEY_I               37
#define CM_KEY_J               38
#define CM_KEY_K               39
#define CM_KEY_L               40
#define CM_KEY_M               41
#define CM_KEY_N               42
#define CM_KEY_O               43
#define CM_KEY_P               44
#define CM_KEY_Q               45
#define CM_KEY_R               46
#define CM_KEY_S               47
#define CM_KEY_T               48
#define CM_KEY_U               49
#define CM_KEY_V               50
#define CM_KEY_W               51
#define CM_KEY_X               52
#define CM_KEY_Y               53
#define CM_KEY_Z               54
#define CM_KEY_LBRACKET        71
#define CM_KEY_RBRACKET        72
#define CM_KEY_RETURN          66    /* Enter on main keyboard */
#define CM_KEY_LCONTROL        113
#define CM_KEY_SEMICOLON       74
#define CM_KEY_APOSTROPHE      75
#define CM_KEY_GRAVE           68    /* accent grave */
#define CM_KEY_LSHIFT          59
#define CM_KEY_BACKSLASH       73
#define CM_KEY_COMMA           55
#define CM_KEY_PERIOD          56    /* . on main keyboard */
#define CM_KEY_SLASH           76    /* / on main keyboard */
#define CM_KEY_RSHIFT          60
#define CM_KEY_MULTIPLY        155    /* * on numeric keypad */
#define CM_KEY_LMENU           82    /* left Alt */
#define CM_KEY_SPACE           62
#define CM_KEY_CAPITAL         0
#define CM_KEY_F1              131
#define CM_KEY_F2              132
#define CM_KEY_F3              133
#define CM_KEY_F4              134
#define CM_KEY_F5              135
#define CM_KEY_F6              136
#define CM_KEY_F7              137
#define CM_KEY_F8              138
#define CM_KEY_F9              139
#define CM_KEY_F10             140
#define CM_KEY_NUMLOCK         143
#define CM_KEY_SCROLL          0    /* Scroll Lock */
#define CM_KEY_NUMPAD0         144
#define CM_KEY_NUMPAD1         145
#define CM_KEY_NUMPAD2         146
#define CM_KEY_NUMPAD3         147
#define CM_KEY_NUMPAD4         148
#define CM_KEY_NUMPAD5         149
#define CM_KEY_NUMPAD6         150
#define CM_KEY_NUMPAD7         151
#define CM_KEY_NUMPAD8         152
#define CM_KEY_NUMPAD9         153
#define CM_KEY_SUBTRACT        156    /* - on numeric keypad */
#define CM_KEY_ADD             157    /* + on numeric keypad */
#define CM_KEY_DECIMAL         158    /* . on numeric keypad */
#define CM_KEY_OEM_102         0    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define CM_KEY_F11             141
#define CM_KEY_F12             142
#define CM_KEY_F13             0    /*                     (NEC PC98) */
#define CM_KEY_F14             0    /*                     (NEC PC98) */
#define CM_KEY_F15             0    /*                     (NEC PC98) */
#define CM_KEY_KANA            0x70    /* (Japanese keyboard)            */
#define CM_KEY_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define CM_KEY_CONVERT         0x79    /* (Japanese keyboard)            */
#define CM_KEY_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define CM_KEY_YEN             0x7D    /* (Japanese keyboard)            */
#define CM_KEY_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define CM_KEY_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define CM_KEY_PREVTRACK       0x90    /* Previous Track (CM_KEY_CIRCUMFLEX on Japanese keyboard) */
#define CM_KEY_AT              0x91    /*                     (NEC PC98) */
#define CM_KEY_COLON           0x92    /*                     (NEC PC98) */
#define CM_KEY_UNDERLINE       0x93    /*                     (NEC PC98) */
#define CM_KEY_KANJI           0x94    /* (Japanese keyboard)            */
#define CM_KEY_STOP            0x95    /*                     (NEC PC98) */
#define CM_KEY_AX              0x96    /*                     (Japan AX) */
#define CM_KEY_UNLABELED       0x97    /*                        (J3100) */
#define CM_KEY_NEXTTRACK       0x99    /* Next Track */
#define CM_KEY_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define CM_KEY_RCONTROL        114
#define CM_KEY_MUTE            0xA0    /* Mute */
#define CM_KEY_CALCULATOR      0xA1    /* Calculator */
#define CM_KEY_PLAYPAUSE       0xA2    /* Play / Pause */
#define CM_KEY_MEDIASTOP       0xA4    /* Media Stop */
#define CM_KEY_VOLUMEDOWN      0xAE    /* Volume - */
#define CM_KEY_VOLUMEUP        0xB0    /* Volume + */
#define CM_KEY_WEBHOME         0xB2    /* Web home */
#define CM_KEY_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define CM_KEY_DIVIDE          154    /* / on numeric keypad */
#define CM_KEY_SYSRQ           0xB7
#define CM_KEY_RMENU           0xB8    /* right Alt */
#define CM_KEY_PAUSE           0xC5    /* Pause */
#define CM_KEY_HOME            0xC7    /* Home on arrow keypad */
#define CM_KEY_UP              0xC8    /* UpArrow on arrow keypad */
#define CM_KEY_PRIOR           0xC9    /* PgUp on arrow keypad */
#define CM_KEY_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define CM_KEY_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define CM_KEY_END             0xCF    /* End on arrow keypad */
#define CM_KEY_DOWN            0xD0    /* DownArrow on arrow keypad */
#define CM_KEY_NEXT            0xD1    /* PgDn on arrow keypad */
#define CM_KEY_INSERT          0xD2    /* Insert on arrow keypad */
#define CM_KEY_DELETE          0xD3    /* Delete on arrow keypad */
#define CM_KEY_LWIN            0xDB    /* Left Windows key */
#define CM_KEY_RWIN            0xDC    /* Right Windows key */
#define CM_KEY_APPS            0xDD    /* AppMenu key */
#define CM_KEY_POWER           0xDE    /* System Power */
#define CM_KEY_SLEEP           0xDF    /* System Sleep */
#define CM_KEY_WAKE            0xE3    /* System Wake */
#define CM_KEY_WEBSEARCH       0xE5    /* Web Search */
#define CM_KEY_WEBFAVORITES    0xE6    /* Web Favorites */
#define CM_KEY_WEBREFRESH      0xE7    /* Web Refresh */
#define CM_KEY_WEBSTOP         0xE8    /* Web Stop */
#define CM_KEY_WEBFORWARD      0xE9    /* Web Forward */
#define CM_KEY_WEBBACK         0xEA    /* Web Back */
#define CM_KEY_MYCOMPUTER      0xEB    /* My Computer */
#define CM_KEY_MAIL            0xEC    /* Mail */
#define CM_KEY_MEDIASELECT     0xED    /* Media Select */

#endif