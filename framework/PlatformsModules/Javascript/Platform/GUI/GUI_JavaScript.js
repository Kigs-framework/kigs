var LibraryJsGUI  = {
    $JsGUI: {
},

	CreateCanvas : function(thewindow,isfirst)
	{
		var windowname=UTF8ToString(thewindow);
		if(isfirst==0)
		{
			var newcanvas = document.createElement('canvas');
			Module.canvas.parentElement.appendChild(newcanvas);
			newcanvas.id = windowname;
			newcanvas.style.position = "absolute";
		}
		else
		{
			var canvas = Module['canvas'];
			canvas.id = windowname;
			canvas.style.position = "absolute";
		}
	},

	GetCanvasPositionX: function(thewindow)
	{
		var windowname=UTF8ToString(thewindow);
		var canvas =document.getElementById(windowname);
		
		//var canvas = Module['canvas'];
		
		return canvas.offsetLeft;
	},
	
	GetCanvasPositionY: function(thewindow)
	{
		var windowname=UTF8ToString(thewindow);
		var canvas =document.getElementById(windowname);

		//var canvas = Module['canvas'];
		return canvas.offsetTop;
	},
	
	SetCanvasSize:function (thewindow,a_width, a_height)
	{
		var windowname=UTF8ToString(thewindow);
		var canvas =document.getElementById(windowname);

		//var canvas = Module['canvas'];
		canvas.width = a_width;
		canvas.height = a_height;
	},
	
	SetCanvasPos:function (thewindow,posx,posy)
	{
		var windowname=UTF8ToString(thewindow);
		var canvas =document.getElementById(windowname);

		//var canvas = Module['canvas'];
		
		canvas.style.left = posx +'px';
		canvas.style.top = posy +'px';
	},
	
	GetHTMLBodySizeX: function()
	{
		var width = window.innerWidth;
		return width;
	},
	
	GetHTMLBodySizeY: function()
	{
		var height = window.innerHeight;
		return height;
	},
	
};

autoAddDeps(LibraryJsGUI, '$JsGUI');
mergeInto(LibraryManager.library, LibraryJsGUI);


