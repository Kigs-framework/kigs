package com.kigs.soundmanager;

import java.io.IOException;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;
import android.os.SystemClock;


public class AudioBuffer {
	static public void	Init(AssetManager manager)
	{
		myAssetManager=manager;
		
		if(mySoundPool == null)
		{
			// not streamed sounds are played with sound pool
			mySoundPool = new SoundPool(16,AudioManager.STREAM_MUSIC,0 );
		}
	}
	
	static public void	Close()
	{
		if(mySoundPool != null)
		{
			mySoundPool.release();
			mySoundPool=null;
			myAssetManager=null;
		}
	}

	
	
	static public SoundResourceStruct 	Load(String name,boolean isStream)
	{
		SoundResourceStruct resultsound=null;
		try {
			
			AssetFileDescriptor afd=myAssetManager.openFd(name);
			// create a player just to get sound length
			MediaPlayer	player=new MediaPlayer();
			player.setDataSource(afd.getFileDescriptor(),afd.getStartOffset(),afd.getLength());
			player.prepare();
			
			// always stream more than 5 seconds sounds
			if(player.getDuration() >= 5000)
			{
				isStream=true;
			}
			
			if(!isStream) // in memory sound
			{
				// test if we can load it in the pool
				int result=mySoundPool.load(afd, 1);
				
				afd.close();
				
				if(result>=0)
				{
					// ok create the structure
					resultsound=new SoundResourceStruct();
					resultsound.mSoundID=result;
					// sound pool object don't keep player
					resultsound.mPlayer=null;
					resultsound.mLength=(((float)player.getDuration())*0.001f);

				}
				
				player.release();
			}
			else	// streaming sound
			{
				// ok create the structure
				
				resultsound=new SoundResourceStruct();
				// use negative numbers for streaming sounds
				resultsound.mSoundID=-1;
				resultsound.mPlayer=player;
				resultsound.mLength=(((float)player.getDuration())*0.001f);

			}
			
			SoundCount++;
			
			return resultsound;
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return resultsound;
	}
	 
	static public SoundStreamStruct Play(SoundResourceStruct sound,boolean loop,float lV,float rV,float P)
	{
		if(sound == null )
		{
			return null;
		}
		
		SoundStreamStruct resultstream=null;
		
		// streaming or soundpool ?
		if((sound.mSoundID >=0) && (sound.mPlayer==null)) // sound pool
		{
			int result=0;
			try
			{
				if(loop)
				{
					result=mySoundPool.play(sound.mSoundID,lV,rV,0,-1,P);
				}
				else
				{
					result=mySoundPool.play(sound.mSoundID,lV,rV,0,0,P);
					
					if(result != 0)
						mySoundPool.setLoop(result, 0);
				}
			}
			catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			if(result != 0)
			{
				resultstream=new SoundStreamStruct();
				resultstream.mLeftVolume=lV;
				resultstream.mRightVolume=rV;
				resultstream.mPitch=P;
				resultstream.mResource=sound;
				resultstream.mStreamID=result;
				resultstream.mIsLooping=loop;
				resultstream.mPaused=false;
				resultstream.mStartTime=((float)SystemClock.currentThreadTimeMillis())*0.001f;
			
			}
			
		}
		else	// streaming
		{
			if(sound.mPlayer != null)
			{
				resultstream=new SoundStreamStruct();
				resultstream.mLeftVolume=lV;
				resultstream.mRightVolume=rV;
				resultstream.mPitch=P;
				resultstream.mResource=sound;
				resultstream.mStreamID=0;
				resultstream.mIsLooping=loop;
				resultstream.mPaused=false;
				resultstream.mStartTime=((float)SystemClock.currentThreadTimeMillis())*0.001f;
				
				sound.mPlayer.setLooping(loop);
				sound.mPlayer.setVolume(resultstream.mLeftVolume,resultstream.mRightVolume);
				sound.mPlayer.start();
			}
		}
		return resultstream;
		
	}
	
	
	static public boolean Pause(SoundStreamStruct sound,boolean pause)
	{
		if(sound ==null)
		{
			return false;
		}
		
		// streaming or soundpool ?
		if((sound.mResource.mSoundID >=0) && (sound.mResource.mPlayer==null)) // sound pool
		{
			if(sound.mStreamID !=0)
			{
				if(pause)
				{
					float currentplayl=((float)SystemClock.currentThreadTimeMillis())*0.001f;
					currentplayl-=sound.mStartTime;
					sound.mStartTime=currentplayl;
					sound.mPaused=true;
					
					mySoundPool.pause(sound.mStreamID);
				}
				else
				{
					sound.mStartTime=((float)SystemClock.currentThreadTimeMillis())*0.001f-sound.mStartTime;
					sound.mPaused=false;
					mySoundPool.resume(sound.mStreamID);
				}
				return true;
			}
			
		}
		else	// streaming
		{
			if(sound.mResource.mPlayer != null)
			{
				if(pause)
				{
					sound.mResource.mPlayer.pause();
					sound.mPaused=true;
				}
				else
				{
					sound.mResource.mPlayer.start();
					sound.mPaused=false;
				}
				return true;
			}
		}
		return false;
	}
	
	static public boolean Stop(SoundStreamStruct sound)
	{
		if(sound ==null)
		{
			return false;
		}
		
		// streaming or soundpool ?
		if((sound.mResource.mSoundID >=0) && (sound.mResource.mPlayer==null)) // sound pool
		{
			if(sound.mStreamID !=0)
			{
				mySoundPool.stop(sound.mStreamID);
				sound.mStreamID=0;
				sound.mResource=null;
				sound.mPaused=false;
				sound=null;
				return true;
			}
			
		}
		else	// streaming
		{
			if(sound.mResource.mPlayer != null)
			{
				sound.mResource.mPlayer.stop();
				try {
					sound.mResource.mPlayer.prepare();
					sound.mResource=null;
					sound.mPaused=false;
					sound=null;
					
				} catch (IllegalStateException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				return true;
			}
		}
		return false;
	}
	
	static public boolean SetVolume(SoundStreamStruct sound,float Leftvolume,float RightVolume)
	{
		if(sound == null)
		{
			return false;
		}
		
		sound.mLeftVolume=Leftvolume;
		sound.mRightVolume=RightVolume;
		
		// streaming or soundpool ?
		if((sound.mResource.mSoundID >=0) && (sound.mResource.mPlayer==null)) // sound pool
		{
			if(sound.mStreamID !=0)
			{
				mySoundPool.setVolume(sound.mStreamID,sound.mLeftVolume,sound.mRightVolume);
			}
		}
		else	// streaming
		{
			if(sound.mResource.mPlayer != null)
			{
				sound.mResource.mPlayer.setVolume(sound.mLeftVolume,sound.mRightVolume);
			}
		}
		
		// always return true if a sound was found
		return true;
	}
	
	static public boolean SetPitch(SoundStreamStruct sound,float rate)
	{
		if(sound == null)
		{
			return false;
		}
		
		sound.mPitch=rate;
		
		// streaming or soundpool ?
		if((sound.mResource.mSoundID >=0) && (sound.mResource.mPlayer==null)) // sound pool
		{
			if(sound.mStreamID !=0)
			{
				mySoundPool.setRate(sound.mStreamID, sound.mPitch);
			}
		}
		else	// streaming
		{
			// NO PITCH ON MEDIA PLAYER !
			return false;
		}
		return true;
	}
	
	
	
	static public boolean Unload(SoundResourceStruct sound)
	{
		if(sound == null)
		{
			return false;
		}
		
		if((sound.mSoundID >=0) && (sound.mPlayer==null)) // sound pool
		{
			mySoundPool.unload(sound.mSoundID);
			sound.mSoundID=-1;
		}
		else
		{
			if(sound.mPlayer != null)
			{
				sound.mPlayer.release();
				sound.mPlayer=null;
			}
		}
		SoundCount--;

		if(SoundCount == 0) 
		{
			mySoundPool.release();
			mySoundPool = new SoundPool(16, AudioManager.STREAM_MUSIC, 0);
		}
		return true;
	}

	static public boolean	isPlaying(SoundStreamStruct sound)
	{
		if(sound == null)
		{
			return false;
		}
		
		if((sound.mResource.mSoundID >=0) && (sound.mResource.mPlayer==null)) // sound pool
		{
			if(sound.mStreamID !=0)
			{
				// if looping then play until stopped
				if(sound.mPaused)
				{
					return false;
				}
				if(sound.mIsLooping)
				{
					return true;
				}
				
				float length=sound.mResource.mLength;
				float currentplayl=((float)SystemClock.currentThreadTimeMillis())*0.001f;
				currentplayl-=sound.mStartTime;
				
				if(currentplayl<length)
				{
					return true;
				}
			}
		}
		else 
		{
			if(sound.mResource.mPlayer != null)
			{
				return sound.mResource.mPlayer.isPlaying();
			}
		}
		return false;
	}

	static private AssetManager myAssetManager	= null;
	static private SoundPool	mySoundPool		= null;
	static private int 			SoundCount 		= 0;

}
