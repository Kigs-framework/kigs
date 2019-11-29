var LibraryJs2DLayers  = {
    $Js2DLayers: {
    myVideoList: [],
	myCurrentTextInput: null,
	myRatioX: 1,
	myRatioY: 1,
    myDisableAllInputs: false,
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
},
	JSDrawTexture2: function(PosX, PosY, T00,T01,T10,T11, Alpha , R , G , B)
	{
        var HalfW = (width*0.5);
        var HalfH = (height*0.5);

        JsRenderer.myInitialContext2D.save();
		JsRenderer.myInitialContext2D.translate(PosX+HalfW, PosY+HalfH);
        JsRenderer.myInitialContext2D.transform(T00,T01,T10,T11,0,0);

        if(Alpha != 1.0)
        {
   
            if(R != -1 || G != -1 || B != -1)
            {
                //canvas temporaire
                JsRenderer.myBlendCanvas.width = JsRenderer.myCurrentCanvas.width;
                JsRenderer.myBlendCanvas.height = JsRenderer.myCurrentCanvas.height;
                var tmpctx = JsRenderer.myBlendCanvas.getContext('2d');
                tmpctx.drawImage(JsRenderer.myCurrentCanvas, 0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height);
                tmpctx.globalCompositeOperation = 'source-atop';
                tmpctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + Alpha + ")";
                tmpctx.fillRect(0, 0,JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height);
				
                JsRenderer.myInitialContext2D.globalCompositeOperation = 'source-over';
	            JsRenderer.myInitialContext2D.drawImage(JsRenderer.myBlendCanvas, -HalfW, -HalfH);
            }
            else
            {
                JsRenderer.myInitialContext2D.globalAlpha = Alpha;
                JsRenderer.myInitialContext2D.drawImage(JsRenderer.myCurrentCanvas,  -HalfW, -HalfH);
            }            
        }
        else
        {
            JsRenderer.myInitialContext2D.drawImage(JsRenderer.myCurrentCanvas,  -HalfW, -HalfH);
        }
        JsRenderer.myInitialContext2D.restore();
	},
	
	JSDrawTexture4: function(PosX, PosY, width, height, T00,T01,T10,T11, Alpha , R , G , B)
	{
        var HalfW = (width*0.5);
        var HalfH = (height*0.5);

        JsRenderer.myInitialContext2D.save();
		JsRenderer.myInitialContext2D.translate(PosX+HalfW, PosY+HalfH);
        JsRenderer.myInitialContext2D.transform(T00,T01,T10,T11,0,0);
        if(Alpha != 1.0)
        {              
            if(R != -1 || G != -1 || B != -1)
            {
                //canvas temporaire
                JsRenderer.myBlendCanvas.width = JsRenderer.myCurrentCanvas.width;
                JsRenderer.myBlendCanvas.height = JsRenderer.myCurrentCanvas.height;
                var tmpctx = JsRenderer.myBlendCanvas.getContext('2d');
                tmpctx.drawImage(JsRenderer.myCurrentCanvas, 0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height);
                tmpctx.globalCompositeOperation = 'source-atop';
                tmpctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + Alpha + ")";
                tmpctx.fillRect(0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height);
				
                JsRenderer.myInitialContext2D.globalCompositeOperation = 'source-over';
	            JsRenderer.myInitialContext2D.drawImage(JsRenderer.myBlendCanvas, -HalfW, -HalfH, width, height);
            }
            else
            {
                JsRenderer.myInitialContext2D.globalAlpha = Alpha;
                JsRenderer.myInitialContext2D.drawImage(JsRenderer.myCurrentCanvas, -HalfW, -HalfH, width, height);
            }
        }
        else
        {
            JsRenderer.myInitialContext2D.drawImage(JsRenderer.myCurrentCanvas, -HalfW, -HalfH, width, height);
        }
        JsRenderer.myInitialContext2D.restore();
	},
	
	JSDrawTextureWithTransform: function(PosX, PosY, width, height, T00,T01,T10,T11,T20,T21, Alpha , R , G , B)
	{
        JsRenderer.myInitialContext2D.save();
        JsRenderer.myInitialContext2D.transform(T00,T01,T10,T11,T20,T21);
        if(Alpha != 1.0)
        {              
            if(R != -1 || G != -1 || B != -1)
            {
                //canvas temporaire
                JsRenderer.myBlendCanvas.width = JsRenderer.myCurrentCanvas.width;
                JsRenderer.myBlendCanvas.height = JsRenderer.myCurrentCanvas.height;
                var tmpctx = JsRenderer.myBlendCanvas.getContext('2d');
                tmpctx.drawImage(JsRenderer.myCurrentCanvas, 0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height);
                tmpctx.globalCompositeOperation = 'source-atop';
                tmpctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + Alpha + ")";
                tmpctx.fillRect(0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height);
				
                JsRenderer.myInitialContext2D.globalCompositeOperation = 'source-over';
	            JsRenderer.myInitialContext2D.drawImage(JsRenderer.myBlendCanvas, PosX, PosY, width, height);
            }
            else
            {
                JsRenderer.myInitialContext2D.globalAlpha = Alpha;
                JsRenderer.myInitialContext2D.drawImage(JsRenderer.myCurrentCanvas, PosX, PosY, width, height);
            }
        }
        else
        {
            JsRenderer.myInitialContext2D.drawImage(JsRenderer.myCurrentCanvas, PosX, PosY, width, height);
        }
        JsRenderer.myInitialContext2D.restore();
	},
	
	JSDrawRectangleWithTransform: function(PosX, PosY, width, height, T00,T01,T10,T11,T20,T21, Alpha , R , G , B)
	{
        JsRenderer.myInitialContext2D.save();
        JsRenderer.myInitialContext2D.transform(T00,T01,T10,T11,T20,T21);
        
		JsRenderer.myInitialContext2D.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + Alpha + ")";
		JsRenderer.myInitialContext2D.fillRect(PosX, PosY, width, height);
	   
        JsRenderer.myInitialContext2D.restore();
	},
	
	JSDrawTexturePartWithTransform: function(PosX, PosY, width, height, offsetX, offsetY, finalWidth, finalHeight, T00,T01,T10,T11,T20,T21, Alpha , R , G , B)
	{
        JsRenderer.myInitialContext2D.save();
        JsRenderer.myInitialContext2D.transform(T00,T01,T10,T11,T20,T21);
        if(Alpha != 1.0)
        {              
            if(R != -1 || G != -1 || B != -1)
            {
                //canvas temporaire
                JsRenderer.myBlendCanvas.width = JsRenderer.myCurrentCanvas.width;
                JsRenderer.myBlendCanvas.height = JsRenderer.myCurrentCanvas.height;
                var tmpctx = JsRenderer.myBlendCanvas.getContext('2d');
                tmpctx.drawImage(JsRenderer.myCurrentCanvas, 0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height);
                tmpctx.globalCompositeOperation = 'source-atop';
                tmpctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + Alpha + ")";
                tmpctx.fillRect(0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height);
				
                JsRenderer.myInitialContext2D.globalCompositeOperation = 'source-over';
                JsRenderer.myInitialContext2D.drawImage(JsRenderer.myBlendCanvas, offsetX, offsetY, width, height, PosX, PosY, finalWidth, finalHeight);
  				
            }
            else
            {
                JsRenderer.myInitialContext2D.globalAlpha = Alpha;
                JsRenderer.myInitialContext2D.drawImage(JsRenderer.myCurrentCanvas, offsetX, offsetY, width, height, PosX, PosY, finalWidth, finalHeight);
            }
        }
        else
        {
            JsRenderer.myInitialContext2D.drawImage(JsRenderer.myCurrentCanvas, offsetX, offsetY, width, height, PosX, PosY, finalWidth, finalHeight);
        }
        JsRenderer.myInitialContext2D.restore();
	},
	
	JSDrawTexture8: function(PosX, PosY, imageWidth, imageHeight, offsetX, offsetY, finalWidth, finalHeight, T00,T01,T10,T11, Alpha , R , G , B)
	{

        var HalfW = (imageWidth*0.5);
        var HalfH = (imageHeight*0.5);

        JsRenderer.myInitialContext2D.save();
		JsRenderer.myInitialContext2D.translate(PosX+HalfW, PosY+HalfH);
        JsRenderer.myInitialContext2D.transform(T00,T01,T10,T11,0,0);
        if(Alpha != 1.0)
        {              
            if(R != -1 || G != -1 || B != -1)
            {
                //canvas temporaire
                JsRenderer.myBlendCanvas.width = JsRenderer.myCurrentCanvas.width;
                JsRenderer.myBlendCanvas.height = JsRenderer.myCurrentCanvas.height;
                var tmpctx = JsRenderer.myBlendCanvas.getContext('2d');
                tmpctx.drawImage(JsRenderer.myCurrentCanvas, 0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height);
                tmpctx.globalCompositeOperation = 'source-atop';
                tmpctx.fillStyle = "rgba(" + R + "," + G  + "," + B + "," + Alpha + ")";
                tmpctx.fillRect(0, 0, JsRenderer.myCurrentCanvas.width, JsRenderer.myCurrentCanvas.height);
				
                JsRenderer.myInitialContext2D.globalCompositeOperation = 'source-over';
	            JsRenderer.myInitialContext2D.drawImage(JsRenderer.myBlendCanvas, offsetX, offsetY, imageWidth, imageHeight, -HalfW, -HalfH, finalWidth, finalHeight);
            }
            else
            {
                JsRenderer.myInitialContext2D.globalAlpha = Alpha;
                JsRenderer.myInitialContext2D.drawImage(JsRenderer.myCurrentCanvas, offsetX, offsetY, imageWidth, imageHeight, -HalfW, -HalfH, finalWidth, finalHeight);
            }
        }
        else
        {
            JsRenderer.myInitialContext2D.drawImage(JsRenderer.myCurrentCanvas, offsetX, offsetY, imageWidth, imageHeight, -HalfW, -HalfH, finalWidth, finalHeight);
        }
        JsRenderer.myInitialContext2D.restore();
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
            if(JsRenderer.myCurrentCanvas.width != width)
                JsRenderer.myCurrentCanvas.width = width;
            if(JsRenderer.myCurrentCanvas.height != height)
                JsRenderer.myCurrentCanvas.height = height;

			JsRenderer.myInitialContext2D.save();
			JsRenderer.myInitialContext2D.translate(PosX, PosY);
			JsRenderer.myInitialContext2D.rotate(angleInRadians);
			JsRenderer.myInitialContext2D.drawImage(JsRenderer.myCurrentCanvas, -AnchorX , -AnchorY , width, height);
			JsRenderer.myInitialContext2D.restore();				
		}
	},
	JSGetText: function(index, text)
	{
		if(JsRenderer.myTextInputs[index] != null)
		{
			var input  = JsRenderer.myTextInputs[index].Element;
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
	JSCheckFocus: function(index)
	{
		if(Js2DLayers.myCurrentTextInput != null)
		{
			if(Js2DLayers.myCurrentTextInput.id == ("textInput"+index))
			{
				Js2DLayers.myCurrentTextInput = null;
				return 1;
			}
		}
		return 0;
	},
	 JSDisableInputs: function(_id)
    {
		var element  = JsRenderer.myTextInputs[_id].Element;
		element.disabled = true;
        element.hidden = true;
    },

    JSEnableInputs: function(_id)
    {
		var element  = JsRenderer.myTextInputs[_id].Element;
		element.disabled = false;
        element.hidden = false;
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
		for(i=0;i<JsRenderer.myTextIndex; i++)
		{
			if(JsRenderer.myTextInputs[i] == null)
			{
				input.id = ("textInput"+i);
				JsRenderer.myTextInputs[i] = { Element:input, PosX:posx, posY:posy};
				stored = true;
				index = i;
				break;
			}
		}
		if(!stored)
		{
			input.id = ("textInput"+JsRenderer.myTextIndex);
			JsRenderer.myTextInputs.push({ Element:input, PosX:posx, posY:posy});
			index = JsRenderer.myTextIndex;
			JsRenderer.myTextIndex++;
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
			for(i=0;i<JsRenderer.myTextIndex; i++)
			{
                if(JsRenderer.myTextInputs[i] != null)
                {
				    if(JsRenderer.myTextInputs[i].Element == event.target)
				    {
					    Js2DLayers.myCurrentTextInput = JsRenderer.myTextInputs[i].Element;
				    }
                }
			}
		}
		
		input.addEventListener("blur", MouseOut, false);

        if(Js2DLayers.myDisableAllInputs == true)
        {
			input.disabled = true;
            input.hidden = true;
        }
        
		return index;
    },

	JSCreateTextInputUTF16: function(posx,posy,width, height, Text, R, G, B, A, FontSize, FontName, maxSize)
    {
		var theText  = Js2DLayers.Pointer_stringify16(Text);
        var canvas = Module['canvas'];
		 
        var fatherdiv=canvas.parentElement;
        var offsetx=canvas.offsetLeft;
        var offsety=canvas.offsetTop;
		
		
        var input = document.createElement("input");
		
		
		var stored = false;
		var index = 0;
		//Recherche un trou dans le tableau
		for(i=0;i<JsRenderer.myTextIndex; i++)
		{
			if(JsRenderer.myTextInputs[i] == null)
			{
				input.id = ("textInput"+i);
				JsRenderer.myTextInputs[i] = { Element:input, PosX:posx, posY:posy};
				stored = true;
				index = i;
				break;
			}
		}
		if(!stored)
		{
			input.id = ("textInput"+JsRenderer.myTextIndex);
			JsRenderer.myTextInputs.push({ Element:input, PosX:posx, posY:posy});
			index = JsRenderer.myTextIndex;
			JsRenderer.myTextIndex++;
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
			//Js2DLayers.mySelectedTextInput =
			for(i=0;i<JsRenderer.myTextIndex; i++)
			{
                if(JsRenderer.myTextInputs[i] != null)
                {
				    if(JsRenderer.myTextInputs[i].Element == event.target)
				    {
					    Js2DLayers.myCurrentTextInput = JsRenderer.myTextInputs[i].Element;
				    }
                }
			}
		}
		
		input.addEventListener("blur", MouseOut, false);
		
        if(Js2DLayers.myDisableAllInputs == true)
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
		for(i=0;i<JsRenderer.myTextIndex; i++)
		{
			if(JsRenderer.myTextInputs[i] == null)
			{
				input.id = ("textInput"+i);
				JsRenderer.myTextInputs[i] = { Element:input, PosX:posx, posY:posy};
				stored = true;
				index = i;
				break;
			}
		}
		if(!stored)
		{
			input.id = ("textInput"+JsRenderer.myTextIndex);
			JsRenderer.myTextInputs.push({ Element:input, PosX:posx, posY:posy});
			index = JsRenderer.myTextIndex;
			JsRenderer.myTextIndex++;
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
			for(i=0;i<JsRenderer.myTextIndex; i++)
			{
                if(JsRenderer.myTextInputs[i] != null)
                {
				    if(JsRenderer.myTextInputs[i].Element == event.target)
				    {
					    Js2DLayers.myCurrentTextInput = JsRenderer.myTextInputs[i].Element;
				    }
                }
			}
		}
		
		input.addEventListener("blur", MouseOut, false);

        if(Js2DLayers.myDisableAllInputs == true)
        {
			input.disabled = true;
            input.hidden = true;
        }
		return index;
    },

	JSCreateTextAreaUTF16: function(posx,posy,col, row, Text, R, G, B, A, FontSize, FontName, maxSize)
    {
		var theText  = Js2DLayers.Pointer_stringify16(Text);
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
		for(i=0;i<JsRenderer.myTextIndex; i++)
		{
			if(JsRenderer.myTextInputs[i] == null)
			{
				input.id = ("textInput"+i);
				JsRenderer.myTextInputs[i] = { Element:input, PosX:posx, posY:posy};
				stored = true;
				index = i;
				break;
			}
		}
		if(!stored)
		{
			input.id = ("textInput"+JsRenderer.myTextIndex);
			JsRenderer.myTextInputs.push({ Element:input, PosX:posx, posY:posy});
			index = JsRenderer.myTextIndex;
			JsRenderer.myTextIndex++;
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
			//Js2DLayers.mySelectedTextInput =
			for(i=0;i<JsRenderer.myTextIndex; i++)
			{		
                if(JsRenderer.myTextInputs[i] != null)
                {
                    if(JsRenderer.myTextInputs[i].Element == event.target)
				    {
					    Js2DLayers.myCurrentTextInput = JsRenderer.myTextInputs[i].Element;
				    }
                }
			}
		}
		
		input.addEventListener("blur", MouseOut, false);
		
        if(Js2DLayers.myDisableAllInputs == true)
        {
			input.disabled = true;
            input.hidden = true;
        }
		return index;
    },
	JSSetTextInput: function(index, text)
    {
        if(JsRenderer.myTextInputs[index] != null)
		{
			var input  = JsRenderer.myTextInputs[index].Element;
  			input.value = UTF8ToString(text);
       }
    },

    JSSetTextInput16: function(index, text)
    {
        if(JsRenderer.myTextInputs[index] != null)
		{
			var input  = JsRenderer.myTextInputs[index].Element;
            var L_Value = Js2DLayers.Pointer_stringify16(text);
  			input.value = L_Value;
       }
    },
	JSDeleteTextInput: function(index)
	{
		var canvas = Module['canvas'];
        var theparent=canvas.parentElement;
		var element  = JsRenderer.myTextInputs[index].Element;
		
		theparent.removeChild(element);
  		JsRenderer.myTextInputs[index] = null;
	},
	JSGetDisableAllInputs: function()
    {
        return Js2DLayers.myDisableAllInputs;
    },
	JSDisableAllInputs: function()
    {
        Js2DLayers.myDisableAllInputs = true;
        for(var i =0; i < JsRenderer.myTextIndex; i++)
  		{
			if(JsRenderer.myTextInputs[i] != null)
			{
				var element  = JsRenderer.myTextInputs[i].Element;
				element.disabled = true;
                element.hidden = true;
			}
  		}
    },

    JSEnableAllInputs: function()
    {
        Js2DLayers.myDisableAllInputs = false;
        for(var i =0; i < JsRenderer.myTextIndex; i++)
  		{
			if(JsRenderer.myTextInputs[i] != null)
			{
				var element  = JsRenderer.myTextInputs[i].Element;
				element.disabled = false;
                element.hidden = false;
			}
  		}
    },
	
	JSSetTextureSize: function(ID, width, height)
	{
		if(width == 0 || height == 0)
			return;
		
		JsRenderer.myCanvasList[ID].width = width;
		JsRenderer.myCanvasList[ID].height = height;
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
			
			for( index = 0; index < Js2DLayers.myVideoList.length; index++)
			{
				if(Js2DLayers.myVideoList[index])
				{
					if(Js2DLayers.myVideoList[index].Video == event.currentTarget)
					{
						Element = Js2DLayers.myVideoList[index];
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
        //Js2DLayers.myShouldHideVideoAtTheEnd = shouldHideAtTheEnd;
		
        var L_Video = null;
        var L_Display = JsRenderer.myCanvasList[DisplayID];
        var L_Buffer =  JsRenderer.myCanvasList[bufferID];
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
        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_Display.id)
				{
					L_Video                 = Js2DLayers.myVideoList[index].Video;
					L_Loop                  = Js2DLayers.myVideoList[index].Loop;
					L_ShoulldHideAtTheEnd   = Js2DLayers.myVideoList[index].Hide;
					L_Src1                  = Js2DLayers.myVideoList[index].Src1;
					L_Src2                  = Js2DLayers.myVideoList[index].Src2;
					L_IsFinished            = Js2DLayers.myVideoList[index].IsFinished;
					L_Px                    = Js2DLayers.myVideoList[index].Px;
					L_Py                    = Js2DLayers.myVideoList[index].Py;
					L_Sx                    = Js2DLayers.myVideoList[index].Sx;
					L_Sy                    = Js2DLayers.myVideoList[index].Sy;
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
            Js2DLayers.myVideoList.push({ Video:L_Video, Display:L_Display, Buffer:L_Buffer, Loop:L_Loop, Hide:L_ShoulldHideAtTheEnd, Src1:L_Src1, Src2:L_Src2, IsFinished:L_IsFinished, Px:L_Px , Py:L_Py, Sx:L_Sx, Sy:L_Sy});
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
            Js2DLayers.myVideoList[index].Video        = L_Video;
            Js2DLayers.myVideoList[index].Display      = L_Display;
            Js2DLayers.myVideoList[index].Loop         = L_Loop;
            Js2DLayers.myVideoList[index].Hide         = L_ShoulldHideAtTheEnd;
            Js2DLayers.myVideoList[index].Src1         = L_Src1;
            Js2DLayers.myVideoList[index].Src2         = L_Src2;
            Js2DLayers.myVideoList[index].IsFinished   = L_IsFinished;
            Js2DLayers.myVideoList[index].Px           = L_Px;
            Js2DLayers.myVideoList[index].Py           = L_Py;
            Js2DLayers.myVideoList[index].Sx           = L_Sx;
            Js2DLayers.myVideoList[index].Sy           = L_Sy; 
		}
	},

	JSCreateVideo: function(width,height, X, Y, VideoName, loop, shouldHideAtTheEnd, DisplayID, bufferID, a_Volume, a_ismuted)
	{
        function StartVideo(event)
        {
            var index = 0;
			var Element = null;
			
			for( index = 0; index < Js2DLayers.myVideoList.length; index++)
			{
				if(Js2DLayers.myVideoList[index])
				{
					if(Js2DLayers.myVideoList[index].Video == event.currentTarget)
					{
						Element = Js2DLayers.myVideoList[index];
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
			
			for( index = 0; index < Js2DLayers.myVideoList.length; index++)
			{
				if(Js2DLayers.myVideoList[index])
				{
					if(Js2DLayers.myVideoList[index].Video == event.currentTarget)
					{
						Element = Js2DLayers.myVideoList[index];
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
        //Js2DLayers.myShouldHideVideoAtTheEnd = shouldHideAtTheEnd;
		
        var L_Video = null;
        var L_Display = JsRenderer.myCanvasList[DisplayID];
        var L_Buffer =  JsRenderer.myCanvasList[bufferID];
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
        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_Display.id)
				{
					L_Video                 = Js2DLayers.myVideoList[index].Video;
					L_Loop                  = Js2DLayers.myVideoList[index].Loop;
					L_ShoulldHideAtTheEnd   = Js2DLayers.myVideoList[index].Hide;
					L_Src1                  = Js2DLayers.myVideoList[index].Src1;
					L_Src2                  = Js2DLayers.myVideoList[index].Src2;
					L_IsFinished            = Js2DLayers.myVideoList[index].IsFinished;
					L_Px                    = Js2DLayers.myVideoList[index].Px;
					L_Py                    = Js2DLayers.myVideoList[index].Py;
					L_Sx                    = Js2DLayers.myVideoList[index].Sx;
					L_Sy                    = Js2DLayers.myVideoList[index].Sy;
					L_bIsLoaded             = Js2DLayers.myVideoList[index].IsLoaded;
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
                buttonnode.style.left = canvas.offsetLeft + JsRenderer.MyVideoButtonPosX + 'px';
                buttonnode.style.top = canvas.offsetTop + JsRenderer.MyVideoButtonPosY + 'px';
                buttonnode.style.zIndex = "9999";

                buttonnode.ontouchstart = function (e) {
                    var index = 0;

                    for (index = 0; index < Js2DLayers.myVideoList.length; index++) {
                        if (Js2DLayers.myVideoList[index]) {
                            if (Js2DLayers.myVideoList[index].IsLoaded == false && Js2DLayers.myVideoList[index].Video && Js2DLayers.myVideoList[index].Video.paused) {
                                Js2DLayers.myVideoList[index].Video.play();
                                Js2DLayers.myVideoList[index].IsLoaded = true;
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
            if (!isTouchSupported() || isDesktopBrowser())
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
                buttonnode.style.left = canvas.offsetLeft + JsRenderer.MyVideoButtonPosX + 'px';
                buttonnode.style.top = canvas.offsetTop + JsRenderer.MyVideoButtonPosY + 'px';
                buttonnode.style.zIndex = "9999";

                buttonnode.ontouchstart = function (e) {
                    var index = 0;
                    for (index = 0; index < Js2DLayers.myVideoList.length; index++) {
                        if (Js2DLayers.myVideoList[index]) {
                            if (Js2DLayers.myVideoList[index].IsLoaded == false && Js2DLayers.myVideoList[index].Video && Js2DLayers.myVideoList[index].Video.paused) {
                                Js2DLayers.myVideoList[index].Video.play();
                                Js2DLayers.myVideoList[index].IsLoaded = true;
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
			Js2DLayers.myVideoList.push({ Video: L_Video, Display: L_Display, Buffer: L_Buffer, Loop: L_Loop, Hide: L_ShoulldHideAtTheEnd, Src1: L_Src1, Src2: L_Src2, IsFinished: L_IsFinished, IsLoaded: L_bIsLoaded, Px: L_Px, Py: L_Py, Sx: L_Sx, Sy: L_Sy });
			
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
            Js2DLayers.myVideoList[index].Video        = L_Video;
            Js2DLayers.myVideoList[index].Display      = L_Display;
            Js2DLayers.myVideoList[index].Loop         = L_Loop;
            Js2DLayers.myVideoList[index].Hide         = L_ShoulldHideAtTheEnd;
            Js2DLayers.myVideoList[index].Src1         = L_Src1;
            Js2DLayers.myVideoList[index].Src2         = L_Src2;
            Js2DLayers.myVideoList[index].IsFinished   = L_IsFinished;
            Js2DLayers.myVideoList[index].Px           = L_Px;
            Js2DLayers.myVideoList[index].Py           = L_Py;
            Js2DLayers.myVideoList[index].Sx           = L_Sx;
            Js2DLayers.myVideoList[index].Sy           = L_Sy;
            Js2DLayers.myVideoList[index].IsLoaded     = L_bIsLoaded;
		}
	},
	JSCloseAlphaVideo: function(DisplayID)
	{
        var index = 0;
        var Element = null;
        var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = Js2DLayers.myVideoList[index];
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

            Js2DLayers.myVideoList[index] = Element;
        }
	},

    JSCloseVideo: function(DisplayID)
	{
        var index = 0;
        var Element = null;
        var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = Js2DLayers.myVideoList[index];
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

            Js2DLayers.myVideoList[index] = Element;
        }
	},

    JSPauseAlphaVideo: function(DisplayID)
    {
        //alert("merde");
        var index = 0;
        var Element = null;
		var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = Js2DLayers.myVideoList[index];
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
        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = Js2DLayers.myVideoList[index];
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
        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = Js2DLayers.myVideoList[index];
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
            Js2DLayers.myVideoList[index] = null;
        }
    },
	JSDestroyVideo: function(DisplayID)
    {
        var index = 0;
        var Element = null;
		var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = Js2DLayers.myVideoList[index];
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
            Js2DLayers.myVideoList[index] = null;
        }
    },

    JSVideoIsFinished: function(DisplayID)
    {
        var index = 0;
        var Element = null;
        var L_DisplayID = 'theCanvas ' + DisplayID;

        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = Js2DLayers.myVideoList[index];
					break;
				}
			}
        }
        if(Element)
            return Element.IsFinished;
        else
            return false;
    },
	JSDrawAlphaVideoFrames:function(DisplayID)
    {
        var index = 0;
        var Element = null;
        var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = Js2DLayers.myVideoList[index];
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
        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = Js2DLayers.myVideoList[index];
					break;
				}
			}
        }
        if(Element)
        {
            if(Element.Video != null)
		    {
                var tmpctx = JsRenderer.myCurrentCanvas.getContext('2d');
                tmpctx.drawImage(Element.Video, 0, 0, Element.Video.width, Element.Video.height);
            }
        }
        else
            console.log("no element found to draw");
    },
	
	JSChangeAudioSettings: function(DisplayID,a_volume,a_ismuted)
    {
        var index = 0;
        var Element = null;
		var L_DisplayID = 'theCanvas ' + DisplayID;
        for( index = 0; index < Js2DLayers.myVideoList.length; index++)
        {
			if(Js2DLayers.myVideoList[index])
			{
				if(Js2DLayers.myVideoList[index].Display.id == L_DisplayID)
				{
					Element = Js2DLayers.myVideoList[index];
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
	
	JSScaleInitialCanvas: function(ratioX,ratioY)
	{
		if((ratioX != JsRenderer.myRatioX) || (ratioY != JsRenderer.myRatioY))
		{
			// reset transform
			JsRenderer.myInitialContext2D.setTransform(1, 0, 0, 1, 0, 0);
			
			JsRenderer.myInitialContext2D.scale(ratioX,ratioY);
			JsRenderer.myRatioX = ratioX;
			JsRenderer.myRatioY = ratioY;
			
		}
	},
};

autoAddDeps(LibraryJs2DLayers, '$Js2DLayers');
mergeInto(LibraryManager.library, LibraryJs2DLayers);


