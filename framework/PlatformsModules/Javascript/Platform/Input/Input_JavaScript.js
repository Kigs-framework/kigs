var LibraryJsInput  = {
    $JsInput: {
    myLastMousePosX: 0,
    myLastMousePosY: 0,
	myMouse_Events: [],
	myKeyBoard_Events: [],
},

/**********************************************************************************************************************************
														Inputs 
***********************************************************************************************************************************/
JSIsTouchable: function()
{

    var msTouchEnabled = window.navigator.msMaxTouchPoints;
    var generalTouchEnabled = "ontouchstart" in document.createElement("div");

    if (msTouchEnabled || generalTouchEnabled) {
        return true;
    }
    return false;

},

	
/**********************************************************************************************************************************
														MOUSE
***********************************************************************************************************************************/
	Get_MouseEventCount: function()
	{
		var counter = JsInput.myMouse_Events.length;
		return counter;
	},
	
	Get_MouseEvent: function(index)
	{
		return JsInput.myMouse_Events[index].EventType;
	},
	
	Get_MouseButton: function(index)
	{
		return JsInput.myMouse_Events[index].Button;
	},
	
	Get_MousePosX: function(index)
	{
		return JsInput.myMouse_Events[index].PosX;
	},
	
	Get_MousePosY: function(index)
	{
		return JsInput.myMouse_Events[index].PosY;
	},
	
	ClearMouseList:function()
	{
		JsInput.myMouse_Events = [];
	},
	
	AddListenerToMouse: function() 
	{
		function MouseDown(event) 
		{
			if(event.which >0 && event.which <4)
            {
				JsInput.myMouse_Events.push({ Button: event.which, EventType: '1', PosX: event.pageX, PosY: event.pageY});
                event.stopImmediatePropagation();
            }
		}
	 
		function MouseUp(event) 
		{
			if(event.which >0 && event.which <4)
				JsInput.myMouse_Events.push({ Button: event.which, EventType: '2', PosX: event.pageX, PosY: event.pageY});
		}
  
		function MouseMove(event) 
		{
			JsInput.myMouse_Events.push({ Button: '0', EventType: '3', PosX: event.pageX, PosY: event.pageY});
		}
		
		function MouseWheel(event)
		{

		    // cross-browser wheel delta
		    var e = window.event || event; // old IE support
		    var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail))) * 120;
		    JsInput.myMouse_Events.push({ Button: '2', EventType: '3', PosX: delta, PosY: '' });
		    if (e.preventDefault) //disable default wheel action of scrolling page
		        e.preventDefault();
		    else
		        return false
		}

		function TouchDownfnc(event)
		{
		    JsInput.myLastMousePosX = event.targetTouches[0].pageX;
		    JsInput.myLastMousePosY = event.targetTouches[0].pageY;
		  
		    JsInput.myMouse_Events.push({ Button: '1', EventType: '1', PosX: JsInput.myLastMousePosX, PosY: JsInput.myLastMousePosY });
		    event.preventDefault();
		}

		function TouchUpfnc(event)
		{
		    JsInput.myMouse_Events.push({ Button: '1', EventType: '2', PosX: JsInput.myLastMousePosX, PosY: JsInput.myLastMousePosY });
		    event.preventDefault();
		}

		function TouchMovefnc(event)
		{
		    alert("TOUCH MOVE");
		    JsInput.myLastMousePosX = event.targetTouches[0].pageX;
		    JsInput.myLastMousePosY = event.targetTouches[0].pageY;
		    JsInput.myMouse_Events.push({ Button: '0', EventType: '3', PosX: JsInput.myLastMousePosX, PosY: JsInput.myLastMousePosY });
		    event.preventDefault();
		}
		

		
		var canvas = Module['canvas'];

		function isTouchSupported()
		{
		    var msTouchEnabled = window.navigator.msMaxTouchPoints;
		    var generalTouchEnabled = "ontouchstart" in document.createElement("div");

		    if (msTouchEnabled || generalTouchEnabled) {
		        return true;
		    }
		    return false;
		}

		if (!isTouchSupported())
		{
		    var mousewheelevt = (/Firefox/i.test(navigator.userAgent)) ? "DOMMouseScroll" : "mousewheel" //FF doesn't recognize mousewheel as of FF3.x
		    canvas.addEventListener("mousedown", MouseDown, false);
            window.addEventListener("mouseup", MouseUp, false);
            canvas.addEventListener("mousemove", MouseMove, false);
            canvas.addEventListener(mousewheelevt, MouseWheel, false);
		}
		else
		{
		    //alert("platform = "+ window.navigator.platform);
		    canvas.addEventListener("touchstart", TouchDownfnc, false);
		    window.addEventListener("touchend", TouchUpfnc, false);
		    canvas.addEventListener('touchmove', function (e) {
		        JsInput.myLastMousePosX = e.targetTouches[0].pageX;
		        JsInput.myLastMousePosY = e.targetTouches[0].pageY;
		        JsInput.myMouse_Events.push({ Button: '0', EventType: '3', PosX: JsInput.myLastMousePosX, PosY: JsInput.myLastMousePosY });
		        e.preventDefault();
		    }, false);
		}
	},
	
	
	
 /**********************************************************************************************************************************
														KEYBOARD
 ***********************************************************************************************************************************/	
	
	
	ClearKeyBoardList: function()
	{
		JsInput.myKeyBoard_Events = [];
	},
	
	Get_KeyBoardEventCount: function()
	{
		var counter = JsInput.myKeyBoard_Events.length;
		return counter;
	},
	
	Get_KeyBoardEvent: function(index)
	{
		return JsInput.myKeyBoard_Events[index].EventType;
	},
	
	Get_KeyUnicode: function(index)
	{
		return JsInput.myKeyBoard_Events[index].Unicode;
	},
	
	Get_Key: function(index)
	{
		// virtual key code
		return JsInput.myKeyBoard_Events[index].Button;
	},
	
	AddListenerToKeyBoard : function()
	{
		function keyToVKey(keyValue)
		{
			
			if(keyValue.length==1)
			{
				if((keyValue.charCodeAt(0) >= 97) && (keyValue.charCodeAt(0) <= 122))
				{
					// return VK_A, VK_B...
					return 65+(keyValue.charCodeAt(0) - 97);
				}
				if((keyValue.charCodeAt(0) >= 65) && (keyValue.charCodeAt(0) <= 90))
				{
					// return VK_A, VK_B...
					return keyValue.charCodeAt(0);
				}
				if((keyValue.charCodeAt(0) >= 48) && (keyValue.charCodeAt(0) <= 57))
				{
					// return VK_0, VK_1...
					return keyValue.charCodeAt(0);
				}
				if((keyValue.charCodeAt(0) == 32))
				{
					// return VK_SPACE
					return keyValue.charCodeAt(0);
				}
			}
			
			if(keyValue.charAt(0) == 'F')
			{
				var number=parseInt(keyValue.substring(1));
				if(number > 0)
				{
					//VK_F1 -1 + number 
					return (111+number);
				}
				
			}
			if(keyValue.valueOf() == "Alt")
			{
				// VK_MENU
				return 18;
			}
			if(keyValue.valueOf() == "CapsLock")
			{
				// VK_CAPITAL
				return 20;
			}
			if(keyValue.valueOf() == "Control")
			{
				// VK_CONTROL
				return 17;
			}
			if(keyValue.valueOf() == "NumLock")
			{
				// VK_NUMLOCK
				return 144;
			}
			if(keyValue.valueOf() == "ScrollLock")
			{
				// VK_SCROLL
				return 145;
			}
			if(keyValue.valueOf() == "Shift")
			{
				// VK_SHIFT
				return 16;
			}
			if(keyValue.valueOf() == "Enter")
			{
				// VK_RETURN
				return 13;
			}
			if(keyValue.valueOf() == "Tab")
			{
				// VK_TAB
				return 9;
			}
			if(keyValue.valueOf() == "ArrowDown")
			{
				// VK_DOWN
				return 40;
			}
			if(keyValue.valueOf() == "ArrowLeft")
			{
				// VK_LEFT
				return 37;
			}
			if(keyValue.valueOf() == "ArrowRight")
			{
				// VK_RIGHT
				return 39;
			}
			if(keyValue.valueOf() == "ArrowUp")
			{
				// VK_UP
				return 38;
			}
			if(keyValue.valueOf() == "End")
			{
				// VK_END
				return 0x23;
			}
			if(keyValue.valueOf() == "Home")
			{
				// VK_HOME
				return 0x24;
			}
			if(keyValue.valueOf() == "PageDown")
			{
				// VK_NEXT
				return 0x22;
			}
			if(keyValue.valueOf() == "PageUp")
			{
				// VK_PRIOR
				return 0x21;
			}
			if(keyValue.valueOf() == "Backspace")
			{
				// VK_BACK
				return 0x08;
			}
			if(keyValue.valueOf() == "Clear")
			{
				// VK_CLEAR
				return 0x0C;
			}
			if(keyValue.valueOf() == "Delete")
			{
				// VK_DELETE
				return 0x2E;
			}
			if(keyValue.valueOf() == "Insert")
			{
				// VK_INSERT
				return 0x2D;
			}
			if(keyValue.valueOf() == "Accept")
			{
				// VK_ACCEPT
				return 0x1E;
			}
			if(keyValue.valueOf() == "ContextMenu")
			{
				// VK_APPS
				return 0x5D;
			}
			if(keyValue.valueOf() == "Escape")
			{
				// VK_ESCAPE
				return 0x1B;
			}
			if(keyValue.valueOf() == "Execute")
			{
				// VK_EXECUTE
				return 0x2B;
			}
			if(keyValue.valueOf() == "Help")
			{
				// VK_EXECUTE
				return 0x2F;
			}
			if(keyValue.valueOf() == "Pause")
			{
				// VK_PAUSE
				return 0x13;
			}
			if(keyValue.valueOf() == "Play")
			{
				// VK_PLAY
				return 0xFA;
			}
			if(keyValue.valueOf() == "Select")
			{
				// VK_SELECT
				return 0x29;
			}
			
			return 0;
		}
		
		function KeyDown(event)
		{
			var unicodeVal=0;
			if(event.key.length==1)
			{
				unicodeVal= event.key.charCodeAt(0);
			}
			JsInput.myKeyBoard_Events.push({ Button: keyToVKey(event.key), Unicode :unicodeVal, EventType: '1'});
		}
		
		function KeyUp(event)
		{
			var unicodeVal=0;
			if(event.key.length==1)
			{
				unicodeVal= event.key.charCodeAt(0);
			}
			JsInput.myKeyBoard_Events.push({ Button: keyToVKey(event.key), Unicode :unicodeVal, EventType: '0'});
		}
	
		window.addEventListener("keydown", KeyDown, false);
		window.addEventListener("keyup", KeyUp, false);
	},
};

autoAddDeps(LibraryJsInput, '$JsInput');
mergeInto(LibraryManager.library, LibraryJsInput);


