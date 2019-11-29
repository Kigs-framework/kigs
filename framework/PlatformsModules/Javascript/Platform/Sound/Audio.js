var LibraryJsAudio  = {
    $JsAudio: {
	mySounds: [],
},

/**********************************************************************************************************************************
														AUDIO
 ***********************************************************************************************************************************/	
	JSDeleteSound: function(ID)
	{
		var index = 0;
		for(index = 0; index < JsAudio.mySounds.length; index++)
        {
			if(JsAudio.mySounds[index] != null)
			{
				if(JsAudio.mySounds[index].ID == ID)
				{
					//this sound has already loaded
					Audio = JsAudio.mySounds[index].Audio;
					Audio.pause();
					Audio = null;
					JsAudio.mySounds[index] = null;
					return true;
				}
			}
		}
		return false;
	},
	
	JSStopSound: function(ID) 
	{
		var index = 0;
		for(index = 0; index < JsAudio.mySounds.length; index++)
        {
			if(JsAudio.mySounds[index] != null)
			{
				if(JsAudio.mySounds[index].ID == ID)
				{
					//this sound has already loaded
					JsAudio.mySounds[index].Audio.pause();
					JsAudio.mySounds[index].AudioState = 2;
					break;
				}
			}
		}
	},

	JSLoadSound: function(SoundName, loop, volume)
	{
		SoundName = UTF8ToString(SoundName);
		var Audio = null;
		var AudioSource1 = null;
		var AudioSource2 = null;
		var index = 0;
		var IndexFree = -1;
		var hasfreeindex = false;
		for(index = 0; index < JsAudio.mySounds.length; index++)
        {
			if(JsAudio.mySounds[index] != null)
			{
				if(JsAudio.mySounds[index].SoundName == SoundName)
				{
					//this sound has already loaded
					Audio = JsAudio.mySounds[index].Audio;
					AudioSource1 = JsAudio.mySounds[index].Source1;
					AudioSource2 = JsAudio.mySounds[index].Source2;
					IndexFree = index;
					break;
				}
			}
			else if(IndexFree == -1)
			{
				IndexFree = index;
				hasfreeindex = true;
			}	
		}
		
		if(IndexFree == -1)
		{
			IndexFree = JsAudio.mySounds.length;
		}
		
		if(Audio == null)
		{
			//Create audio element
			Audio = document.createElement('audio');
			Audio.id = ('AudioPlayer');
			
			//Create Sources Elements
			AudioSource1 = document.createElement('source');
			AudioSource1.setAttribute("type","audio/mp3");
			AudioSource1.setAttribute("src",SoundName+".mp3");
			
			AudioSource2 = document.createElement('source');
			AudioSource2.setAttribute("type","audio/ogg");
			AudioSource2.setAttribute("src",SoundName+".ogg");
			
			//Add source to video
			Audio.appendChild(AudioSource1);	
			Audio.appendChild(AudioSource2);
			
			Audio.loop = loop;
			Audio.volume = volume;
			
			//insert sound in list
			if(hasfreeindex == false)
				JsAudio.mySounds.push({ID:IndexFree, Audio:Audio, Source1:AudioSource1, Source2:AudioSource2, Loop:loop, Volume:volume, SoundName:SoundName, AudioState:0});
			else
			{
				JsAudio.mySounds[IndexFree]= {ID:IndexFree, Audio:Audio, Source1:AudioSource1, Source2:AudioSource2, Loop:loop, Volume:volume, SoundName:SoundName, AudioState:0};
				
			}
		}
		else
		{
			//Update informations
			JsAudio.mySounds[index].Loop = loop;
			JsAudio.mySounds[index].Volume = volume;
			Audio.loop = loop;
			Audio.volume = volume;
		}
		return IndexFree;
	},
	
	JSPlaySound: function(ID, loop, volume) 
	{
		var Audio = null;
		var index = 0;
		for(index = 0; index < JsAudio.mySounds.length; index++)
        {
			if(JsAudio.mySounds[index] != null)
			{
				if(JsAudio.mySounds[index].ID == ID)
				{
					//this sound has already loaded
					Audio = JsAudio.mySounds[index].Audio;
					JsAudio.mySounds[index].Volume = volume;
					JsAudio.mySounds[index].Loop = loop;
					break;
				}
			}
		}
		
		if(Audio != null)
		{
			Audio.pause();
		}
		else
			return -1;
		
		//Add event listener
		Audio.addEventListener("ended",
		function()
		{
			if(!JsAudio.mySounds[index].Loop)
				JsAudio.mySounds[index].AudioState = 1;
            else
            {
                JsAudio.mySounds[index].Audio.currentTime = 0;
                 JsAudio.mySounds[index].Audio.play();
            }
		}, false);
		
  		//Play video
		Audio.load();
        Audio.play();
        JsAudio.mySounds[index].AudioState = 0;

        return Audio.duration;
	},
	
    JSGetSoundState: function(ID)
    {
        /*
            returned values :
			-1 : error to find this sound
            0 : none
            1 : play finished
            2 : play stoped
        */
		var index = 0;
		for(index = 0; index < JsAudio.mySounds.length; index++)
        {
			if(JsAudio.mySounds[index] != null)
			{
				if(JsAudio.mySounds[index].ID == ID)
				{
					return JsAudio.mySounds[index].AudioState;
				}
			}
		}
		
        return -1;
    },
    
    JSSetVolume: function(ID, _volume)
    {
		var index = 0;
		for(index = 0; index < JsAudio.mySounds.length; index++)
        {
			if(JsAudio.mySounds[index] != null)
			{
				if(JsAudio.mySounds[index].ID == ID)
				{
					JsAudio.mySounds[index].Volume = _volume;
					JsAudio.mySounds[index].Audio.volume = _volume;
					break;
				}
			}
		}
    },
	
	JSSetAllVolumes: function(a_volume)
	{
		var index = 0;
		for(index = 0; index < JsAudio.mySounds.length; index++)
        {
			if(JsAudio.mySounds[index] != null)
			{
				JsAudio.mySounds[index].Volume = _volume;
				JsAudio.mySounds[index].Audio.volume = _volume;
				break;

			}
		}
	},

};

autoAddDeps(LibraryJsAudio, '$JsAudio');
mergeInto(LibraryManager.library, LibraryJsAudio);


