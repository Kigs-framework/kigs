var LibraryJsRenderer  = {
    $JsRenderer: {
	myCanvasList: [],
	myCurrentCanvas: null,
	myInitialContext2D: null,
	myInitialCanvas: null,
    myBlendCanvas: null,
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
	myAnimationList: [],
	MyVideoButtonPosX: 0,
	MyVideoButtonPosY: 0,
	myRatioX : 1,
	myRatioY : 1,
	myUseWebGL : 0,
	tmpCanvas : null,
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
	splitString: function (ctx, text, maxWidth, foundwhObj, a_maxLineNumber) {
	    // lines array
	    var returned = [];
	    foundwhObj.foundw = 0;
	    foundwhObj.foundh = 0;

	    // split around line feed
	    if (text.search("\n") != -1) 
		{
	        var lines = text.split("\n");

	        for (var i = 0; i < lines.length; i++) 
			{
	            var splittedlines = [];
	            var lfoundwhObj = { foundw: 0, foundh: 0 };
	            splittedlines = JsRenderer.splitString(ctx, lines[i], maxWidth, lfoundwhObj, a_maxLineNumber);

	            if (lfoundwhObj.foundw > foundwhObj.foundw) 
				{
	                foundwhObj.foundw = lfoundwhObj.foundw;
	            }

	            returned.push(splittedlines);

	            if (a_maxLineNumber) 
				{
	                while (returned.length > a_maxLineNumber) 
					{
	                    returned.pop();
	                }
	            }
	        }

	    }
	    else // no line feed 
	    {
	        if (maxWidth) {
	            var wordArray = [];
	            wordArray = text.split(" ");

	            var currentWordIndex = 0;
	            var currentText = wordArray[currentWordIndex];
	            var previousText = "";
	            var previousWidth = 0;
	            var currentWidth;

	            while (currentWordIndex < wordArray.length) 
	            {
	                currentWidth = ctx.measureText(currentText).width;
	                if (currentWidth > maxWidth) 
	                {
	                    if (previousWidth == 0) // current word alone is too long
	                    {
	                        returned.push(currentText);

	                        previousText = "";
	                        previousWidth = 0;
	                        currentWordIndex++;

	                        currentText = wordArray[currentWordIndex];
	                    }
	                    else
	                    {
	                        if (previousWidth > foundwhObj.foundw) 
	                        {
	                            foundwhObj.foundw = previousWidth;
	                        }
	                        returned.push(previousText);
	                        previousWidth = 0;
	                        previousText = "";

	                        currentText = wordArray[currentWordIndex];
	                    }
	                }
	                else
	                {
	                    currentWordIndex++;
	                    previousWidth = currentWidth;
	                    previousText = currentText;
	                    currentText += " ";
	                    currentText += wordArray[currentWordIndex];
	                }
	            }

	            if (previousWidth)
	            {
	                returned.push(previousText);
	                if (previousWidth > foundwhObj.foundw) {
	                    foundwhObj.foundw = previousWidth;
	                }
	            }
	        }
	        else // no maxwidth just add text to returned and compute foundw and foundh
	        {
	            returned.push(text);
	            foundwhObj.foundw = ctx.measureText(text).width;
	        }
	    }

	    if ((maxWidth) && (foundwhObj.foundw > maxWidth)) {
	        foundwhObj.foundw = maxWidth;
	    }

	    foundwhObj.foundh = (returned.length);

	    return returned;
	},
},

	JSGetLanguage: function(_var)
	{
		var thevar= window.navigator.language || window.navigator.browserLanguage;
		HEAP8[_var] = thevar.charCodeAt(0);
		HEAP8[_var+1] = thevar.charCodeAt(1);
	},
	KigsCreateContext: function(withWebGL)
	{
        JsRenderer.myUseWebGL = withWebGL;
		if(JsRenderer.myInitialCanvas == null && JsRenderer.myContext2D == null)
		{
			JsRenderer.myInitialCanvas = Module['canvas'];
			if(withWebGL)
			{
				JsRenderer.myInitialContext2D = JsRenderer.myInitialCanvas.getContext('webgl');
			}
			else
			{
				JsRenderer.myInitialContext2D = JsRenderer.myInitialCanvas.getContext('2d');
			}
		}
		
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
  			

  			for(var i =0; i < JsRenderer.myTextIndex; i++)
  			{
				if(JsRenderer.myTextInputs[i] != null)
				{
					var element  = JsRenderer.myTextInputs[i].Element;
					element.style.left = offsetx+JsRenderer.myTextInputs[i].PosX +'px';
					element.style.top = offsety+JsRenderer.myTextInputs[i].PosY +'px';
				}
  			}

            var L_element = document.getElementById("BtPlayVideo");
            if (L_element != null)
            {
                L_element.style.left = canvas.offsetLeft + JsRenderer.MyVideoButtonPosX + 'px';
                L_element.style.top = canvas.offsetTop + JsRenderer.MyVideoButtonPosY + 'px';
            }
		}
		
        Browser.createContext(Module['canvas'], JsRenderer.myUseWebGL, true);
		
		//add listener to window to call method when browser is resized
		window.addEventListener("resize", BrowerResized, false);
		window.addEventListener("haschange", LeaveGame, false);
		window.addEventListener("beforeunload", LeaveGame, false);
        window.addEventListener("keydown", fkey, false);

		//LoadYouTube();

        JsRenderer.myBlendCanvas = document.createElement('canvas');
        JsRenderer.myBlendCanvas.width = 1;
		JsRenderer.myBlendCanvas.height = 1;
	},
    KigsDestroyContext: function()
    {
        var canvas = Module['canvas'];
    		 
  		var fatherdiv=canvas.parentElement;
  		var offsetx=canvas.offsetLeft;
  		var offsety=canvas.offsetTop;
  			

  		for(var i =0; i < JsRenderer.myTextIndex; i++)
  		{
			if(JsRenderer.myTextInputs[i] != null)
			{
				JsRenderer.myTextInputs[i].Element = null;
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
        JsRenderer.myInitialCanvas.parentElement.removeChild(JsRenderer.myInitialCanvas);
        JsRenderer.myInitialCanvas = null;
    },
	
	JSClearCanvas: function()
	{
		if(JsRenderer.myInitialContext2D != null)
		{
			JsRenderer.myInitialContext2D.fillStyle="rgba(" + JsRenderer.myR + "," + JsRenderer.myG  + "," + JsRenderer.myB + "," + JsRenderer.myA +")";
			JsRenderer.myInitialContext2D.fillRect ( 0 , 0 , JsRenderer.myInitialCanvas.width , JsRenderer.myInitialCanvas.height );
		}
	},
	
	JSSetBackgroundColor:function(R,G,B,A)
	{
		JsRenderer.myR = R*255;
		JsRenderer.myG = G*255;
		JsRenderer.myB = B*255;
		JsRenderer.myA = A;
	},
	
	JSDrawRect: function(PosX, PosY, width, height, T00,T01,T10,T11,T22,T21)
	{
		var HalfW = (width*0.5);
        var HalfH = (height*0.5);

        JsRenderer.myInitialContext2D.save();
		JsRenderer.myInitialContext2D.translate(PosX+HalfW, PosY+HalfH);
        JsRenderer.myInitialContext2D.transform(T00,T01,T10,T11,T22,T21);
		
		JsRenderer.myInitialContext2D.fillStyle = "rgba(" + JsRenderer.myR + "," + JsRenderer.myG  + "," + JsRenderer.myB + "," + JsRenderer.myA +")";
		JsRenderer.myInitialContext2D.fillRect(-HalfW, -HalfH, width, height);
		JsRenderer.myInitialContext2D.restore();
	},
	
	JSGenTexture: function(width, height)
	{
		if(JsRenderer.myInitialCanvas == null && JsRenderer.myContext2D == null)
		{
			JsRenderer.myInitialCanvas = Module['canvas'];
			JsRenderer.myInitialContext2D = JsRenderer.myInitialCanvas.getContext('2d');
		}
	
		//Create new canvas
		var imageCanvas = document.createElement('canvas');
		
		if(width == 0 && height == 0)
		{
			imageCanvas.width = JsRenderer.myInitialCanvas.width;
			imageCanvas.height = JsRenderer.myInitialCanvas.height;
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
		for(var i=0;i<JsRenderer.myCanvasListSize; i++)
		{
			if(JsRenderer.myCanvasList[i] == null)
			{
                imageCanvas.id = ('theCanvas ' + i);
				JsRenderer.myCanvasList[i] = imageCanvas;
				stored = true;
				index = i;
				break;
			}
		}
		
		
		//Le tableau ne comportait pas de trou
		if(!stored)
		{
			index = JsRenderer.myCanvasListSize;
            imageCanvas.id = ('theCanvas ' + index);
			JsRenderer.myCanvasList[JsRenderer.myCanvasListSize] = imageCanvas;
			JsRenderer.myCanvasListSize++;
		}

		//Return index
		return index;
	},
	
	JSSetTextureSize: function(ID, width, height)
	{
		if(width == 0 || height == 0)
			return;
		
		JsRenderer.myCanvasList[ID].width = width;
		JsRenderer.myCanvasList[ID].height = height;
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

        var ctx = JsRenderer.myCurrentCanvas.getContext('2d');
        JsRenderer.myCurrentCanvas.width = raw.width;
		JsRenderer.myCurrentCanvas.height = raw.height;
		
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

        var ctx = JsRenderer.myCurrentCanvas.getContext('2d');
		 
		var imageObj = new Image();
		var mycanvas = JsRenderer.myCurrentCanvas;
		imageObj.onload = function() 
		{
			mycanvas.width = imageObj.width;
			mycanvas.height = imageObj.height;

			ctx.drawImage(imageObj, 0, 0, imageObj.width, imageObj.height, 0, 0, imageObj.width, imageObj.height);
		};
		 imageObj.src = filename;
	},
	

	JSLoadTextureInCanvas: function(filename,width, height)
	{
		var gl = JsRenderer.myInitialCanvas.getContext("webgl");
		filename =  PATH_FS.resolve(UTF8ToString(filename));
       
        var raw = Module["preloadedImages"][filename];
        if (!raw) 
		{
			HEAP32[width>>2]=0;
			HEAP32[height>>2]=0;
			
          if (raw === null) 
			Module.printErr('Trying to reuse preloaded image, but freePreloadedMediaOnUse is set!');
			
          Runtime.warnOnce('Cannot find preloaded image ' + filename);
          return -1;
        }
        if (Module['freePreloadedMediaOnUse']) 
		{
            Module["preloadedImages"][filename] = null;
        }
        JsRenderer.tmpCanvas=document.createElement('canvas');
        var ctx = JsRenderer.tmpCanvas.getContext('2d');
        JsRenderer.tmpCanvas.width = raw.width;
        JsRenderer.tmpCanvas.height = raw.height;
		
        HEAP32[width>>2]=raw.width;
        HEAP32[height>>2]=raw.height;
        
  		ctx.drawImage(raw, 0, 0, raw.width, raw.height, 0, 0, raw.width, raw.height);

	},
	JSInitImageData: function(pixels,width, height)
	{
		var pixelData=pixels;
        var ctx = JsRenderer.tmpCanvas.getContext('2d');
        var imageData = ctx.getImageData(0, 0, width, height);
          		
        var size = width*height;
  		var index = 0;

		for (var i = 0; i < size; i++) 
		{
			
			HEAPU8[pixelData++]=imageData.data[index++];
			HEAPU8[pixelData++]=imageData.data[index++];
			HEAPU8[pixelData++]=imageData.data[index++];
			HEAPU8[pixelData++]=imageData.data[index++];
		}
			
        JsRenderer.tmpCanvas=null;
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
            JsRenderer.myXmlhttp=new XMLHttpRequest();
        }
        else
        {
            // code for IE6, IE5
            JsRenderer.myXmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
        }

        JsRenderer.myXmlhttp.open("GET",filename,true);
		JsRenderer.myXmlhttp.responseType = 'arraybuffer';
		
        JsRenderer.myXmlhttp.onreadystatechange=function()
        {
            if (JsRenderer.myXmlhttp.readyState==4 && JsRenderer.myXmlhttp.status==200)
            {
				JsRenderer.myOnlineFile = new Uint16Array(JsRenderer.myXmlhttp.response);
                return JsRenderer.myOnlineFile;
            }
        }
        
        JsRenderer.myXmlhttp.send();
	},
	
	JSDeleteTexture: function(index)
	{
		JsRenderer.myCanvasList[index] = null;
	},
	
	JSBindTexture: function(index)
	{
		JsRenderer.myCurrentCanvas = JsRenderer.myCanvasList[index];
	},
	
	JSSetPixelsData: function(colors, width, height)
	{
		JsRenderer.myCurrentCanvas.width = width;
  		JsRenderer.myCurrentCanvas.height = height;
  		var ctx = JsRenderer.myCurrentCanvas.getContext('2d');
		
  		var imageData = ctx.getImageData(0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height);
          		
        var size = width*height;
  		var index = 0;

		for (var i = 0; i < size; i++) 
		{
			//var index = i *4;
			imageData.data[index++] = HEAPU8[colors++];
			imageData.data[index++] = HEAPU8[colors++];
			imageData.data[index++] = HEAPU8[colors++];
			imageData.data[index++] = HEAPU8[colors++];
		}
			
        ctx.putImageData(imageData,0,0);
	},
	
	JSSetPixelsZone: function(colors, startposx, startposy, zonewidth, zoneheight, bitmapw,bitmaph)
	{
		var ctx = JsRenderer.myCurrentCanvas.getContext('2d');
		
  		var imageData = ctx.createImageData(zonewidth, zoneheight);
       
  		var index = 0;
		var i,j;
		for(j=0;j<zoneheight;j++)
		{
			var ColorData = new  Uint8ClampedArray(HEAPU8.buffer,colors,4*zonewidth);
  			imageData.data.set(ColorData,index);
  			index+=4*zonewidth;
  			colors+=4*bitmapw;
		}			
        ctx.putImageData(imageData,startposx, startposy, 0,0, zonewidth, zoneheight);
	},
	
	JSSetTextInfo: function(Text, Font, FontSize, R,G,B,A,_bold, _stroke, st_R,st_G,st_B,st_A )
	{
    
	    FontSize*=0.82;
		
        var myText  = UTF8ToString(Text);
        if(myText.length == 0)
            return; 	
  		
		var myFont = UTF8ToString(Font);
		var ctx = JsRenderer.myCurrentCanvas.getContext('2d');
       
        var text = [];
		function CutString(str)
		{
			var currentText = '';
			var futurText = '';
			var newligned = false;

			for(var i = 0; i < str.length; i++)
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
		JsRenderer.myCurrentCanvas.width = ctx.measureText(myText).width;
		var ComputedHeight = FontSize * 1.2;
		JsRenderer.myCurrentCanvas.height =  (ComputedHeight * text.length) + ((FontSize * 0.5) * (text.length -1));


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
  		    ctx.fillText(myText, JsRenderer.myCurrentCanvas.width*0.5,JsRenderer.myCurrentCanvas.height*0.5);
            if(_stroke)
            {
                //ctx.font = (FontSize+5) + "px " + myFont;
                ctx.strokeText(myText, JsRenderer.myCurrentCanvas.width*0.5,JsRenderer.myCurrentCanvas.height*0.5);
            }
        }
        else
		{
			var textX, textY;
			textX = JsRenderer.myCurrentCanvas.width/2;

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
	    FontSize*=0.82;
        var myText  =  JsRenderer.Pointer_stringify16(Text);
        if(myText.length == 0)
            return;

		var myFont = UTF8ToString(Font);
		myFont = myFont.split(".")[0];
		var ctx = JsRenderer.myCurrentCanvas.getContext('2d');
		
        var text = [];
		function CutString(str)
		{
			var currentText = '';
			var futurText = '';
			var newligned = false;

			for(var i = 0; i < str.length; i++)
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
		JsRenderer.myCurrentCanvas.width = ctx.measureText(myText).width;

        var ComputedHeight = FontSize * 1.2;
        JsRenderer.myCurrentCanvas.height = (ComputedHeight * text.length) + ((FontSize * 0.5) * (text.length - 1));

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
  		    ctx.fillText(myText, JsRenderer.myCurrentCanvas.width*0.5,JsRenderer.myCurrentCanvas.height*0.5);
            if(_stroke)
            {
                //ctx.font = (FontSize+5) + "px " + myFont;
                ctx.strokeText(myText, JsRenderer.myCurrentCanvas.width*0.5,JsRenderer.myCurrentCanvas.height*0.5);
            }
        }
        else
		{
			var textX, textY;
			textX = JsRenderer.myCurrentCanvas.width/2;

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
		HEAP32[width>>2]=JsRenderer.myCurrentCanvas.width;
		HEAP32[height>>2]=JsRenderer.myCurrentCanvas.height;
	},
	
	JSGetPixelColor: function(Data)
	{
		var ctx = JsRenderer.myCurrentCanvas.getContext('2d');
	
		var p = ctx.getImageData(0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height).data;
		var index = 0;
		var size = JsRenderer.myCurrentCanvas.width * JsRenderer.myCurrentCanvas.height;
		for(var i=0;i <  size; i++)
		{
			HEAPU8[Data++] = p[index++];
			HEAPU8[Data++] = p[index++];
			HEAPU8[Data++] = p[index++];
			HEAPU8[Data++] = p[index++];
		}
	},

	JSCreateMultilineText: function(TextToWrite, maxLineNumber, maxWidth, FontSize, FontName, jumpedLines,R, G, B, A)
	{
	    FontSize*=0.82;
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
		
		var ctx = JsRenderer.myCurrentCanvas.getContext('2d');
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
		
		JsRenderer.myCurrentCanvas.width = canvasX;
  		JsRenderer.myCurrentCanvas.height = canvasY+offset;
		
		
		ctx.font = FontSize + "px " + myFont;
  		ctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + A +")";
  		//ctx.font = FontSize + "px " + myFont;
  		ctx.textBaseline = 'middle';
      	ctx.textAlign = 'start';
		
		for(var i = 0; i < (text.length-jumpedLines); i++) 
		{
			textY = (i+1)*FontSize + offset;
			ctx.fillText(text[i+jumpedLines], 0,  textY);
		}
	},
	JSCreateMultilineTextUTF16: function(TextToWrite, maxLineNumber, maxWidth, FontSize, FontName,alignement,jumpedLines, R, G, B, A)
	{
	    FontSize*=0.82;
	    var myText = JsRenderer.Pointer_stringify16(TextToWrite);
  		
		var ctx = JsRenderer.myCurrentCanvas.getContext('2d');
		var myFont = UTF8ToString(FontName);
		myFont = myFont.split(".")[0];
		ctx.font = FontSize + "px " + myFont;
		
	    // test split
		var lfoundwhObj = { foundw: 0, foundh: 0 };

		var Splited = JsRenderer.splitString(ctx, myText, maxWidth, lfoundwhObj, maxLineNumber);

		var LineNumber = Splited.length;
		if (maxLineNumber > 0 && Splited.length > maxLineNumber)
            LineNumber = maxLineNumber;

		var canvasX = lfoundwhObj.foundw;
		var canvasY = FontSize * Splited.length;
        var textX, textY;
		textX = 0;
		var offset = 0;
		
		JsRenderer.myCurrentCanvas.width = canvasX;
  		JsRenderer.myCurrentCanvas.height = canvasY+offset;
		
		A = 1;
		ctx.font = FontSize + "px " + myFont;
		ctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + A +")";
		//ctx.font = FontSize + "px " + myFont;
		ctx.textBaseline = 'middle';
		
		if (alignement == 2)
		{
		    textX = canvasX;
		    ctx.textAlign = 'end';
		}
		else if (alignement == 1)
		{
		    ctx.textAlign = 'center';
		    textX = canvasX / 2;
		}
		else
		{
		    textX = 0;
		    ctx.textAlign = 'start';
		}

    	for (var i = 0; i < (Splited.length - jumpedLines) ; i++)
		{
		    textY = i * FontSize + FontSize*0.5;
		    ctx.fillText(Splited[i + jumpedLines], textX, textY);
		}
	},
	JSDrawMultilineTextUTF16: function(TextToWrite, maxLineNumber, maxWidth, FontSize, FontName,alignement,jumpedLines, R, G, B, A,width,height)
	{
	    FontSize*=0.82;
	    var myText = JsRenderer.Pointer_stringify16(TextToWrite);
  		
        JsRenderer.tmpCanvas=document.createElement('canvas');
        var ctx = JsRenderer.tmpCanvas.getContext('2d');
		var myFont = UTF8ToString(FontName);
		myFont = myFont.split(".")[0];
		ctx.font = FontSize + "px " + myFont;
		
	    // test split
		var lfoundwhObj = { foundw: 0, foundh: 0 };

		var Splited = JsRenderer.splitString(ctx, myText, maxWidth, lfoundwhObj, maxLineNumber);

		var LineNumber = Splited.length;
		if (maxLineNumber > 0 && Splited.length > maxLineNumber)
            LineNumber = maxLineNumber;

		var canvasX = lfoundwhObj.foundw;
		var canvasY = FontSize * Splited.length;
        var textX, textY;
		textX = 0;
		var offset = 0;
		
        JsRenderer.tmpCanvas.width = canvasX;
        JsRenderer.tmpCanvas.height = canvasY+offset;
		
		A = 1;
		ctx.font = FontSize + "px " + myFont;
		ctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + A +")";
		//ctx.font = FontSize + "px " + myFont;
		ctx.textBaseline = 'middle';
		
		if (alignement == 2)
		{
		    textX = canvasX;
		    ctx.textAlign = 'end';
		}
		else if (alignement == 1)
		{
		    ctx.textAlign = 'center';
		    textX = canvasX / 2;
		}
		else
		{
		    textX = 0;
		    ctx.textAlign = 'start';
		}

    	for (var i = 0; i < (Splited.length - jumpedLines) ; i++)
		{
		    textY = i * FontSize + FontSize*0.5;
		    ctx.fillText(Splited[i + jumpedLines], textX, textY);
		}
		
        HEAP32[width >> 2] = JsRenderer.tmpCanvas.width;
        HEAP32[height >> 2] = JsRenderer.tmpCanvas.height;
		
	},
	JSGetCanvasSize: function (width, height)
	{
		if(JsRenderer.myCurrentCanvas.width > 1 || JsRenderer.myCurrentCanvas.height >1)
		{
			HEAP32[width>>2]=JsRenderer.myCurrentCanvas.width ;
			HEAP32[height>>2]=JsRenderer.myCurrentCanvas.height;
		}
	},
	JSIsAlphaAtPoint:function(a_Ratiox, a_Ratioy, a_Threshold)
    {
        var ctx = JsRenderer.myCurrentCanvas.getContext('2d');
        var p = ctx.getImageData(0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height).data;
        var L_Px = Math.floor(a_Ratiox*JsRenderer.myCurrentCanvas.width);
        var L_Py = Math.floor(a_Ratioy*JsRenderer.myCurrentCanvas.height);
        var index = ((JsRenderer.myCurrentCanvas.width * L_Py)+L_Px)*4;
        return (p[index+3] < a_Threshold);    
    },
};

autoAddDeps(LibraryJsRenderer, '$JsRenderer');
mergeInto(LibraryManager.library, LibraryJsRenderer);


