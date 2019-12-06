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
	
	Get_Key: function(index)
	{
		return JsInput.myKeyBoard_Events[index].Button;
	},
	
	AddListenerToKeyBoard : function()
	{
		function KeyDown(event)
		{
			JsInput.myKeyBoard_Events.push({ Button: event.keyCode, EventType: '1'});
		}
		
		function KeyUp(event)
		{
			JsInput.myKeyBoard_Events.push({ Button: event.keyCode, EventType: '0'});
		}
	
		window.addEventListener("keydown", KeyDown, false);
		window.addEventListener("keyup", KeyUp, false);
	},
};

autoAddDeps(LibraryJsInput, '$JsInput');
mergeInto(LibraryManager.library, LibraryJsInput);


