-- Useful defines to work with the Kigs Framework

key = {}
button = {}
cursor = {}


function update_behaviour(old_one, new_one)
	old_one.init = new_one.init
	old_one.update = new_one.update
	old_one.addItem = new_one.addItem
	old_one.removeItem = new_one.removeItem
	old_one.destroy = new_one.destroy
end

button.LEFT = 0
button.RIGHT = 1
button.MIDDLE = 2

key.KEY_ESCAPE          = 0x01
key.KEY_1               = 0x02
key.KEY_2               = 0x03
key.KEY_3               = 0x04
key.KEY_4               = 0x05
key.KEY_5               = 0x06
key.KEY_6               = 0x07
key.KEY_7               = 0x08
key.KEY_8               = 0x09
key.KEY_9               = 0x0A
key.KEY_0               = 0x0B
key.KEY_MINUS           = 0x0C    --  - on main keyboard 
key.KEY_EQUALS          = 0x0D
key.KEY_BACK            = 0x0E    --  backspace 
key.KEY_TAB             = 0x0F
key.KEY_Q               = 0x10
key.KEY_W               = 0x11
key.KEY_E               = 0x12
key.KEY_R               = 0x13
key.KEY_T               = 0x14
key.KEY_Y               = 0x15
key.KEY_U               = 0x16
key.KEY_I               = 0x17
key.KEY_O               = 0x18
key.KEY_P               = 0x19
key.KEY_LBRACKET        = 0x1A
key.KEY_RBRACKET        = 0x1B
key.KEY_RETURN          = 0x1C    --  Enter on main keyboard 
key.KEY_LCONTROL        = 0x1D
key.KEY_A               = 0x1E
key.KEY_S               = 0x1F
key.KEY_D               = 0x20
key.KEY_F               = 0x21
key.KEY_G               = 0x22
key.KEY_H               = 0x23
key.KEY_J               = 0x24
key.KEY_K               = 0x25
key.KEY_L               = 0x26
key.KEY_SEMICOLON       = 0x27
key.KEY_APOSTROPHE      = 0x28
key.KEY_GRAVE           = 0x29    --  accent grave 
key.KEY_LSHIFT          = 0x2A
key.KEY_BACKSLASH       = 0x2B
key.KEY_Z               = 0x2C
key.KEY_X               = 0x2D
key.KEY_C               = 0x2E
key.KEY_V               = 0x2F
key.KEY_B               = 0x30
key.KEY_N               = 0x31
key.KEY_M               = 0x32
key.KEY_COMMA           = 0x33
key.KEY_PERIOD          = 0x34    --  . on main keyboard 
key.KEY_SLASH           = 0x35    --  / on main keyboard 
key.KEY_RSHIFT          = 0x36
key.KEY_MULTIPLY        = 0x37    --  * on numeric keypad 
key.KEY_LMENU           = 0x38    --  left Alt 
key.KEY_SPACE           = 0x39
key.KEY_CAPITAL         = 0x3A
key.KEY_F1              = 0x3B
key.KEY_F2              = 0x3C
key.KEY_F3              = 0x3D
key.KEY_F4              = 0x3E
key.KEY_F5              = 0x3F
key.KEY_F6              = 0x40
key.KEY_F7              = 0x41
key.KEY_F8              = 0x42
key.KEY_F9              = 0x43
key.KEY_F10             = 0x44
key.KEY_NUMLOCK         = 0x45
key.KEY_SCROLL          = 0x46    --  Scroll Lock 
key.KEY_NUMPAD7         = 0x47
key.KEY_NUMPAD8         = 0x48
key.KEY_NUMPAD9         = 0x49
key.KEY_SUBTRACT        = 0x4A    --  - on numeric keypad 
key.KEY_NUMPAD4         = 0x4B
key.KEY_NUMPAD5         = 0x4C
key.KEY_NUMPAD6         = 0x4D
key.KEY_ADD             = 0x4E    --  + on numeric keypad 
key.KEY_NUMPAD1         = 0x4F
key.KEY_NUMPAD2         = 0x50
key.KEY_NUMPAD3         = 0x51
key.KEY_NUMPAD0         = 0x52
key.KEY_DECIMAL         = 0x53    --  . on numeric keypad 
key.KEY_OEM_102         = 0x56    --  <> or \| on RT 102-key keyboard (Non-U.S.) 
key.KEY_F11             = 0x57
key.KEY_F12             = 0x58
key.KEY_F13             = 0x64    --                      (NEC PC98) 
key.KEY_F14             = 0x65    --                      (NEC PC98) 
key.KEY_F15             = 0x66    --                      (NEC PC98) 
key.KEY_KANA            = 0x70    --  (Japanese keyboard)            
key.KEY_ABNT_C1         = 0x73    --  /? on Brazilian keyboard 
key.KEY_CONVERT         = 0x79    --  (Japanese keyboard)            
key.KEY_NOCONVERT       = 0x7B    --  (Japanese keyboard)            
key.KEY_YEN             = 0x7D    --  (Japanese keyboard)            
key.KEY_ABNT_C2         = 0x7E    --  Numpad . on Brazilian keyboard 
key.KEY_NUMPADEQUALS    = 0x8D    --  = on numeric keypad (NEC PC98) 
key.KEY_PREVTRACK       = 0x90    --  Previous Track (KEY_CIRCUMFLEX on Japanese keyboard) 
key.KEY_AT              = 0x91    --                      (NEC PC98) 
key.KEY_COLON           = 0x92    --                      (NEC PC98) 
key.KEY_UNDERLINE       = 0x93    --                      (NEC PC98) 
key.KEY_KANJI           = 0x94    --  (Japanese keyboard)            
key.KEY_STOP            = 0x95    --                      (NEC PC98) 
key.KEY_AX              = 0x96    --                      (Japan AX) 
key.KEY_UNLABELED       = 0x97    --                         (J3100) 
key.KEY_NEXTTRACK       = 0x99    --  Next Track 
key.KEY_NUMPADENTER     = 0x9C    --  Enter on numeric keypad 
key.KEY_RCONTROL        = 0x9D
key.KEY_MUTE            = 0xA0    --  Mute 
key.KEY_CALCULATOR      = 0xA1    --  Calculator 
key.KEY_PLAYPAUSE       = 0xA2    --  Play / Pause 
key.KEY_MEDIASTOP       = 0xA4    --  Media Stop 
key.KEY_VOLUMEDOWN      = 0xAE    --  Volume - 
key.KEY_VOLUMEUP        = 0xB0    --  Volume + 
key.KEY_WEBHOME         = 0xB2    --  Web home 
key.KEY_NUMPADCOMMA     = 0xB3    --  , on numeric keypad (NEC PC98) 
key.KEY_DIVIDE          = 0xB5    --  / on numeric keypad 
key.KEY_SYSRQ           = 0xB7
key.KEY_RMENU           = 0xB8    --  right Alt 
key.KEY_PAUSE           = 0xC5    --  Pause 
key.KEY_HOME            = 0xC7    --  Home on arrow keypad 
key.KEY_UP              = 0xC8    --  UpArrow on arrow keypad 
key.KEY_PRIOR           = 0xC9    --  PgUp on arrow keypad 
key.KEY_LEFT            = 0xCB    --  LeftArrow on arrow keypad 
key.KEY_RIGHT           = 0xCD    --  RightArrow on arrow keypad 
key.KEY_END             = 0xCF    --  End on arrow keypad 
key.KEY_DOWN            = 0xD0    --  DownArrow on arrow keypad 
key.KEY_NEXT            = 0xD1    --  PgDn on arrow keypad 
key.KEY_INSERT          = 0xD2    --  Insert on arrow keypad 
key.KEY_DELETE          = 0xD3    --  Delete on arrow keypad 
key.KEY_LWIN            = 0xDB    --  Left Windows key 
key.KEY_RWIN            = 0xDC    --  Right Windows key 
key.KEY_APPS            = 0xDD    --  AppMenu key 
key.KEY_POWER           = 0xDE    --  System Power 
key.KEY_SLEEP           = 0xDF    --  System Sleep 
key.KEY_WAKE            = 0xE3    --  System Wake 
key.KEY_WEBSEARCH       = 0xE5    --  Web Search 
key.KEY_WEBFAVORITES    = 0xE6    --  Web Favorites 
key.KEY_WEBREFRESH      = 0xE7    --  Web Refresh 
key.KEY_WEBSTOP         = 0xE8    --  Web Stop 
key.KEY_WEBFORWARD      = 0xE9    --  Web Forward 
key.KEY_WEBBACK         = 0xEA    --  Web Back 
key.KEY_MYCOMPUTER      = 0xEB    --  My Computer 
key.KEY_MAIL            = 0xEC    --  Mail 
key.KEY_MEDIASELECT     = 0xED    --  Media Select 



cursor.IDC_APPSTARTING = 32650 --Standard arrow and small hourglass
cursor.IDC_ARROW = 32512 --Standard arrow
cursor.IDC_CROSS = 32515 --Crosshair
cursor.IDC_HAND = 32649 --Hand
cursor.IDC_HELP = 32651 --Arrow and question mark
cursor.IDC_IBEAM = 32513 --I-beam
cursor.IDC_ICON = 32641 --Obsolete for applications marked version 4.0 or later.
cursor.IDC_NO = 32648 --Slashed circle
cursor.IDC_SIZE = 32640 --Obsolete for applications marked version 4.0 or later. Use IDC_SIZEALL.
cursor.IDC_SIZEALL = 32646 --Four-pointed arrow pointing north, south, east, and west
cursor.IDC_SIZENESW = 32643 --Double-pointed arrow pointing northeast and southwest
cursor.IDC_SIZENS = 32645 --Double-pointed arrow pointing north and south
cursor.IDC_SIZENWSE = 32642 --Double-pointed arrow pointing northwest and southeast
cursor.IDC_SIZEWE = 32644 --Double-pointed arrow pointing west and east
cursor.IDC_UPARROW = 32516 --Vertical arrow
cursor.IDC_WAIT = 32514 --Hourglass

aggregate = {}

function item_base(itemtype, name, type, attrs, aggr)
	local obj = {_info={}, _childs={}, _attrs={}}

	obj._info[itemtype] = type
	obj._info["name"] = name
	if(attrs ~= nil) then
		for k,v in pairs(attrs) do
			obj._attrs[k] = v
		end
	end
	
	if(aggr ~= nil) then
		obj._info._aggregate = true
	end
	
	obj.childs = 
	function(...)
		for i,v in ipairs({...}) do
			table.insert(obj._childs, v)
		end
		return obj
	end
	obj.items = obj.childs
	obj.children = obj.childs
	
	return obj
end

function item(name, type, attrs, aggr)
	return item_base("type", name, type, attrs, aggr)
end

function ref(name, type, attrs, aggr)
	return item_base("ref", name, type, attrs, aggr)
end

function lua(name, type, attrs, aggr)
	return item_base("lua", name, type, attrs, aggr)
end

function xml(name, type, attrs, aggr)
	return item_base("xml", name, type, attrs, aggr)
end

function path(p, attrs, aggr)
	return item_base("path", p, "", attrs, aggr)
end

function connect(sender, signal, receiver, slot)
	local obj = {_info={name=""}}
	local connect_info = {}
	connect_info["sender"] = sender
	connect_info["signal"] = signal
	connect_info["receiver"] = receiver
	connect_info["slot"] = slot
	obj._info["connect"] = connect_info
	return obj
end


















