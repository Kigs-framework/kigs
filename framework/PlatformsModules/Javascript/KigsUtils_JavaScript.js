var LibraryJsInput  = {
    $JsInput: {
    myLastMousePosX: 0,
    myLastMousePosY: 0,
	myMouse_Events: [],
	myKeyBoard_Events: [],
	myCanvasList: [],
    myVideoList: [],
	myCurrentCanvas: null,
	myInitialContext2D: null,
	myInitialCanvas: null,
	myCurrentTextInput: null,
    myBlendCanvas: null,
	myPixelData: [],
	myRatioX: 1,
	myRatioY: 1,
	myR: 255,
	myG: 255,
	myG: 255,
	myA: 255,
	myCanvasListSize: 0,
	myTextInputs: [],
	myTextIndex: 0,
    videoposx: 600,
    videoposy: 150,
    myOnlineFile: null,
    myXmlhttp: null,
    myYouTubeVideo: null,
	myAudioPlayer: null,
	myAudioSource1: null,
	myAudioSource2: null,
	myAudioShouldLoop: false,
	myAudioVolume: 100,
	myAudioState: 0,
    myDisableAllInputs: false,
	myAnimationList: [],
	myAnimationIndex: 0,
	MyVideoButtonPosX: 0,
	MyVideoButtonPosY: 0,
	MyVideoButtonSizeX: 0,
	MyVideoButtonSizeY: 0,
    constructBuffer: function(buffer, result, out_size){
         var mallocbuffer=_malloc(result.length*2+2);

         HEAP32[out_size>>2]=result.length + 1;
         HEAP32[buffer>>2]=mallocbuffer;

         var bufferpos=mallocbuffer>>1;

         for(var i = 0; i < result.length; i++){
              HEAPU16[bufferpos++] = result.charCodeAt(i);
         }
         HEAPU16[bufferpos] = '\0';
   },
   Pointer_stringify16: function(ptr) 
	{
		var ret = "";
		var i = 0;
		var currentPos=ptr>>1;
		var t;
		while (1) 
		{
			t = HEAPU16[currentPos++];
			if (t == 0) break;
			ret += String.fromCharCode(t);
			i += 1;
		}
		return ret;
	},
    copyBuffer: function(buffer, result, out_size)
    {
         var mallocbuffer=_malloc((result.length*2)+2);

         HEAP32[out_size>>2]=(result.length*2)+2;
         HEAP32[buffer>>2]=mallocbuffer;

         var bufferpos=mallocbuffer>>1;

         for(var i = 0; i < result.length; i++)
         {
              HEAPU16[bufferpos++] = result[i];
         }
         HEAPU16[bufferpos] = '\0';
   },
   loadjscssfile: function(filename, filetype)
   {
		if (filetype==".js")
		{ 
			//if filename is a external JavaScript file
			var fileref=document.createElement('script')
			//fileref.setAttribute("type","text/javascript")
			fileref.setAttribute("src", filename+filetype)
			console.log(fileref);
		}
		else if (filetype==".css")
		{ 
			//if filename is an external CSS file
			var fileref=document.createElement("link")
			fileref.setAttribute("rel", "stylesheet")
			fileref.setAttribute("type", "text/css")
			fileref.setAttribute("href", filename+filetype)
		}
		if (typeof fileref!="undefined")
		{
			document.getElementsByTagName("head")[0].appendChild(fileref)
		}
	},
},

/**********************************************************************************************************************************
														YOUTUBE PLAYER
***********************************************************************************************************************************/
JScreateYouTubeVideoPlayer: function(idsong, _width, _height, _posX, _posY, _cross)
{
    var theWidth = _width;
    var theHeight = _height;
    var thePosX = _posX;
    var thePosY = _posY;
	
    JsInput.videoposx = thePosX;
    JsInput.videoposy = thePosY;
	
    var arg = [];
	arg[0] = { id: UTF8ToString(idsong),
			width: theWidth,
			height: theHeight,
			};

	CreateYouTubeVideoPlayer(arg, thePosX, thePosY,_cross);
},

JSdestroyYouTubeVideoPlayer: function()
{
    DestroyYouTubeVideoPlayer();
},

JSGetYouTubeVideoState: function()
{
    return GetYouTubeVideoState();
},

JShideYouTubeVideoPlayer: function()
{
    HideYouTubeVideoPlayer();
},

JSshowYouTubeVideoPlayer: function()
{
	ShowYouTubeVideoPlayer();
},

/**********************************************************************************************************************************
														BROWSER UTILS
***********************************************************************************************************************************/

  JSGetLanguage: function(_var)
  {
	var thevar= window.navigator.language || window.navigator.browserLanguage;
	HEAP8[_var] = thevar.charCodeAt(0);
	HEAP8[_var+1] = thevar.charCodeAt(1);
  },
 
 /**********************************************************************************************************************************
														BLUE PAID 
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

JSSetVideoButtonValues: function (_px, _py, _sx, _sy)
{
    JsInput.MyVideoButtonPosX = _px;
    JsInput.MyVideoButtonPosY = _py;
    JsInput.MyVideoButtonSizeX = _sx;
    JsInput.MyVideoButtonSizeY = _sy;
},


JSCreateBluePaidDiv: function ()
{
    var L_BPdiv = document.getElementById('BluePaid');
    if (L_BPdiv == null) 
    {
        var canvas = Module['canvas'];
        var fatherdiv = canvas.parentElement;
	    var offsetx = canvas.offsetLeft;
	    var offsety = canvas.offsetTop;
    
        L_BPdiv = document.createElement('div');
        L_BPdiv.setAttribute("id", "BluePaid");
        L_BPdiv.style.position = "absolute";
	    L_BPdiv.style.left = (offsetx + JsInput.myInitialCanvas.width * 0.5) + 'px';
	    L_BPdiv.style.top = (offsety + JsInput.myInitialCanvas.height * 0.5) + 'px';
        fatherdiv.appendChild(L_BPdiv);
    }
},

JSHideBluePaidDiv: function()
{
    var L_BPdiv = document.getElementById('BluePaid');
    if(L_BPdiv != null)
    {
        L_BPdiv.style.display = "none";
    }
},

JSShowBluePaidDiv: function()
{
    var L_BPdiv = document.getElementById('BluePaid');
    if(L_BPdiv != null)
    {
        L_BPdiv.style.display = "inline";
    }
},

JSExecuteBluePaidBuffer: function(_HTMLbuffer)
{
    var L_BPdiv = document.getElementById('BluePaid');
    if(L_BPdiv != null)
    {
        var L_HTMLbuffer  =  JsInput.Pointer_stringify16(_HTMLbuffer);
        L_BPdiv.innerHTML += L_HTMLbuffer;
        document.getElementById('bluepaidfrm').submit();
    }
},

/**********************************************************************************************************************************
														CREATE CONTEXT
***********************************************************************************************************************************/
    JSGotoIndexHtml: function(){
        window.location.href = window.location.origin;
    },

	KigsCreateContext: function()
	{
        function LeaveGame(event)
        {           
            //BD_Logout();       
        }

 
	    var wasPressed = false;
	    function fkey(e)
        {
		    e = e || window.event;
		    if(wasPressed) 
                return; 

		    if (e.keyCode == 116)
            {	
			    window.location.href = window.location.origin;
			    wasPressed = true;
			
			    e.returnValue = false;
                if (e.preventDefault) 
                {
                    e.preventDefault();
                }
				
			    //BD_Logout();
		    }
	    }

		function BrowerResized(event) 
		{
  			var canvas = Module['canvas'];
    		 
  			var fatherdiv=canvas.parentElement;
  			var offsetx=canvas.offsetLeft;
  			var offsety=canvas.offsetTop;
  			

  			for(var i =0; i < JsInput.myTextIndex; i++)
  			{
				if(JsInput.myTextInputs[i] != null)
				{
					var element  = JsInput.myTextInputs[i].Element;
					element.style.left = offsetx+JsInput.myTextInputs[i].PosX +'px';
					element.style.top = offsety+JsInput.myTextInputs[i].PosY +'px';
				}
  			}

			var playerdiv = document.getElementById('player');
            if(playerdiv != null)
            {
				var posx = offsetx + JsInput.videoposx + 'px';
				var posy = offsety + JsInput.videoposy + 'px';			
                playerdiv.style.left = posx;
                playerdiv.style.top = posy;				
            }

            var displaycanvas = document.getElementById("theVideoDisplay");
            if(displaycanvas != null)
            {
                var posx = (offsetx + JsInput.myVideoPosX) + 'px';
				var posy = (offsety + JsInput.myVideoPosY) + 'px';			
                displaycanvas.style.left = posx;
                displaycanvas.style.top = posy;	
            }

            var L_element = document.getElementById("BtPlayVideo");
            if (L_element != null)
            {
                L_element.style.left = canvas.offsetLeft + JsInput.MyVideoButtonPosX + 'px';
                L_element.style.top = canvas.offsetTop + JsInput.MyVideoButtonPosY + 'px';
            }
		}
		
		Browser.createContext(Module['canvas'], false, true);
		
		//add listener to window to call method when browser is resized
		window.addEventListener("resize", BrowerResized, false);
		window.addEventListener("haschange", LeaveGame, false);
		window.addEventListener("beforeunload", LeaveGame, false);
        window.addEventListener("keydown", fkey, false);

		//LoadYouTube();

        JsInput.myBlendCanvas = document.createElement('canvas');
        JsInput.myBlendCanvas.width = 1;
		JsInput.myBlendCanvas.height = 1;
	},
    KigsDestroyContext: function()
    {
        var canvas = Module['canvas'];
    		 
  		var fatherdiv=canvas.parentElement;
  		var offsetx=canvas.offsetLeft;
  		var offsety=canvas.offsetTop;
  			

  		for(var i =0; i < JsInput.myTextIndex; i++)
  		{
			if(JsInput.myTextInputs[i] != null)
			{
				JsInput.myTextInputs[i].Element = null;
			}
  		}

		var playerdiv = document.getElementById('player');
        if(playerdiv != null)
        {
			playerdiv.parentNode.removeChild(playerdiv);	
        }

        var displaycanvas = document.getElementById("theVideoDisplay");
        if(displaycanvas != null)
        {
            displaycanvas.parentNode.removeChild(displaycanvas);	
        }
        JsInput.myInitialCanvas.parentElement.removeChild(JsInput.myInitialCanvas);
        JsInput.myInitialCanvas = null;
    },

/**********************************************************************************************************************************
														DRAW
***********************************************************************************************************************************/
	JSGenTexture: function(width, height)
	{
		if(JsInput.myInitialCanvas == null && JsInput.myContext2D == null)
		{
			JsInput.myInitialCanvas = Module['canvas'];
			JsInput.myInitialContext2D = JsInput.myInitialCanvas.getContext('2d');
		}
	
		//Create new canvas
		var imageCanvas = document.createElement('canvas');
		
		if(width == 0 && height == 0)
		{
			imageCanvas.width = JsInput.myInitialCanvas.width;
			imageCanvas.height = JsInput.myInitialCanvas.height;
		}
		else
		{
			imageCanvas.width = width;
			imageCanvas.height = height;
		}

		//Add to list
		var stored = false;
		var index = 0;
		//Recherche un trou dans le tableau
		for(i=0;i<JsInput.myCanvasListSize; i++)
		{
			if(JsInput.myCanvasList[i] == null)
			{
                imageCanvas.id = ('theCanvas ' + i);
				JsInput.myCanvasList[i] = imageCanvas;
				stored = true;
				index = i;
				break;
			}
		}
		
		
		//Le tableau ne comportait pas de trou
		if(!stored)
		{
			index = JsInput.myCanvasListSize;
            imageCanvas.id = ('theCanvas ' + index);
			JsInput.myCanvasList[JsInput.myCanvasListSize] = imageCanvas;
			JsInput.myCanvasListSize++;
		}

		//Return index
		return index;
	},
	
	JSSetTextureSize: function(ID, width, height)
	{
		if(width == 0 || height == 0)
			return;
		
		JsInput.myCanvasList[ID].width = width;
		JsInput.myCanvasList[ID].height = height;
	},
	
    // width and height are output params
    JSSetTextureFromFile: function(filename,width, height)
	{
		//filename = FS.standardizePath(UTF8ToString(filename));
        filename =  PATH_FS.resolve(UTF8ToString(filename));
        /*if (filename[0] == '/') 
		{
          // Convert the path to relative
          filename = filename.substr(1);
        }*/
        var raw = Module["preloadedImages"][filename];
        if (!raw) 
		{
          if (raw === null) 
			Module.printErr('Trying to reuse preloaded image, but freePreloadedMediaOnUse is set!');
			
          Runtime.warnOnce('Cannot find preloaded image ' + filename);
          return -1;
        }
        if (Module['freePreloadedMediaOnUse']) 
		{
            Module["preloadedImages"][filename] = null;
        }

        var ctx = JsInput.myCurrentCanvas.getContext('2d');
        JsInput.myCurrentCanvas.width = raw.width;
		JsInput.myCurrentCanvas.height = raw.height;
		
        HEAP32[width>>2]=raw.width;
        HEAP32[height>>2]=raw.height;
        
  		ctx.drawImage(raw, 0, 0, raw.width, raw.height, 0, 0, raw.width, raw.height);
	},
	
	JSSetTextureFromURL: function(filename,width, height)
	{
		filename = UTF8ToString(filename);
		
		if (filename[0] == '@') 
		{
          // Convert the path to relative
          filename = filename.substr(1);
        }
		else
		{
			alert("URL DOES NOT HAVE @ IDENTIFIER !");
		}

        var ctx = JsInput.myCurrentCanvas.getContext('2d');
		 
		var imageObj = new Image();
		var mycanvas = JsInput.myCurrentCanvas;
		imageObj.onload = function() 
		{
			mycanvas.width = imageObj.width;
			mycanvas.height = imageObj.height;

			ctx.drawImage(imageObj, 0, 0, imageObj.width, imageObj.height, 0, 0, imageObj.width, imageObj.height);
		};
		 imageObj.src = filename;
	},

    JSGetFileFromURL: function(filename)
	{
		filename = UTF8ToString(filename);
		
		if (filename[0] == '@') 
		{
          // Convert the path to relative
          filename = filename.substr(1);
        }
		else
		{
			alert("URL DOES NOT HAVE @ IDENTIFIER !");
		}

        if (window.XMLHttpRequest)
        {
            // code for IE7+, Firefox, Chrome, Opera, Safari
            JsInput.myXmlhttp=new XMLHttpRequest();
        }
        else
        {
            // code for IE6, IE5
            JsInput.myXmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
        }

        JsInput.myXmlhttp.open("GET",filename,true);
		JsInput.myXmlhttp.responseType = 'arraybuffer';
		
        JsInput.myXmlhttp.onreadystatechange=function()
        {
            if (JsInput.myXmlhttp.readyState==4 && JsInput.myXmlhttp.status==200)
            {
				JsInput.myOnlineFile = new Uint16Array(JsInput.myXmlhttp.response);
                return JsInput.myOnlineFile;
            }
        }
        
        JsInput.myXmlhttp.send();
	},

    JSCheckOnlineFile: function(buffer, size)
    {
        if(JsInput.myOnlineFile != null)
        {
            JsInput.copyBuffer(buffer,JsInput.myOnlineFile, size);
            JsInput.myOnlineFile = null;
            return true;
        }
        return false;
    },
	
	JSDeleteTexture: function(index)
	{
		JsInput.myCanvasList[index] = null;
	},
	
	JSBindTexture: function(index)
	{
		JsInput.myCurrentCanvas = JsInput.myCanvasList[index];
	},
	
	JSCanvasRotation: function(PosX, PosY, AnchorX, AnchorY, width, height, Rotation)
	{
		if(Rotation)
  		{
			var angleInRadians = 0;

			if(Rotation == 1)
			{
				angleInRadians = -(Math.PI/2);
			}
			else if(Rotation == 2)
			{
				angleInRadians = -(Math.PI);
			}
			else if(Rotation == 3)
			{

				angleInRadians = -(Math.PI+(Math.PI/2));
			}	
		
            //canvas is haven't the good size
            if(JsInput.myCurrentCanvas.width != width)
                JsInput.myCurrentCanvas.width = width;
            if(JsInput.myCurrentCanvas.height != height)
                JsInput.myCurrentCanvas.height = height;

			JsInput.myInitialContext2D.save();
			JsInput.myInitialContext2D.translate(PosX, PosY);
			JsInput.myInitialContext2D.rotate(angleInRadians);
			JsInput.myInitialContext2D.drawImage(JsInput.myCurrentCanvas, -AnchorX , -AnchorY , width, height);
			JsInput.myInitialContext2D.restore();				
		}
	},
	
	
	JSSetPixelsData: function(colors, width, height)
	{
		JsInput.myCurrentCanvas.width = width;
  		JsInput.myCurrentCanvas.height = height;
  		var ctx = JsInput.myCurrentCanvas.getContext('2d');
		
  		var imageData = ctx.getImageData(0, 0, JsInput.myCurrentCanvas.width, JsInput.myCurrentCanvas.height);
          		
        var size = width*height;
  		var index = 0;

		for (i = 0; i < size; i++) 
		{
			//var index = i *4;
			imageData.data[index++] = HEAPU8[colors++];
			imageData.data[index++] = HEAPU8[colors++];
			imageData.data[index++] = HEAPU8[colors++];
			imageData.data[index++] = HEAPU8[colors++];
		}
			
        ctx.putImageData(imageData,0,0);
	},
	
	JSGetCanvasSize: function (width, height)
	{
		if(JsInput.myCurrentCanvas.width > 1 || JsInput.myCurrentCanvas.height >1)
		{
			HEAP32[width>>2]=JsInput.myCurrentCanvas.width ;
			HEAP32[height>>2]=JsInput.myCurrentCanvas.height;
		}
	},
	 
	JSGetPixelColor: function(Data)
	{
		var ctx = JsInput.myCurrentCanvas.getContext('2d');
	
		var p = ctx.getImageData(0, 0, JsInput.myCurrentCanvas.width, JsInput.myCurrentCanvas.height).data;
		var index = 0;
		var size = JsInput.myCurrentCanvas.width * JsInput.myCurrentCanvas.height;
		for(i=0;i <  size; i++)
		{
			HEAPU8[Data++] = p[index++];
			HEAPU8[Data++] = p[index++];
			HEAPU8[Data++] = p[index++];
			HEAPU8[Data++] = p[index++];
		}
	},
	
    JSIsAlphaAtPoint:function(a_Ratiox, a_Ratioy, a_Threshold)
    {
        var ctx = JsInput.myCurrentCanvas.getContext('2d');
        var p = ctx.getImageData(0, 0, JsInput.myCurrentCanvas.width, JsInput.myCurrentCanvas.height).data;
        var L_Px = Math.floor(a_Ratiox*JsInput.myCurrentCanvas.width);
        var L_Py = Math.floor(a_Ratioy*JsInput.myCurrentCanvas.height);
        var index = ((JsInput.myCurrentCanvas.width * L_Py)+L_Px)*4;
        return (p[index+3] < a_Threshold);    
    },

	JSScaleInitialCanvas: function(ratioX,ratioY)
	{
		JsInput.myInitialContext2D.scale(ratioX/JsInput.myRatioX,ratioY/JsInput.myRatioY);
		JsInput.myRatioX = ratioX;
		JsInput.myRatioY = ratioY;
	},
	
	JSClearCanvas: function()
	{
		if(JsInput.myInitialContext2D != null)
			JsInput.myInitialContext2D.clearRect ( 0 , 0 , JsInput.myInitialCanvas.width , JsInput.myInitialCanvas.height );
	},
	
	JSDrawTexture2: function(PosX, PosY, T00,T01,T10,T11, Alpha , R , G , B)
	{
        var HalfW = (width*0.5);
        var HalfH = (height*0.5);

        JsInput.myInitialContext2D.save();
		JsInput.myInitialContext2D.translate(PosX+HalfW, PosY+HalfH);
        JsInput.myInitialContext2D.transform(T00,T01,T10,T11,0,0);

        if(Alpha != 1.0)
        {
   
            if(R != -1 || G != -1 || B != -1)
            {
                //canvas temporaire
                JsInput.myBlendCanvas.width = JsInput.myCurrentCanvas.width;
                JsInput.myBlendCanvas.height = JsInput.myCurrentCanvas.height;
                var tmpctx = JsInput.myBlendCanvas.getContext('2d');
                tmpctx.drawImage(JsInput.myCurrentCanvas, 0, 0, JsInput.myCurrentCanvas.width, JsInput.myCurrentCanvas.height);
                tmpctx.globalCompositeOperation = 'source-atop';
                tmpctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + Alpha + ")";
                tmpctx.fillRect(0, 0,JsInput.myCurrentCanvas.width, JsInput.myCurrentCanvas.height);
				
                JsInput.myInitialContext2D.globalCompositeOperation = 'source-over';
	            JsInput.myInitialContext2D.drawImage(JsInput.myBlendCanvas, -HalfW, -HalfH);
            }
            else
            {
                JsInput.myInitialContext2D.globalAlpha = Alpha;
                JsInput.myInitialContext2D.drawImage(JsInput.myCurrentCanvas,  -HalfW, -HalfH);
            }            
        }
        else
        {
            JsInput.myInitialContext2D.drawImage(JsInput.myCurrentCanvas,  -HalfW, -HalfH);
        }
        JsInput.myInitialContext2D.restore();
	},
	
	JSDrawTexture4: function(PosX, PosY, width, height, T00,T01,T10,T11, Alpha , R , G , B)
	{
        var HalfW = (width*0.5);
        var HalfH = (height*0.5);

        JsInput.myInitialContext2D.save();
		JsInput.myInitialContext2D.translate(PosX+HalfW, PosY+HalfH);
        JsInput.myInitialContext2D.transform(T00,T01,T10,T11,0,0);
        if(Alpha != 1.0)
        {              
            if(R != -1 || G != -1 || B != -1)
            {
                //canvas temporaire
                JsInput.myBlendCanvas.width = JsInput.myCurrentCanvas.width;
                JsInput.myBlendCanvas.height = JsInput.myCurrentCanvas.height;
                var tmpctx = JsInput.myBlendCanvas.getContext('2d');
                tmpctx.drawImage(JsInput.myCurrentCanvas, 0, 0, JsInput.myCurrentCanvas.width, JsInput.myCurrentCanvas.height);
                tmpctx.globalCompositeOperation = 'source-atop';
                tmpctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + Alpha + ")";
                tmpctx.fillRect(0, 0, JsInput.myCurrentCanvas.width, JsInput.myCurrentCanvas.height);
				
                JsInput.myInitialContext2D.globalCompositeOperation = 'source-over';
	            JsInput.myInitialContext2D.drawImage(JsInput.myBlendCanvas, -HalfW, -HalfH, width, height);
            }
            else
            {
                JsInput.myInitialContext2D.globalAlpha = Alpha;
                JsInput.myInitialContext2D.drawImage(JsInput.myCurrentCanvas, -HalfW, -HalfH, width, height);
            }
        }
        else
        {
            JsInput.myInitialContext2D.drawImage(JsInput.myCurrentCanvas, -HalfW, -HalfH, width, height);
        }
        JsInput.myInitialContext2D.restore();
	},
	
	JSDrawTexture8: function(PosX, PosY, imageWidth, imageHeight, offsetX, offsetY, finalWidth, finalHeight, T00,T01,T10,T11, Alpha , R , G , B)
	{

        var HalfW = (imageWidth*0.5);
        var HalfH = (imageHeight*0.5);

        JsInput.myInitialContext2D.save();
		JsInput.myInitialContext2D.translate(PosX+HalfW, PosY+HalfH);
        JsInput.myInitialContext2D.transform(T00,T01,T10,T11,0,0);
        if(Alpha != 1.0)
        {              
            if(R != -1 || G != -1 || B != -1)
            {
                //canvas temporaire
                JsInput.myBlendCanvas.width = JsInput.myCurrentCanvas.width;
                JsInput.myBlendCanvas.height = JsInput.myCurrentCanvas.height;
                var tmpctx = JsInput.myBlendCanvas.getContext('2d');
                tmpctx.drawImage(JsInput.myCurrentCanvas, 0, 0, JsInput.myCurrentCanvas.width, JsInput.myCurrentCanvas.height);
                tmpctx.globalCompositeOperation = 'source-atop';
                tmpctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + Alpha + ")";
                tmpctx.fillRect(0, 0, JsInput.myCurrentCanvas.width, JsInput.myCurrentCanvas.height);
				
                JsInput.myInitialContext2D.globalCompositeOperation = 'source-over';
	            JsInput.myInitialContext2D.drawImage(JsInput.myBlendCanvas, offsetX, offsetY, imageWidth, imageHeight, -HalfW, -HalfH, finalWidth, finalHeight);
            }
            else
            {
                JsInput.myInitialContext2D.globalAlpha = Alpha;
                JsInput.myInitialContext2D.drawImage(JsInput.myCurrentCanvas, offsetX, offsetY, imageWidth, imageHeight, -HalfW, -HalfH, finalWidth, finalHeight);
            }
        }
        else
        {
            JsInput.myInitialContext2D.drawImage(JsInput.myCurrentCanvas, offsetX, offsetY, imageWidth, imageHeight, -HalfW, -HalfH, finalWidth, finalHeight);
        }
        JsInput.myInitialContext2D.restore();
	},
	
	JSDrawRect: function(PosX, PosY, width, height)
	{
		JsInput.myInitialContext2D.fillStyle = "rgba(" + JsInput.myR + "," + JsInput.myG  + "," + JsInput.myB + "," + JsInput.myA +")";
		JsInput.myInitialContext2D.fillRect(PosX, PosY, width, height);
	},
	
	JSSetBackgroundColor:function(R,G,B,A)
	{
		JsInput.myR = R*255;
		JsInput.myG = G*255;
		JsInput.myB = B*255;
		JsInput.myA = A;
	},
	
/**********************************************************************************************************************************
														TEXT
***********************************************************************************************************************************/
	
			
	JSSetTextInfo: function(Text, Font, FontSize, R,G,B,A,_bold, _stroke, st_R,st_G,st_B,st_A )
	{
    
        var myText  = UTF8ToString(Text);
        if(myText.length == 0)
            return;

		var myFont = UTF8ToString(Font);
		var ctx = JsInput.myCurrentCanvas.getContext('2d');
       
        var text = [];
		function CutString(str)
		{
			var currentText = '';
			var futurText = '';
			var newligned = false;

			for(i = 0; i < str.length; i++)
			{
				if(newligned == false)
				{
					if(str[i] == '\n')
					{
                        if(currentText.length >0)
						    text.push(currentText);
						newligned = true
					}
					else
						currentText += str[i];
				}
				else
					futurText += str[i];
			}
			if(futurText)
			{
				CutString(futurText);
			}
            else
			{
                if(currentText.length >0)
				    text.push(currentText);
			}
		}

		CutString(myText);
		myFont = myFont.split(".")[0];
		ctx.font = FontSize + "px " + myFont;
		JsInput.myCurrentCanvas.width = ctx.measureText(myText).width;
		var ComputedHeight = FontSize * 1.2;
		JsInput.myCurrentCanvas.height =  (ComputedHeight * text.length) + ((FontSize * 0.5) * (text.length -1));


		ctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + A +")";
        
        if(_stroke)
        {
            ctx.lineWidth = (FontSize / 20);
            //ctx.font = (FontSize+5) + "px " + myFont;
            ctx.strokeStyle = "rgba(" + st_R + "," + st_G  + "," + st_B + "," + st_A +")";
        }

		ctx.font = FontSize + "px " + myFont;
		ctx.textBaseline = 'middle';
    	ctx.textAlign = 'center';
        if(text.length == 1)
        {
  		    ctx.fillText(myText, JsInput.myCurrentCanvas.width*0.5,JsInput.myCurrentCanvas.height*0.5);
            if(_stroke)
            {
                //ctx.font = (FontSize+5) + "px " + myFont;
                ctx.strokeText(myText, JsInput.myCurrentCanvas.width*0.5,JsInput.myCurrentCanvas.height*0.5);
            }
        }
        else
		{
			var textX, textY;
			textX = JsInput.myCurrentCanvas.width/2;

			for(var i = 0; i < text.length; i++) 
			{
				textY = (i+1)*ComputedHeight;
                //ctx.font = FontSize + "px " + myFont;
				ctx.fillText(text[i], textX,  textY);
                if(_stroke)
                {
                    //ctx.font = (FontSize+5) + "px " + myFont;
                    ctx.strokeText(text[i], textX,  textY);
                }
			}
		}
	},
		
	JSSetTextInfoUTF16: function(Text, Font, FontSize, R,G,B,A,_bold, _stroke, st_R,st_G,st_B,st_A)
	{
        var myText  =  JsInput.Pointer_stringify16(Text);
        if(myText.length == 0)
            return;
  		
		var myFont = UTF8ToString(Font);
		myFont = myFont.split(".")[0];
		var ctx = JsInput.myCurrentCanvas.getContext('2d');
		

        var text = [];
		function CutString(str)
		{
			var currentText = '';
			var futurText = '';
			var newligned = false;

			for(i = 0; i < str.length; i++)
			{
				if(newligned == false)
				{
					if(str[i] == '\n')
					{
                        if(currentText.length >0)
						    text.push(currentText);
						newligned = true
					}
					else
						currentText += str[i];
				}
				else
					futurText += str[i];
			}
			if(futurText)
			{
				CutString(futurText);
			}
            else
			{
                if(currentText.length >0)
				    text.push(currentText);
			}
		}

		CutString(myText);

		ctx.font = FontSize + "px " + myFont;
		JsInput.myCurrentCanvas.width = ctx.measureText(myText).width;
        var ComputedHeight = FontSize * 1.2;
		JsInput.myCurrentCanvas.height =   (ComputedHeight * text.length) + ((FontSize * 0.5) * (text.length -1));

		ctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + A +")";
        if(_stroke)
        {
            ctx.lineWidth = (FontSize / 20);
            //ctx.font = (FontSize+5) + "px " + myFont;
            ctx.strokeStyle = "rgba(" + st_R + "," + st_G  + "," + st_B + "," + st_A +")";
        }

		ctx.font = FontSize + "px " + myFont;
		ctx.textBaseline = 'middle';
    	ctx.textAlign = 'center';
        if(text.length == 1)
        {
  		    ctx.fillText(myText, JsInput.myCurrentCanvas.width*0.5,JsInput.myCurrentCanvas.height*0.5);
            if(_stroke)
            {
                //ctx.font = (FontSize+5) + "px " + myFont;
                ctx.strokeText(myText, JsInput.myCurrentCanvas.width*0.5,JsInput.myCurrentCanvas.height*0.5);
            }
        }
        else
		{
			var textX, textY;
			textX = JsInput.myCurrentCanvas.width/2;

			for(var i = 0; i < text.length; i++) 
			{
				textY = (i+1)*ComputedHeight;

                //ctx.font = FontSize + "px " + myFont;
				ctx.fillText(text[i], textX,  textY);
                if(_stroke)
                {
                    //ctx.font = (FontSize+5) + "px " + myFont;
                    ctx.strokeText(text[i], textX,  textY);
                }
			}
		}
	},

	JSTextGetSize: function(width, height)
	{
		HEAP32[width>>2]=JsInput.myCurrentCanvas.width;
		HEAP32[height>>2]=JsInput.myCurrentCanvas.height;
	},

    JSCreateTextInput: function(posx,posy,width, height, Text, R, G, B, A, FontSize, FontName, maxSize)
    {	
        var canvas = Module['canvas'];
		 
        var fatherdiv=canvas.parentElement;
        var offsetx=canvas.offsetLeft;
        var offsety=canvas.offsetTop;
		
		
        var input = document.createElement("input");
		
		
		var stored = false;
		var index = 0;
		//Recherche un trou dans le tableau
		for(i=0;i<JsInput.myTextIndex; i++)
		{
			if(JsInput.myTextInputs[i] == null)
			{
				input.id = ("textInput"+i);
				JsInput.myTextInputs[i] = { Element:input, PosX:posx, posY:posy};
				stored = true;
				index = i;
				break;
			}
		}
		if(!stored)
		{
			input.id = ("textInput"+JsInput.myTextIndex);
			JsInput.myTextInputs.push({ Element:input, PosX:posx, posY:posy});
			index = JsInput.myTextIndex;
			JsInput.myTextIndex++;
		}
		
		var myFont = UTF8ToString(FontName);
		myFont = myFont.split(".")[0];
        input.onfocus = function()
		{
			input.value  = "";
		};
		input.value  = UTF8ToString(Text);
        input.type = "text";
		input.style.color = "rgba(" + R + "," + G  + "," + B + "," + A +")";
        input.className = "css-class-name"; // set the CSS class
        input.style.position="absolute";
        fatherdiv.appendChild(input); // put it into the DOM
        input.style.left=(offsetx + posx)+'px';
        input.style.top=(offsety + posy)+'px';
        input.style.width = width +'px';
        input.style.height = height +'px';
        input.style.backgroundColor = "rgba(255,255,255,0.0)";
		input.style.textAlign = "center";
		input.style.font = FontSize + "px " + myFont;
		input.style.border = "none";
		if(maxSize>0)
			input.setAttribute("maxlength",maxSize);
		
		function MouseOut(event) 
		{
			for(i=0;i<JsInput.myTextIndex; i++)
			{
                if(JsInput.myTextInputs[i] != null)
                {
				    if(JsInput.myTextInputs[i].Element == event.target)
				    {
					    JsInput.myCurrentTextInput = JsInput.myTextInputs[i].Element;
				    }
                }
			}
		}
		
		input.addEventListener("blur", MouseOut, false);

        if(JsInput.myDisableAllInputs == true)
        {
			input.disabled = true;
            input.hidden = true;
        }
        
		return index;
    },

	JSCreateTextInputUTF16: function(posx,posy,width, height, Text, R, G, B, A, FontSize, FontName, maxSize)
    {
		var theText  = JsInput.Pointer_stringify16(Text);
        var canvas = Module['canvas'];
		 
        var fatherdiv=canvas.parentElement;
        var offsetx=canvas.offsetLeft;
        var offsety=canvas.offsetTop;
		
		
        var input = document.createElement("input");
		
		
		var stored = false;
		var index = 0;
		//Recherche un trou dans le tableau
		for(i=0;i<JsInput.myTextIndex; i++)
		{
			if(JsInput.myTextInputs[i] == null)
			{
				input.id = ("textInput"+i);
				JsInput.myTextInputs[i] = { Element:input, PosX:posx, posY:posy};
				stored = true;
				index = i;
				break;
			}
		}
		if(!stored)
		{
			input.id = ("textInput"+JsInput.myTextIndex);
			JsInput.myTextInputs.push({ Element:input, PosX:posx, posY:posy});
			index = JsInput.myTextIndex;
			JsInput.myTextIndex++;
		}
		
		var myFont = UTF8ToString(FontName);
		myFont = myFont.split(".")[0];
        input.onfocus = function()
		{
			input.value  = "";
		};
		input.value  = theText;
        input.type = "text";
		input.style.color = "rgba(" + R + "," + G  + "," + B + "," + A +")";
        input.className = "css-class-name"; // set the CSS class
        input.style.position="absolute";
        fatherdiv.appendChild(input); // put it into the DOM
        input.style.left=(offsetx + posx)+'px';
        input.style.top=(offsety + posy)+'px';
        input.style.width = width +'px';
        input.style.height = height +'px';
        input.style.backgroundColor = "rgba(255,255,255,0.0)";
		input.style.textAlign = "center";
		input.style.font = FontSize + "px " + myFont;
		input.style.border = "none";
		if(maxSize>0)
			input.setAttribute("maxlength",maxSize);
		
		function MouseOut(event) 
		{
			//JsInput.mySelectedTextInput =
			for(i=0;i<JsInput.myTextIndex; i++)
			{
                if(JsInput.myTextInputs[i] != null)
                {
				    if(JsInput.myTextInputs[i].Element == event.target)
				    {
					    JsInput.myCurrentTextInput = JsInput.myTextInputs[i].Element;
				    }
                }
			}
		}
		
		input.addEventListener("blur", MouseOut, false);
		
        if(JsInput.myDisableAllInputs == true)
        {
			input.disabled = true;
            input.hidden = true;
        }

		return index;
    },
	
	JSCreateTextArea: function(posx,posy,col, row, Text, R, G, B, A, FontSize, FontName, maxSize)
    {	
        var canvas = Module['canvas'];
		 
        var fatherdiv=canvas.parentElement;
        var offsetx=canvas.offsetLeft;
        var offsety=canvas.offsetTop;
		
		
        var input = document.createElement("textarea");
		input.rows = row;
		input.cols = col;
		
		var stored = false;
		var index = 0;
		//Recherche un trou dans le tableau
		for(i=0;i<JsInput.myTextIndex; i++)
		{
			if(JsInput.myTextInputs[i] == null)
			{
				input.id = ("textInput"+i);
				JsInput.myTextInputs[i] = { Element:input, PosX:posx, posY:posy};
				stored = true;
				index = i;
				break;
			}
		}
		if(!stored)
		{
			input.id = ("textInput"+JsInput.myTextIndex);
			JsInput.myTextInputs.push({ Element:input, PosX:posx, posY:posy});
			index = JsInput.myTextIndex;
			JsInput.myTextIndex++;
		}
		
		var myFont = UTF8ToString(FontName);
		myFont = myFont.split(".")[0];
        input.onfocus = function()
		{
			input.value  = "";
		};
		input.value  = UTF8ToString(Text);
        input.type = "text";
		input.setAttribute("style","resize:none");
		input.style.color = "rgba(" + R + "," + G  + "," + B + "," + A +")";
        input.className = "css-class-name"; // set the CSS class
        input.style.position="absolute";
        fatherdiv.appendChild(input); // put it into the DOM
        input.style.left=(offsetx + posx)+'px';
        input.style.top=(offsety + posy)+'px';
        /*input.style.width = width +'px';
        input.style.height = height +'px';*/
        input.style.backgroundColor = "rgba(255,255,255,0.0)";
		input.style.textAlign = "left";
		input.style.font = FontSize + "px " + myFont;
		input.style.border = "none";

		if(maxSize>0)
			input.setAttribute("maxlength",maxSize);
		
		function MouseOut(event) 
		{
			for(i=0;i<JsInput.myTextIndex; i++)
			{
                if(JsInput.myTextInputs[i] != null)
                {
				    if(JsInput.myTextInputs[i].Element == event.target)
				    {
					    JsInput.myCurrentTextInput = JsInput.myTextInputs[i].Element;
				    }
                }
			}
		}
		
		input.addEventListener("blur", MouseOut, false);

        if(JsInput.myDisableAllInputs == true)
        {
			input.disabled = true;
            input.hidden = true;
        }
		return index;
    },

	JSCreateTextAreaUTF16: function(posx,posy,col, row, Text, R, G, B, A, FontSize, FontName, maxSize)
    {
		var theText  = JsInput.Pointer_stringify16(Text);
        var canvas = Module['canvas'];
		 
        var fatherdiv=canvas.parentElement;
        var offsetx=canvas.offsetLeft;
        var offsety=canvas.offsetTop;
		
		
        var input = document.createElement("textarea");
		input.cols = col;
		input.rows = row;
		
		var stored = false;
		var index = 0;
		//Recherche un trou dans le tableau
		for(i=0;i<JsInput.myTextIndex; i++)
		{
			if(JsInput.myTextInputs[i] == null)
			{
				input.id = ("textInput"+i);
				JsInput.myTextInputs[i] = { Element:input, PosX:posx, posY:posy};
				stored = true;
				index = i;
				break;
			}
		}
		if(!stored)
		{
			input.id = ("textInput"+JsInput.myTextIndex);
			JsInput.myTextInputs.push({ Element:input, PosX:posx, posY:posy});
			index = JsInput.myTextIndex;
			JsInput.myTextIndex++;
		}
		
		var myFont = UTF8ToString(FontName);
		myFont = myFont.split(".")[0];
        input.onfocus = function()
		{
			input.value  = "";
		};
		input.value  = theText;
        input.type = "text";
		input.setAttribute("style","resize:none");
		input.style.color = "rgba(" + R + "," + G  + "," + B + "," + A +")";
        input.className = "css-class-name"; // set the CSS class
        input.style.position="absolute";
        fatherdiv.appendChild(input); // put it into the DOM
        input.style.left=(offsetx + posx)+'px';
        input.style.top=(offsety + posy)+'px';
        input.style.backgroundColor = "rgba(255,255,255,0.0)";
		input.style.textAlign = "left";
		input.style.font = FontSize + "px " + myFont;
		input.style.border = "none";

		if(maxSize>0)
			input.setAttribute("maxlength",maxSize);
		
		function MouseOut(event) 
		{
			//JsInput.mySelectedTextInput =
			for(i=0;i<JsInput.myTextIndex; i++)
			{		
                if(JsInput.myTextInputs[i] != null)
                {
                    if(JsInput.myTextInputs[i].Element == event.target)
				    {
					    JsInput.myCurrentTextInput = JsInput.myTextInputs[i].Element;
				    }
                }
			}
		}
		
		input.addEventListener("blur", MouseOut, false);
		
        if(JsInput.myDisableAllInputs == true)
        {
			input.disabled = true;
            input.hidden = true;
        }
		return index;
    },
	
	JSCheckFocus: function(index)
	{
		if(JsInput.myCurrentTextInput != null)
		{
			if(JsInput.myCurrentTextInput.id == ("textInput"+index))
			{
				JsInput.myCurrentTextInput = null;
				return 1;
			}
		}
		return 0;
	},
	
    JSSetTextInput: function(index, text)
    {
        if(JsInput.myTextInputs[index] != null)
		{
			var input  = JsInput.myTextInputs[index].Element;
  			input.value = UTF8ToString(text);
       }
    },

    JSSetTextInput16: function(index, text)
    {
        if(JsInput.myTextInputs[index] != null)
		{
			var input  = JsInput.myTextInputs[index].Element;
            var L_Value = JsInput.Pointer_stringify16(text);
  			input.value = L_Value;
       }
    },


	JSGetText: function(index, text)
	{
		if(JsInput.myTextInputs[index] != null)
		{
			var input  = JsInput.myTextInputs[index].Element;
  			var mastring = input.value;
			if(mastring.length > 0)
			{
				var mallocbuffer=_malloc(mastring.length*2+2);

		
				HEAP32[text>>2]=mallocbuffer;

				var bufferpos=mallocbuffer>>1;

				for(var i = 0; i < mastring.length; i++)
				{
					HEAPU16[bufferpos++] = mastring.charCodeAt(i);
				}
				HEAPU16[bufferpos] = '\0';
			}
		}
	},
		
	JSDeleteTextInput: function(index)
	{
		var canvas = Module['canvas'];
        var theparent=canvas.parentElement;
		var element  = JsInput.myTextInputs[index].Element;
		
		theparent.removeChild(element);
  		JsInput.myTextInputs[index] = null;
	},
	
    JSGetDisableAllInputs: function()
    {
        return JsInput.myDisableAllInputs;
    },
    
    JSDisableInputs: function(_id)
    {
		var element  = JsInput.myTextInputs[_id].Element;
		element.disabled = true;
        element.hidden = true;
    },

    JSEnableInputs: function(_id)
    {
		var element  = JsInput.myTextInputs[_id].Element;
		element.disabled = false;
        element.hidden = false;
    },

    JSDisableAllInputs: function()
    {
        JsInput.myDisableAllInputs = true;
        for(var i =0; i < JsInput.myTextIndex; i++)
  		{
			if(JsInput.myTextInputs[i] != null)
			{
				var element  = JsInput.myTextInputs[i].Element;
				element.disabled = true;
                element.hidden = true;
			}
  		}
    },

    JSEnableAllInputs: function()
    {
        JsInput.myDisableAllInputs = false;
        for(var i =0; i < JsInput.myTextIndex; i++)
  		{
			if(JsInput.myTextInputs[i] != null)
			{
				var element  = JsInput.myTextInputs[i].Element;
				element.disabled = false;
                element.hidden = false;
			}
  		}
    },

	JSCreateMultilineText: function(TextToWrite, maxLineNumber, maxWidth, FontSize, FontName, jumpedLines,R, G, B, A)
	{
		TextToWrite = UTF8ToString(TextToWrite);

		
		//Cut string to keep "\n" form
		var  ligne = TextToWrite.split("\n");
		
		function MultilineText(ctx, textToWrite, maxWidth, text) 
		{			
			var currentText = textToWrite;
			var futureText;
			var subWidth = 0;
			var maxLineWidth = 0;
			
			var tmptextToWrite = textToWrite.replace("\n","");
			tmptextToWrite = tmptextToWrite.replace("\n","");
			var wordArray;
			if(tmptextToWrite.length > 0)
				wordArray = tmptextToWrite.split(" ");
			else
				wordArray = textToWrite.split(" ");
				
			var wordsInCurrent, wordArrayLength;
			wordsInCurrent = wordArrayLength = wordArray.length;
			
			// Reduce currentText until it is less than maxWidth or is a single word
			// futureText var keeps track of text not yet written to a text line
			while (ctx.measureText(currentText).width > maxWidth && wordsInCurrent > 1) 
			{
				wordsInCurrent--;
				
				currentText = futureText = "";
				for(var i = 0; i < wordArrayLength; i++) 
				{
					if (i < wordsInCurrent) 
					{
						currentText += wordArray[i];
							
						if (i+1 < wordsInCurrent)
							currentText += " ";
					}
					else 
					{
						futureText += wordArray[i];
						if(i+1 < wordArrayLength) 
						{ 
							futureText += " "; 
						}
					}
				}
			}
			
			
			text.push(currentText); // Write this line of text to the array
			
            if(maxLineNumber > 0 && text.length >= maxLineNumber)
				futureText = null;

			maxLineWidth = ctx.measureText(currentText).width;
			
			// If there is any text left to be written call the function again
			if(futureText) 
			{
				subWidth = MultilineText(ctx, futureText, maxWidth, text);
				if (subWidth > maxLineWidth) 
				{ 
					maxLineWidth = subWidth;
				}
			}
			
			// Return the maximum line width
			return maxLineWidth;
		}
		
		var ctx = JsInput.myCurrentCanvas.getContext('2d');
		var text = [];
		var textX, textY;

		var myFont = UTF8ToString(FontName);
		myFont = myFont.split(".")[0];
		ctx.font = FontSize + "px " + myFont;
		
        var LineNumber = ligne.length;
        if(maxLineNumber > 0 && ligne.length > 1)
            LineNumber = maxLineNumber;
  
  		for(var i = 0; i <	LineNumber; i++)
		{
			MultilineText(ctx, ligne[i], maxWidth, text);
		}

		var canvasX = maxWidth;
		var canvasY = FontSize*(text.length+1);
		
		textX = canvasX/2;
		var offset = 0;
		
		JsInput.myCurrentCanvas.width = canvasX;
  		JsInput.myCurrentCanvas.height = canvasY+offset;
		
		
		ctx.font = FontSize + "px " + myFont;
  		ctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + A +")";
  		ctx.font = FontSize + "px " + myFont;
  		ctx.textBaseline = 'middle';
      	ctx.textAlign = 'start';
		
		for(var i = 0; i < (text.length-jumpedLines); i++) 
		{
			textY = (i+1)*FontSize + offset;
			ctx.fillText(text[i+jumpedLines], 0,  textY);
		}
	},
	
	JSCreateMultilineTextUTF16: function(TextToWrite, maxLineNumber, maxWidth, FontSize, FontName,jumpedLines, R, G, B, A)
	{
		var myText  = JsInput.Pointer_stringify16(TextToWrite);
 

  		//Cut string to keep "\n" form
  		var  ligne = myText.split("\n");

  		function MultilineText(ctx, textToWrite, maxWidth, text) 
    	{			
    		var currentText = textToWrite;
    		var futureText;
    		var subWidth = 0;
    		var maxLineWidth = 0;
    			
    		var tmptextToWrite = textToWrite.replace("\n","");
    		tmptextToWrite = tmptextToWrite.replace("\n","");
    		var wordArray;
    		if(tmptextToWrite.length > 0)
    			wordArray = tmptextToWrite.split(" ");
    		else
    			wordArray = textToWrite.split(" ");
    				
    		var wordsInCurrent, wordArrayLength;
    		wordsInCurrent = wordArrayLength = wordArray.length;
    			
    		// Reduce currentText until it is less than maxWidth or is a single word
    		// futureText var keeps track of text not yet written to a text line
				
    		while (ctx.measureText(currentText).width > maxWidth && wordsInCurrent > 1) 
    		{
    			wordsInCurrent--;
    				
    			currentText = futureText = "";
    			for(var i = 0; i < wordArrayLength; i++) 
    			{
    				if (i < wordsInCurrent) 
    				{
    					currentText += wordArray[i];
    							
    					if (i+1 < wordsInCurrent)
    						currentText += " ";
    				}
    				else 
    				{
    					futureText += wordArray[i];
    					if(i+1 < wordArrayLength) 
    					{ 
    						futureText += " "; 
    					}
    				}
    			}
    		}
    			
    		text.push(currentText); // Write this line of text to the array
    		
            if(maxLineNumber > 0 && text.length >= maxLineNumber)
				futureText = null;

    		maxLineWidth = ctx.measureText(currentText).width;
    			
    		// If there is any text left to be written call the function again
    		if(futureText) 
    		{
    			subWidth = MultilineText(ctx, futureText, maxWidth, text);
    			if (subWidth > maxLineWidth) 
    			{ 
    				maxLineWidth = subWidth;
    			}
    		}
    			
    		// Return the maximum line width
    		return maxLineWidth;
    	}
		
		var ctx = JsInput.myCurrentCanvas.getContext('2d');
		var myFont = UTF8ToString(FontName);
		myFont = myFont.split(".")[0];
		ctx.font = FontSize + "px " + myFont;
		
		var text = [];
		var textX, textY;
		
        var LineNumber = ligne.length;
        if(maxLineNumber > 0 && ligne.length > 1)
            LineNumber = maxLineNumber;

        for(var i = 0; i <	LineNumber; i++)
		{
			MultilineText(ctx, ligne[i], maxWidth, text);
		}

		var canvasX = maxWidth;
		var canvasY = FontSize*(text.length+1);
		
		textX = canvasX/2;
		var offset = 0;
		
		JsInput.myCurrentCanvas.width = canvasX;
  		JsInput.myCurrentCanvas.height = canvasY+offset;
		
		ctx.font = FontSize + "px " + myFont;
		ctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + A +")";
		ctx.font = FontSize + "px " + myFont;
		ctx.textBaseline = 'middle';
    	ctx.textAlign = 'start';
		
		for(var i = 0; i < (text.length-jumpedLines); i++) 
		{
			textY = (i+1)*FontSize + offset;
			ctx.fillText(text[i+jumpedLines], 0,  textY);
		}
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
	
	GetCanvasPositionX: function()
	{
		var canvas = Module['canvas'];
		return canvas.offsetLeft;
	},
	
	GetCanvasPositionY: function()
	{
		var canvas = Module['canvas'];
		return canvas.offsetTop;
	},
	

	isTouchSupported: function()
	{
		var msTouchEnabled = window.navigator.msMaxTouchPoints;
		var generalTouchEnabled = "ontouchstart" in document.createElement("div");

		if (msTouchEnabled || generalTouchEnabled) {
			return true;
		}
		return false;
	},
	
	isDesktopBrowser: function()
	{
		 var ismobile=navigator.userAgent.match(/(iPad)|(iPhone)|(iPod)|(android)|(webOS)/i);
		if (!ismobile)
		{
			return true;
		}
		return false;
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
		
		var canvas = Module['canvas'];



		// use both mouse & touch
		//if (!isTouchSupported())
		{
		    var mousewheelevt = (/Firefox/i.test(navigator.userAgent)) ? "DOMMouseScroll" : "mousewheel" //FF doesn't recognize mousewheel as of FF3.x
		    canvas.addEventListener("mousedown", MouseDown, false);
            window.addEventListener("mouseup", MouseUp, false);
            canvas.addEventListener("mousemove", MouseMove, false);
            canvas.addEventListener(mousewheelevt, MouseWheel, false);
		}
		//else
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
	
/**********************************************************************************************************************************
														AUDIO
 ***********************************************************************************************************************************/	
 


	StopSound: function() 
	{
		if(JsInput.myAudioPlayer != null)
		{
			JsInput.myAudioPlayer.pause();
			JsInput.myAudioState = 2;
		}
	},

	PlaySound: function(SoundName, loop, volume) 
	{
		SoundName = UTF8ToString(SoundName);
		
		JsInput.myAudioShouldLoop = loop;
		JsInput.myAudioVolume = volume;
		var reload = false;
		if(JsInput.myAudioPlayer == null)
		{
			JsInput.myAudioPlayer = document.createElement('audio');
			JsInput.myAudioPlayer.id = ('AudioPlayer');
		}
		else
		{
			JsInput.myAudioPlayer.pause();
			reload = true;
			JsInput.myAudioPlayer.removeChild(JsInput.myAudioSource1);
			JsInput.myAudioPlayer.removeChild(JsInput.myAudioSource2);
		}
			
		//Create Sources Elements
		if(JsInput.myAudioSource1 == null)
		{
			JsInput.myAudioSource1 = document.createElement('source');
			JsInput.myAudioSource1.setAttribute("type","audio/mp3");
		}
		if(JsInput.myAudioSource2 == null)
		{
			JsInput.myAudioSource2 = document.createElement('source');
			JsInput.myAudioSource2.setAttribute("type","audio/ogg");
		}
       
		JsInput.myAudioSource1.setAttribute("src",SoundName+".mp3");
        JsInput.myAudioSource2.setAttribute("src",SoundName+".ogg");
    	
        //Add source to video
		JsInput.myAudioPlayer.appendChild(JsInput.myAudioSource1);	
		JsInput.myAudioPlayer.appendChild(JsInput.myAudioSource2);
    	
		JsInput.myAudioPlayer.loop = JsInput.myAudioShouldLoop;
		JsInput.myAudioPlayer.volume = JsInput.myAudioVolume;
		
		function AudioFinished()
		{
			if(!JsInput.myAudioShouldLoop)
				JsInput.myAudioState = 1;
            else
            {
                JsInput.myAudioPlayer.currentTime = 0;
                JsInput.myAudioPlayer.play();
            }
		};
		
		//Add event listener
		JsInput.myAudioPlayer.addEventListener("ended", AudioFinished, false);
		
  		//Play video
		if(reload)
			JsInput.myAudioPlayer.load();
			
        JsInput.myAudioPlayer.play();
        JsInput.myAudioState = 0;

        return JsInput.myAudioPlayer.duration;
	},
	
    JSGetSoundState: function()
    {
        /*
            returned values :
            0 : none
            1 : play finished
            2 : play stoped
        */

        return JsInput.myAudioState;
    },
    
    JSSetVolume: function(_volume)
    {
        JsInput.myAudioVolume = _volume;
        if(JsInput.myAudioPlayer != null)
        {
            JsInput.myAudioPlayer.volume = JsInput.myAudioVolume;
        }
    },

//*********************************************************************************************************************************//
//														VIDEO                                                                      //
//*********************************************************************************************************************************//
 
    JSCreateAlphaVideo: function(width,height, X, Y, VideoName, loop, shouldHideAtTheEnd, DisplayID, bufferID)
	{
        
        //Video finished
        function VideoFinished(event) 
		{
			var index = 0;
			var Element = null;
			
			for( index = 0; index < JsInput.myVideoList.length; index++)
			{
				if(JsInput.myVideoList[index])
				{
					if(JsInput.myVideoList[index].Video == event.currentTarget)
					{
						Element = JsInput.myVideoList[index];
						break;
					}
				}
			}
			if(Element != null && !Element.Loop)
			{
				Element.IsFinished = true;
				if(Element.Hide)
				{
					StopVideo(Element.Display);
					if(Element.Display != null)
					{
						Element.Display.style.display = "none";
					}

					if(Element.Src1.parentElement != null)
					{
						Element.Video.removeChild(Element.Src1);
						Element.Video.removeChild(Element.Src2);
					}
					if(Element.Video.parentElement != null)
						fatherdiv.removeChild(Element.Video);
				}
			}
			else
			{
				Element.IsFinished = false;
			}

            if(Element == null)
            {
                alert("ELEMENT NULL\n");
                console.log(event);
            }
        }

		var canvas = Module['canvas'];
        var fatherdiv=canvas.parentElement;
		var initVideo = true;
		
        var offsetx=canvas.offsetLeft;
        var offsety=canvas.offsetTop;
		
  		VideoName = UTF8ToString(VideoName);
        //JsInput.myShouldHideVideoAtTheEnd = shouldHideAtTheEnd;
		
        var L_Video = null;
        var L_Display = JsInput.myCanvasList[DisplayID];
        var L_Buffer =  JsInput.myCanvasList[bufferID];
        var L_Loop = false;
        var L_ShoulldHideAtTheEnd = false;
        var L_Src1 = null;
        var L_Src2 = null;
        var L_IsFinished = false;
        var L_Px = 0;
        var L_Py = 0;
        var L_Sx = 0;
        var L_Sy = 0;

        //Parse video List to find my display canvas
        var index = 0;
        var found = false;
        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_Display.id)
				{
					L_Video                 = JsInput.myVideoList[index].Video;
					L_Loop                  = JsInput.myVideoList[index].Loop;
					L_ShoulldHideAtTheEnd   = JsInput.myVideoList[index].Hide;
					L_Src1                  = JsInput.myVideoList[index].Src1;
					L_Src2                  = JsInput.myVideoList[index].Src2;
					L_IsFinished            = JsInput.myVideoList[index].IsFinished;
					L_Px                    = JsInput.myVideoList[index].Px;
					L_Py                    = JsInput.myVideoList[index].Py;
					L_Sx                    = JsInput.myVideoList[index].Sx;
					L_Sy                    = JsInput.myVideoList[index].Sy;
					found                   = true;
					break;
				}
			}
        }

        if(!found)
        {
            L_Loop = loop;
            L_ShoulldHideAtTheEnd = shouldHideAtTheEnd;
            L_Px = X;
            L_Py = Y;
            L_Sx = width;
            L_Sy = height;
        }
	
        //Video exist
  		if(L_Video != null)
  		{
            //Stop it and remove sources
			initVideo = false;
  			StopVideo(L_Display);
  			
			if(L_Src1.parentElement != null)
			{
				L_Video.removeChild(L_Src1);
				L_Video.removeChild(L_Src2);
			}
  		}
        else
  		{
             //Create video element
  			L_Video = document.createElement('video');
  			L_Video.style.position="absolute";
  			L_Video.style.display = "none";
			
			// put it into the DOM
			fatherdiv.appendChild(L_Video); 

            //Add listener to video player
		    L_Video.addEventListener("ended", VideoFinished, false);
  		}
    	
		//Create Sources Elements
		if(L_Src1 == null)
  		{
			L_Src1 = document.createElement('source');
			L_Src1.setAttribute("type","video/webm");
		}
        if(L_Src2 == null)
  		{
			L_Src2 = document.createElement('source');
			L_Src2.setAttribute("type","video/mp4");
		}
		L_Src1.setAttribute("src",VideoName+".webm");
        L_Src2.setAttribute("src",VideoName+".mp4");
    	
        //Add source to video
		L_Video.appendChild(L_Src1);	
		L_Video.appendChild(L_Src2);
    		
  		//Play video
		if(initVideo)
        {
  		    CreateVideo(L_Sx,L_Sy, offsetx+L_Px , offsety+L_Py, L_Loop, L_Video, L_Display, L_Buffer);
            
			L_IsFinished = false;
            JsInput.myVideoList.push({ Video:L_Video, Display:L_Display, Buffer:L_Buffer, Loop:L_Loop, Hide:L_ShoulldHideAtTheEnd, Src1:L_Src1, Src2:L_Src2, IsFinished:L_IsFinished, Px:L_Px , Py:L_Py, Sx:L_Sx, Sy:L_Sy});
        }
  		else
  		{
            //show display canvas 
  			if(L_Display != null)
            {
  				L_Display.style.display = "inline";
            }
  
            //Check if size or position have changed
            if( L_Px!= X || L_Py != Y || L_Sx != width || L_Sy != height)
            {
                L_Px = X;
                L_Py = Y;
                L_Sx= width;
                L_Sy = height;
				
                //Resize Video
                ResizeVideo(L_Display,L_Sx,L_Sy, offsetx+L_Px , offsety+L_Py);
            }

            //Check if loop have changed
            if(L_Loop != loop)
            {
                ChangeVideoLoopMode(L_Display,loop);
                L_Loop = loop;
            }
            L_Video.load();
			ReloadVideo(L_Display);
            L_IsFinished = false;
			
            //Update Values
            JsInput.myVideoList[index].Video        = L_Video;
            JsInput.myVideoList[index].Display      = L_Display;
            JsInput.myVideoList[index].Loop         = L_Loop;
            JsInput.myVideoList[index].Hide         = L_ShoulldHideAtTheEnd;
            JsInput.myVideoList[index].Src1         = L_Src1;
            JsInput.myVideoList[index].Src2         = L_Src2;
            JsInput.myVideoList[index].IsFinished   = L_IsFinished;
            JsInput.myVideoList[index].Px           = L_Px;
            JsInput.myVideoList[index].Py           = L_Py;
            JsInput.myVideoList[index].Sx           = L_Sx;
            JsInput.myVideoList[index].Sy           = L_Sy; 
		}
	},

	JSCreateVideo: function(width,height, X, Y, VideoName, loop, shouldHideAtTheEnd, DisplayID, bufferID, a_Volume, a_ismuted)
	{
        function StartVideo(event)
        {
            var index = 0;
			var Element = null;
			
			for( index = 0; index < JsInput.myVideoList.length; index++)
			{
				if(JsInput.myVideoList[index])
				{
					if(JsInput.myVideoList[index].Video == event.currentTarget)
					{
						Element = JsInput.myVideoList[index];
						break;
					}
				}
			}
            if(Element != null)
            {
                if (Element.Video.paused)
                {
                    Element.Video.play();
                    Element.IsLoaded = true;
                }
            }
        }
        
        //Video finished
        function VideoFinished(event) 
		{
			var index = 0;
			var Element = null;
			
			for( index = 0; index < JsInput.myVideoList.length; index++)
			{
				if(JsInput.myVideoList[index])
				{
					if(JsInput.myVideoList[index].Video == event.currentTarget)
					{
						Element = JsInput.myVideoList[index];
						break;
					}
				}
			}
			if(Element != null && !Element.Loop)
			{
				Element.IsFinished = true;
				if(Element.Hide)
				{
					if(Element.Display != null)
					{
						Element.Display.style.display = "none";
					}

					if(Element.Src1.parentElement != null)
					{
						Element.Video.removeChild(Element.Src1);
						Element.Video.removeChild(Element.Src2);
					}
					if(Element.Video.parentElement != null)
						fatherdiv.removeChild(Element.Video);
				}
			}
			else
			{
				Element.IsFinished = false;
			}

            if(Element == null)
            {
                console.log(event);
            }
        }

		var canvas = Module['canvas'];
        var fatherdiv=canvas.parentElement;
		var initVideo = true;
		
        var offsetx=canvas.offsetLeft;
        var offsety=canvas.offsetTop;
		
  		VideoName = UTF8ToString(VideoName);
        //JsInput.myShouldHideVideoAtTheEnd = shouldHideAtTheEnd;
		
        var L_Video = null;
        var L_Display = JsInput.myCanvasList[DisplayID];
        var L_Buffer =  JsInput.myCanvasList[bufferID];
        var L_Loop = false;
        var L_ShoulldHideAtTheEnd = false;
        var L_Src1 = null;
        var L_Src2 = null;
        var L_IsFinished = false;
        var L_Px = 0;
        var L_Py = 0;
        var L_Sx = 0;
        var L_Sy = 0;
        var L_bIsLoaded = false;

        //Parse video List to find my display canvas
        var index = 0;
        var found = false;
        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_Display.id)
				{
					L_Video                 = JsInput.myVideoList[index].Video;
					L_Loop                  = JsInput.myVideoList[index].Loop;
					L_ShoulldHideAtTheEnd   = JsInput.myVideoList[index].Hide;
					L_Src1                  = JsInput.myVideoList[index].Src1;
					L_Src2                  = JsInput.myVideoList[index].Src2;
					L_IsFinished            = JsInput.myVideoList[index].IsFinished;
					L_Px                    = JsInput.myVideoList[index].Px;
					L_Py                    = JsInput.myVideoList[index].Py;
					L_Sx                    = JsInput.myVideoList[index].Sx;
					L_Sy                    = JsInput.myVideoList[index].Sy;
					L_bIsLoaded             = JsInput.myVideoList[index].IsLoaded;
					found                   = true;
					break;
				}
			}
        }

        if(!found)
        {
            L_Loop = loop;
            L_ShoulldHideAtTheEnd = shouldHideAtTheEnd;
            L_Px = X;
            L_Py = Y;
            L_Sx = width;
            L_Sy = height;
        }
	
        function isTouchSupported()
        {
            var msTouchEnabled = window.navigator.msMaxTouchPoints;
            var generalTouchEnabled = "ontouchstart" in document.createElement("div");

            if (msTouchEnabled || generalTouchEnabled)
            {
                return true;
            }
            return false;
        }

        //Video exist
        if(L_Video != null)
        {
            //Stop it and remove sources
            initVideo = false;
            L_Video.pause();
            L_Video.currentTime = 0;
            L_bIsLoaded = false;

            if(L_Src1.parentElement != null)
            {
                L_Video.removeChild(L_Src1);
                L_Video.removeChild(L_Src2);
            }

            if (isTouchSupported())
            {
                var buttonnode = document.createElement('img');
                buttonnode.id = "BtPlayVideo";
                //buttonnode.type = 'image';
                buttonnode.src = "./TabletPlayBg.png";
                buttonnode.style.position = "absolute";
                buttonnode.style.left = canvas.offsetLeft + JsInput.MyVideoButtonPosX + 'px';
                buttonnode.style.top = canvas.offsetTop + JsInput.MyVideoButtonPosY + 'px';
                buttonnode.style.zIndex = "9999";

                buttonnode.ontouchstart = function (e) {
                    var index = 0;

                    for (index = 0; index < JsInput.myVideoList.length; index++) {
                        if (JsInput.myVideoList[index]) {
                            if (JsInput.myVideoList[index].IsLoaded == false && JsInput.myVideoList[index].Video && JsInput.myVideoList[index].Video.paused) {
                                JsInput.myVideoList[index].Video.play();
                                JsInput.myVideoList[index].IsLoaded = true;
                            }
                        }
                    }

                    var L_element = document.getElementById("BtPlayVideo");
                    if (L_element) {
                        var canvas = Module['canvas'];
                        var fatherdiv = canvas.parentElement;
                        fatherdiv.removeChild(L_element);
                    }
                    e.preventDefault();
                };

                fatherdiv.appendChild(buttonnode);
            }
        }
        else
        {
            //Create video element
            L_Video = document.createElement('video');
            L_Video.style.position="absolute";
            

            L_Video.width = width;
            L_Video.height = height;
           
            // put it into the DOM
            fatherdiv.appendChild(L_Video); 

            //Add listener to video player
            L_Video.addEventListener("ended", VideoFinished);
            if (!isTouchSupported())
            {

                L_Video.style.display = "none";
                L_Video.addEventListener("canplaythrough", StartVideo);
                L_Video.autoplay = true;
            }
            else
            {
                L_Video.style.display = "none";
                L_Video.controls = false;

                var buttonnode = document.createElement('img');
                buttonnode.id = "BtPlayVideo";
                //buttonnode.type = 'image';
                buttonnode.src = "./TabletPlayBg.png";
                buttonnode.style.position = "absolute";
                buttonnode.style.left = canvas.offsetLeft + JsInput.MyVideoButtonPosX + 'px';
                buttonnode.style.top = canvas.offsetTop + JsInput.MyVideoButtonPosY + 'px';
                buttonnode.style.zIndex = "9999";

                buttonnode.ontouchstart = function (e) {
                    var index = 0;
                    for (index = 0; index < JsInput.myVideoList.length; index++) {
                        if (JsInput.myVideoList[index]) {
                            if (JsInput.myVideoList[index].IsLoaded == false && JsInput.myVideoList[index].Video && JsInput.myVideoList[index].Video.paused) {
                                JsInput.myVideoList[index].Video.play();
                                JsInput.myVideoList[index].IsLoaded = true;
                            }
                        }
                    }

                    var L_element = document.getElementById("BtPlayVideo");
                    if (L_element) {
                        var canvas = Module['canvas'];
                        var fatherdiv = canvas.parentElement;
                        fatherdiv.removeChild(L_element);
                    }
                    e.preventDefault();
                };

                fatherdiv.appendChild(buttonnode);
            }
  		}
    	
		//Create Sources Elements
		if(L_Src1 == null)
  		{
			L_Src1 = document.createElement('source');
			L_Src1.setAttribute("type","video/ogg");
		}
        if(L_Src2 == null)
  		{
			L_Src2 = document.createElement('source');
			L_Src2.setAttribute("type","video/mp4");
        }
    
		L_Src1.setAttribute("src",VideoName+".ogv");
        L_Src2.setAttribute("src",VideoName+".mp4");
    	
	    //Add source to video
        L_Video.appendChild(L_Src2);
		L_Video.appendChild(L_Src1);	
		
		
    	
        L_Video.volume = a_Volume;	
        L_Video.muted = a_ismuted;

  		//Play video
		if(initVideo)
        {          
		    L_IsFinished = false;
			JsInput.myVideoList.push({ Video: L_Video, Display: L_Display, Buffer: L_Buffer, Loop: L_Loop, Hide: L_ShoulldHideAtTheEnd, Src1: L_Src1, Src2: L_Src2, IsFinished: L_IsFinished, IsLoaded: L_bIsLoaded, Px: L_Px, Py: L_Py, Sx: L_Sx, Sy: L_Sy });
			
        }
  		else
  		{
            //show display canvas 
  			if(L_Display != null)
            {
  				L_Display.style.display = "inline";
            }
  
            //Check if size or position have changed
            if( L_Px!= X || L_Py != Y || L_Sx != width || L_Sy != height)
            {
                L_Px = X;
                L_Py = Y;
                L_Sx= width;
                L_Sy = height;
				
                L_Video.width = width+"px";
                L_Video.height = height+"px";
            }

            //Check if loop have changed
            L_Loop = loop;
       
            L_Video.load();
            L_IsFinished = false;
			
            //Update Values
            JsInput.myVideoList[index].Video        = L_Video;
            JsInput.myVideoList[index].Display      = L_Display;
            JsInput.myVideoList[index].Loop         = L_Loop;
            JsInput.myVideoList[index].Hide         = L_ShoulldHideAtTheEnd;
            JsInput.myVideoList[index].Src1         = L_Src1;
            JsInput.myVideoList[index].Src2         = L_Src2;
            JsInput.myVideoList[index].IsFinished   = L_IsFinished;
            JsInput.myVideoList[index].Px           = L_Px;
            JsInput.myVideoList[index].Py           = L_Py;
            JsInput.myVideoList[index].Sx           = L_Sx;
            JsInput.myVideoList[index].Sy           = L_Sy;
            JsInput.myVideoList[index].IsLoaded     = L_bIsLoaded;
		}
	},
	JSCloseAlphaVideo: function(DisplayID)
	{
        var index = 0;
        var Element = null;
        var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = JsInput.myVideoList[index];
					break;
				}
			}
        }

        if (Element)
        {
		    if(Element.Video != null)
		    {
                var displaycanvas = Element.Display;
			    //Pause video
  			    StopVideo(displaycanvas);

                //hide display canvas
  			    
			    if(displaycanvas != null)
			    {
				    displaycanvas.style.display = "none";
			    }
			    var canvas = Module['canvas'];
			    var fatherdiv=canvas.parentElement;

                //Remove sources from parents
			    if(Element.Src1.parentElement != null)
			    {
				    Element.Video.removeChild(Element.Src1);
				    Element.Video.removeChild(Element.Src2);
			    }
                //Remove video from parent
			    if(Element.Video.parentElement != null)
				    fatherdiv.removeChild(Element.Video);

			    var L_element = document.getElementById("BtPlayVideo");
			    if (L_element) {
			        fatherdiv.removeChild(L_element);
			    }

                Element.IsFinished = false;
		    }

            JsInput.myVideoList[index] = Element;
        }
	},

    JSCloseVideo: function(DisplayID)
	{
        var index = 0;
        var Element = null;
        var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = JsInput.myVideoList[index];
					break;
				}
			}
        }

        if (Element)
        {
		    if(Element.Video != null)
		    {
                var displaycanvas = Element.Display;
			    //Pause video
  			    Element.Video.pause();

                //hide display canvas
  			    
			    if(displaycanvas != null)
			    {
				    displaycanvas.style.display = "none";
			    }
			    var canvas = Module['canvas'];
			    var fatherdiv=canvas.parentElement;

                //Remove sources from parents
			    if(Element.Src1.parentElement != null)
			    {
				    Element.Video.removeChild(Element.Src1);
				    Element.Video.removeChild(Element.Src2);
			    }
                //Remove video from parent
			    if(Element.Video.parentElement != null)
				    fatherdiv.removeChild(Element.Video);

			    var L_element = document.getElementById("BtPlayVideo");
			    if (L_element) {
			        fatherdiv.removeChild(L_element);
			    }

                Element.IsFinished = false;
		    }

            JsInput.myVideoList[index] = Element;
        }
	},

    JSPauseAlphaVideo: function(DisplayID)
    {
        //alert("merde");
        var index = 0;
        var Element = null;
		var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = JsInput.myVideoList[index];
					break;
				}
			}
        }
        if(Element)
        {
		    if(Element.Video != null)
		    {
			    //Pause video
  			    PauseVideo(Element.Display);
		    }
        }
	},

    JSPauseVideo: function(DisplayID)
	{
        var index = 0;
        var Element = null;
		var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = JsInput.myVideoList[index];
					break;
				}
			}
        }
        if(Element)
        {
		    if(Element.Video != null)
		    {
			    //Pause video
  			    Element.Video.pause();
		    }
        }
	},

    JSDestroyAlphaVideo: function(DisplayID)
    {
        var index = 0;
        var Element = null;
		var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = JsInput.myVideoList[index];
					break;
				}
			}
        }
        if(Element)
        {
            if(Element.Video != null)
		    {
			    var displaycanvas = Element.Display;
			    if(displaycanvas != null)
			    {
				    displaycanvas.style.display = "none";
			    }
			    var canvas = Module['canvas'];
			    var fatherdiv=canvas.parentElement;

			    if(Element.Src1.parentElement != null)
			    {
				    Element.Video.removeChild(Element.Src1);
				    Element.Video.removeChild(Element.Src2);
				
			    }
		    
                Element.Src1 = null;
			    Element.Src2 = null;

                DestroyVideo(displaycanvas);
			
                var L_Video  = Element.Video;
                L_Video.pause();

			    if(Element.Video.parentElement != null)
				    fatherdiv.removeChild(L_Video);

			    L_Video = null;
			
			    var L_element = document.getElementById("BtPlayVideo");
			    if (L_element) {
			        fatherdiv.removeChild(L_element);
			        L_element = null;
			    }

			    Element.IsFinished = false;
            }
            //Remove Element from list
            JsInput.myVideoList[index] = null;
        }
    },

    JSChangeAudioSettings: function(DisplayID,a_volume,a_ismuted)
    {
        var index = 0;
        var Element = null;
		var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = JsInput.myVideoList[index];
					break;
				}
			}
        }
        if(Element)
        {
            if(Element.Video != null)
		    {
                Element.Video.muted = a_ismuted;
                Element.Video.volume = a_volume;
            }
        }
    },

    JSDestroyVideo: function(DisplayID)
    {
        var index = 0;
        var Element = null;
		var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = JsInput.myVideoList[index];
					break;
				}
			}
        }
        if(Element)
        {
            if(Element.Video != null)
		    {
			    var displaycanvas = Element.Display;
			    if(displaycanvas != null)
			    {
				    displaycanvas.style.display = "none";
			    }
			    var canvas = Module['canvas'];
			    var fatherdiv=canvas.parentElement;

			    if(Element.Src1.parentElement != null)
			    {
				    Element.Video.removeChild(Element.Src1);
				    Element.Video.removeChild(Element.Src2);
				
			    }
		    
                Element.Src1 = null;
			    Element.Src2 = null;

                var L_Video  = Element.Video;
                L_Video.pause();

			    if(Element.Video.parentElement != null)
				    fatherdiv.removeChild(L_Video);

			    L_Video = null;
			
			    var L_element = document.getElementById("BtPlayVideo");
			    if (L_element) {
			        fatherdiv.removeChild(L_element);
			        L_element = null;
			    }

			    Element.IsFinished = false;
            }
            //Remove Element from list
            JsInput.myVideoList[index] = null;
        }
    },

    JSVideoIsFinished: function(DisplayID)
    {
        var index = 0;
        var Element = null;
        var L_DisplayID = 'theCanvas ' + DisplayID;

        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = JsInput.myVideoList[index];
					break;
				}
			}
        }
        if(Element)
            return Element.IsFinished;
        else
            return false;
    },


    JSSetExitRunTime:function(statut){

    Module['noExitRuntime']=statut;

    },


    JSDrawAlphaVideoFrames:function(DisplayID)
    {
        var index = 0;
        var Element = null;
        var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = JsInput.myVideoList[index];
					break;
				}
			}
        }
        if(Element)
        {
            if(Element.Video != null)
		    {
                DrawVideoFrames(Element.Display);
            }
        }
    },
    JSDrawVideoFrames:function(DisplayID)
    {
        var index = 0;
        var Element = null;
        var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < JsInput.myVideoList.length; index++)
        {
			if(JsInput.myVideoList[index])
			{
				if(JsInput.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = JsInput.myVideoList[index];
					break;
				}
			}
        }
        if(Element)
        {
            if(Element.Video != null)
		    {
                var tmpctx = JsInput.myCurrentCanvas.getContext('2d');
                tmpctx.drawImage(Element.Video, 0, 0, Element.Video.width, Element.Video.height);
            }
        }
        else
            console.log("no element found to draw");
    },

    //*********************************************************************************************************************************//
    //														ANIMATION                                                                      //
    //*********************************************************************************************************************************//

    JSDrawAnimationsHTML:function(a_AnimationName, a_width, a_height,a_bLoop,a_bPlayAtInit)
    {
		var L_SX = HEAP32[a_width>>2];
		var L_SY = HEAP32[a_height>>2];
		
        var L_AnimationName  = UTF8ToString(a_AnimationName);
		L_AnimationName = 'lib.' + L_AnimationName;
		
        //Create new canvas
		var AnimCanvas = document.createElement('canvas');

        //Add to list
		var stored = false;
		var index = 0;
		
		//Recherche un trou dans le tableau
		for(i=0;i<JsInput.myCanvasListSize; i++)
		{
			if(JsInput.myCanvasList[i] == null)
			{
                AnimCanvas.id = ('theCanvas ' + i);
				JsInput.myCanvasList[i] = AnimCanvas;
				stored = true;
				index = i;
				break;
			}
		}
		
		//Le tableau ne comportait pas de trou
		if(!stored)
		{
			index = JsInput.myCanvasListSize;
            AnimCanvas.id = ('theCanvas ' + index);
			JsInput.myCanvasList[JsInput.myCanvasListSize] = AnimCanvas;
			JsInput.myCanvasListSize++;
		}

        var exportRoot = eval("new "+L_AnimationName+"()");
		exportRoot.loop = a_bLoop;
		
		
		//set the canvas to screen size
		AnimCanvas.width = lib.properties.width;
		AnimCanvas.height = lib.properties.height;

		//set real animation dimensions
        HEAP32[a_width>>2]  = AnimCanvas.width;
        HEAP32[a_height>>2] = AnimCanvas.height;
		

       
        
	    stage = new createjs.Stage(AnimCanvas);
	    stage.addChild(exportRoot);
	    stage.update();

	    stage.autoClear = true;

	    createjs.Ticker.setFPS(lib.properties.fps);
        createjs.Ticker.addEventListener("tick", stage);
        if(a_bPlayAtInit == true)
		    exportRoot.play();
        else
            exportRoot.stop();

		stored = false;
		for(i=0;i<JsInput.myAnimationIndex; i++)
		{
			if(JsInput.myAnimationList[i] == null)
			{
			    JsInput.myAnimationList[i] = { ExportRoot: exportRoot, Stage: stage, AnimName: L_AnimationName, IDCanvas: index };
				stored = true;
				break;
			}
		}
		if(!stored)
		{
		    JsInput.myAnimationList.push({ ExportRoot: exportRoot, Stage: stage, AnimName: L_AnimationName, IDCanvas: index });
			JsInput.myAnimationIndex++;
		}
		
        //Return index
		return index;
    },
	
    JSSchangeLoopMode:function(a_idCanvas , a_value)
    {
        for(i=0;i<JsInput.myAnimationIndex; i++)
		{
			if(JsInput.myAnimationList[i].IDCanvas == a_idCanvas)
			{
                JsInput.myAnimationList[i].loop = a_value;
				break;
			}
		}
    },

	JSAnimationsRestart:function(a_idCanvas, play)
	{
		for(i=0;i<JsInput.myAnimationIndex; i++)
		{
			if(JsInput.myAnimationList[i].IDCanvas == a_idCanvas)
			{
                if(play)
                {
                    JsInput.myAnimationList[i].ExportRoot.gotoAndPlay(0);
                    JsInput.myAnimationList[i].ExportRoot.play();
                }
                else
                {
                    JsInput.myAnimationList[i].ExportRoot.gotoAndStop(0);
                    JsInput.myAnimationList[i].ExportRoot.stop();
                }
				break;
			}
		}
	},
	
	JSAnimationsPause:function(a_idCanvas, bPause)
	{
		for(i=0;i<JsInput.myAnimationIndex; i++)
		{
			if(JsInput.myAnimationList[i].IDCanvas == a_idCanvas)
			{
				if(bPause == true)
                     JsInput.myAnimationList[i].ExportRoot.stop();
				else
                    JsInput.myAnimationList[i].ExportRoot.play();
				break;
			}
		}
	},
	
	JSAnimationStatus:function(a_idCanvas)
	{
		for(i=0;i<JsInput.myAnimationIndex; i++)
		{
			if(JsInput.myAnimationList[i].IDCanvas == a_idCanvas)
			{
				var L_currentFrame = JsInput.myAnimationList[i].ExportRoot.currentFrame;
				var L_Size = JsInput.myAnimationList[i].ExportRoot.timeline.duration;
				if(L_currentFrame >= L_Size)
				{
					//Complete
					return 0;
				}
				else if(L_currentFrame < L_Size && L_currentFrame > 0)
				{
					//running
					return 1;
				}
					
				break;
			}
		}
		return 0;
	},
	
	JSAnimationsStop:function(a_idCanvas)
	{
		for(i=0;i<JsInput.myAnimationIndex; i++)
		{
			if(JsInput.myAnimationList[i] != null && JsInput.myAnimationList[i].IDCanvas == a_idCanvas)
			{
				/*JsInput.myAnimationList[i].CreateJS.Ticker.removeAllEventListeners();
				JsInput.myAnimationList[i].CreateJS.Ticker.off()*/
			    JsInput.myAnimationList[i].ExportRoot.stop();
			    JsInput.myAnimationList[i].ExportRoot.uncache();

                JsInput.myAnimationList[i].Stage.removeAllChildren();
                createjs.Ticker.removeEventListener(JsInput.myAnimationList[i].Stage);
				JsInput.myAnimationList[i] = null;
				JsInput.myCanvasList[a_idCanvas] = null;
				break;
			}
		}
	},
};

autoAddDeps(LibraryJsInput, '$JsInput');
mergeInto(LibraryManager.library, LibraryJsInput);


