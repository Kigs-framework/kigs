var LibraryJsAnimationHTML  = {
    $JsAnimationHTML: {
	myAnimationList: [],
	myAnimationIndex: 0,
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
		for(i=0;i<JsRenderer.myCanvasListSize; i++)
		{
			if(JsRenderer.myCanvasList[i] == null)
			{
                AnimCanvas.id = ('theCanvas ' + i);
				JsRenderer.myCanvasList[i] = AnimCanvas;
				stored = true;
				index = i;
				break;
			}
		}
		
		//Le tableau ne comportait pas de trou
		if(!stored)
		{
			index = JsRenderer.myCanvasListSize;
            AnimCanvas.id = ('theCanvas ' + index);
			JsRenderer.myCanvasList[JsRenderer.myCanvasListSize] = AnimCanvas;
			JsRenderer.myCanvasListSize++;
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
		for(i=0;i<JsAnimationHTML.myAnimationIndex; i++)
		{
			if(JsAnimationHTML.myAnimationList[i] == null)
			{
			    JsAnimationHTML.myAnimationList[i] = { ExportRoot: exportRoot, Stage: stage, AnimName: L_AnimationName, IDCanvas: index };
				stored = true;
				break;
			}
		}
		if(!stored)
		{
		    JsAnimationHTML.myAnimationList.push({ ExportRoot: exportRoot, Stage: stage, AnimName: L_AnimationName, IDCanvas: index });
			JsAnimationHTML.myAnimationIndex++;
		}
		
        //Return index
		return index;
    },
	
    JSSchangeLoopMode:function(a_idCanvas , a_value)
    {
        for(i=0;i<JsAnimationHTML.myAnimationIndex; i++)
		{
			if(JsAnimationHTML.myAnimationList[i].IDCanvas == a_idCanvas)
			{
                JsAnimationHTML.myAnimationList[i].loop = a_value;
				break;
			}
		}
    },

	JSAnimationsRestart:function(a_idCanvas, play)
	{
		for(i=0;i<JsAnimationHTML.myAnimationIndex; i++)
		{
			if(JsAnimationHTML.myAnimationList[i].IDCanvas == a_idCanvas)
			{
                if(play)
                {
                    JsAnimationHTML.myAnimationList[i].ExportRoot.gotoAndPlay(0);
                    JsAnimationHTML.myAnimationList[i].ExportRoot.play();
                }
                else
                {
                    JsAnimationHTML.myAnimationList[i].ExportRoot.gotoAndStop(0);
                    JsAnimationHTML.myAnimationList[i].ExportRoot.stop();
                }
				break;
			}
		}
	},
	
	JSAnimationsPause:function(a_idCanvas, bPause)
	{
		for(i=0;i<JsAnimationHTML.myAnimationIndex; i++)
		{
			if(JsAnimationHTML.myAnimationList[i].IDCanvas == a_idCanvas)
			{
				if(bPause == true)
                     JsAnimationHTML.myAnimationList[i].ExportRoot.stop();
				else
                    JsAnimationHTML.myAnimationList[i].ExportRoot.play();
				break;
			}
		}
	},
	
	JSAnimationStatus:function(a_idCanvas)
	{
		for(i=0;i<JsAnimationHTML.myAnimationIndex; i++)
		{
			if(JsAnimationHTML.myAnimationList[i].IDCanvas == a_idCanvas)
			{
				var L_currentFrame = JsAnimationHTML.myAnimationList[i].ExportRoot.currentFrame;
				var L_Size = JsAnimationHTML.myAnimationList[i].ExportRoot.timeline.duration;
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
		for(i=0;i<JsAnimationHTML.myAnimationIndex; i++)
		{
			if(JsAnimationHTML.myAnimationList[i] != null && JsAnimationHTML.myAnimationList[i].IDCanvas == a_idCanvas)
			{
				/*JsAnimationHTML.myAnimationList[i].CreateJS.Ticker.removeAllEventListeners();
				JsAnimationHTML.myAnimationList[i].CreateJS.Ticker.off()*/
			    JsAnimationHTML.myAnimationList[i].ExportRoot.stop();
			    JsAnimationHTML.myAnimationList[i].ExportRoot.uncache();

                JsAnimationHTML.myAnimationList[i].Stage.removeAllChildren();
                createjs.Ticker.removeEventListener(JsAnimationHTML.myAnimationList[i].Stage);
				JsAnimationHTML.myAnimationList[i] = null;
				JsRenderer.myCanvasList[a_idCanvas] = null;
				break;
			}
		}
	},
};

autoAddDeps(LibraryJsAnimationHTML, '$JsAnimationHTML');
mergeInto(LibraryManager.library, LibraryJsAnimationHTML);


