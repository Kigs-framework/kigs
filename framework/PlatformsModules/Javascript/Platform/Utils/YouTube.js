var LibraryJsYouTube  = {
    $JsYouTube: {
	videoposx: 600,
    videoposy: 150,
	MyYTPlayer: null,
	MyYTPlayerisDone: false,
	MyYTDiv: null,
	MyYTPlayercpy: null,
	AskClose = false,
},
	LoadYouTube: function ()
	{
		var YT = window['YT'];
		if (!YT)
		{
			YT = {};
		}
		if (!YT.Player)
		{
			(function()
			{
				var a = document.createElement('script');
				a.src = 'https://s.ytimg.com/yts/jsbin/www-widgetapi-vflr_hAhz.js';
				a.async = true;
				var b = document.getElementsByTagName('script')[0];
				b.parentNode.insertBefore(a, b);
			})();
		}
	}
	JScreateYouTubeVideoPlayer: function(idsong, _width, _height, _posX, _posY, _cross)
	{
		var theWidth = _width;
		var theHeight = _height;
		var thePosX = _posX;
		var thePosY = _posY;
		
		JsInput.videoposx = thePosX;
		JsInput.videoposy = thePosY;
	
		var canvas = Module['canvas'];
		var fatherdiv = canvas.parentElement;
		var offsetx = canvas.offsetLeft;
		var offsety = canvas.offsetTop;

		var mydiv = document.getElementById('player');
		var myclosediv = document.getElementById('closeDiv');
		var ids = Pointer_stringify(idsong);

		if (mydiv != null) 
		{
			//show current div and load new video
			mydiv.style.display = "inline";
			if(myclosediv != null)
				myclosediv.style.display = "inline";
			if (JsYouTube.MyYTPlayer != null) 
			{
				JsYouTube.MyYTPlayer.stopVideo();
				JsYouTube.MyYTPlayer.clearVideo();
				JsYouTube.MyYTPlayer.loadVideoById(ids);
				JsYouTube.MyYTPlayerisDone = false;
			}
			JsYouTube.AskClose = false;
		}
		else 
		{
			JsYouTube.AskClose = false;
			//Create div player
			JsYouTube.MyYTDiv = document.createElement('div');
			JsYouTube.MyYTDiv.setAttribute("id", "player");
			JsYouTube.MyYTDiv.style.position = "absolute";
			JsYouTube.MyYTDiv.style.left = (offsetx + _thePosX) + 'px';
			JsYouTube.MyYTDiv.style.top = (offsety + _thePosY) + 'px';
			fatherdiv.appendChild(JsYouTube.MyYTDiv);

			if(_cross)
			{
				//Create close Div
				var thediv = document.createElement('div');
				thediv.setAttribute("id", "closeDiv");
				thediv.style.position = "absolute";
				thediv.style.left = (offsetx + _thePosX + theWidth) + 'px';
				thediv.style.top = (offsety + _thePosY - 20) + 'px';
				fatherdiv.appendChild(thediv);

				//Create close button
				var buttonnode = document.createElement('input');
				buttonnode.type = 'image';
				buttonnode.src = "./Assets/close.png";
				buttonnode.style.position = "absolute";
				buttonnode.style.left = -buttonnode.width + 'px';
				buttonnode.style.top = -(buttonnode.height / 2) + 'px';
				buttonnode.style.zIndex = "9999";
				
				buttonnode.onclick = function () 
				{
					if (JsYouTube.MyYTPlayer != null) 
					{
						JsYouTube.MyYTPlayer.stopVideo();
						JsYouTube.MyYTPlayer.clearVideo();
						JsYouTube.MyYTPlayer = null;
					}

					var father = JsYouTube.MyYTDiv.parentElement;
					if (father != null && JsYouTube.MyYTDiv != null) 
					{
						father.removeChild(JsYouTube.MyYTDiv);
						var closediv = document.getElementById('closeDiv');
						if (closediv != null)
							father.removeChild(closediv);
						JsYouTube.MyYTPlayerisDone = true;
					}
					JsYouTube.MyYTPlayercpy = null;
					JsYouTube.AskClose = true;
				}

				thediv.appendChild(buttonnode);
			}

			//Load player api asynchronously.
			/*var tag = document.createElement('script');		
			tag.src = "https://www.youtube.com/iframe_api";
			var firstScriptTag = document.getElementsByTagName('script')[0];
			firstScriptTag.parentNode.insertBefore(tag, firstScriptTag);*/
			var done = false;

			function onYouTubeIframeAPIReady(_ids, _width, _height) 
			{
				JsYouTube.MyYTPlayercpy = new YT.Player('player',
				{
					height: _height,
					width: _width,
					videoId: _ids,
					events:
					{
						'onReady': onPlayerReady,
						'onStateChange': onPlayerStateChange,
						'onError': onCloseAndDestroyPlayer
					}
				});
				JsYouTube.MyYTPlayerisDone = false;
			}
			function onCloseAndDestroyPlayer(evt)
			{
				if (JsYouTube.MyYTPlayer != null) 
				{
					JsYouTube.MyYTPlayer.stopVideo();
					JsYouTube.MyYTPlayer.clearVideo();
					JsYouTube.MyYTPlayer = null;
				}
				var father = JsYouTube.MyYTDiv.parentElement;
				if (father != null && JsYouTube.MyYTDiv != null) 
				{
					father.removeChild(JsYouTube.MyYTDiv);
					var closediv = document.getElementById('closeDiv');
					if (closediv != null)
						father.removeChild(closediv);
					JsYouTube.MyYTPlayerisDone = true;
				}
				JsYouTube.MyYTPlayercpy = null;
				JsYouTube.AskClose = true;
			}
			
			function onPlayerReady(evt) 
			{
				JsYouTube.MyYTPlayer = JsYouTube.MyYTPlayercpy;

				if(JsYouTube.AskClose == false)
				{
					evt.target.playVideo();
				}
			}

			function onPlayerStateChange(evt) 
			{
				if (evt.data == YT.PlayerState.PLAYING && !done) 
				{
					//setTimeout(stopVideo, 6000);
					done = true;
				}
				else if (evt.data == YT.PlayerState.ENDED) 
				{
					JsYouTube.MyYTPlayerisDone = true;
					//Stop video and Hide player
					
					var closediv = document.getElementById('closeDiv');
					if (JsYouTube.MyYTDiv != null) 
					{
						JsYouTube.MyYTDiv.style.display = "none";
						if(closediv != null)
							closediv.style.display = "none";
					}
				}
			}

			function stopVideo() 
			{
				if (JsYouTube.MyYTPlayer != null)
					JsYouTube.MyYTPlayer.stopVideo();
				JsYouTube.AskClose = true;
			}

			//--- load new video and set dysplay inline ---
			onYouTubeIframeAPIReady(ids, theWidth, theHeight);
		}
	},

	JSdestroyYouTubeVideoPlayer: function()
	{

		var closediv = document.getElementById('closeDiv');
		if (JsYouTube.MyYTDiv != null) 
		{
			JsYouTube.MyYTDiv.style.display = "none";
			if(closediv != null)
				closediv.style.display = "none";

			if (JsYouTube.MyYTPlayer != null) 
			{
				JsYouTube.MyYTPlayer.stopVideo();
				JsYouTube.MyYTPlayer.clearVideo();
				JsYouTube.MyYTPlayer = null;
			}
			var father = JsYouTube.MyYTDiv.parentElement;
			if (father != null && JsYouTube.MyYTDiv != null) 
			{
				father.removeChild(JsYouTube.MyYTDiv);
				var closediv = document.getElementById('closeDiv');
				if (closediv != null)
					father.removeChild(closediv);
				JsYouTube.MyYTPlayerisDone = true;
			}
			JsYouTube.MyYTPlayercpy = null;
			JsYouTube.AskClose = true;
			JsYouTube.MyYTDiv = null;
		}
		else
		{
			JsYouTube.MyYTPlayer = null;
			JsYouTube.MyYTPlayerisDone = true;
			JsYouTube.MyYTDiv = null;
			JsYouTube.AskClose = true;
		}
	},

	JSGetYouTubeVideoState: function()
	{
		return JsYouTube.MyYTPlayerisDone;
	},

	JShideYouTubeVideoPlayer: function()
	{
		var closediv = document.getElementById('closeDiv');
		if (JsYouTube.MyYTDiv != null) 
		{
			JsYouTube.MyYTDiv.style.display = "none";
			if(closediv != null)
				closediv.style.display = "none";
			
			if (JsYouTube.MyYTPlayer != null)
				JsYouTube.MyYTPlayer.pauseVideo();
			JsYouTube.AskClose = true;
		}
	},

	JSshowYouTubeVideoPlayer: function()
	{
		var closediv = document.getElementById('closeDiv');
		if (JsYouTube.MyYTDiv != null) 
		{
			if(closediv != null)
				closediv.style.display = "inline";
			JsYouTube.MyYTDiv.style.display = "inline";

			if (JsYouTube.MyYTPlayer != null)
				JsYouTube.MyYTPlayer.pauseVideo();
			JsYouTube.AskClose = false;
		}
	},
};

autoAddDeps(LibraryJsYouTube, '$JsYouTube');
mergeInto(LibraryManager.library, LibraryJsYouTube);


