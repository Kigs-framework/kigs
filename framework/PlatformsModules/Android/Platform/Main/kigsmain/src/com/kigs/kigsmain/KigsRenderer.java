package com.kigs.kigsmain;

import java.io.ByteArrayOutputStream;
import java.io.File;

import java.io.FileOutputStream;
import java.io.IOException;

import java.nio.ByteBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;



import android.graphics.Bitmap;

import android.opengl.GLSurfaceView;
import android.os.Environment;

import android.provider.MediaStore;

public class KigsRenderer implements GLSurfaceView.Renderer {

	public enum ActivityState {
	    Uninit, Pause, Reinit, Play, Destroy   
	}
	public KigsRenderer()
	{
		
	}
	
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		synchronized(mState)
		{
			if(mState == ActivityState.Play)
			{
				// if we reached this state without pause then set pause so that the onSurfaceChanged will resume correctly
				KigsMainManager.Pause();
				KigsRenderer.setState(ActivityState.Pause);
			}
		}
	}

	public void onSurfaceChanged(GL10 gl, int w, int h)
	{
		mResolutionX=w;
		mResolutionY=h;
		//Log.i("kigsPrintf","onSurfaceChanged wait synchro");
		synchronized(mState)
		{
			//Log.i("kigsPrintf","onSurfaceChanged synchro OK");
			switch(mState)
			{
				case Uninit:
				{
					
					// first time init here
					KigsMainManager.Init(); 
					KigsMainManager.GLResize(w,h); 
		        	mState=ActivityState.Play;
		        	break;
				}
				case Pause:
				{
					//Log.i("kigsPrintf","resume after pause");
		        	KigsMainManager.GLResize(w,h); 
					KigsMainManager.Resume(true);
					mState=ActivityState.Play;
					break;
				}
				case Destroy:
				{
					/*
					KigsMainManager.Init(); 
					KigsMainManager.GLResize(w,h);
					mState=ActivityState.Play;*/
					
					KigsMainManager.GLResize(w,h); 
					KigsMainManager.Resume(true);
					mState=ActivityState.Play;
					
					break;
				}
			default:
				break;
			}
		}
	}

	public void onDrawFrame(GL10 gl)
	{
		synchronized(mState)
		{

			if(mState == ActivityState.Play)
			{
				KigsMainManager.Update();
			}
		}
			
		KigsMainManager.UpdateSurfaceView();
		
		if(KigsMainManager.NeedExit())
		{
			kigsmainActivity.askExit();
		}
		
	}

	static public void setState(ActivityState newstate)
	{
		mState=newstate;
	}
	 
	static public ActivityState getState()
	{
		return mState;
	}
	
	static public int	GetResolutionX()
	{
		return mResolutionX;
	}
	static public int	GetResolutionY()
	{
		return mResolutionY;
	}
	
	public static void	saveFrameBuffer(byte[] framebuffer, int size, int RdscnX, int RdscnY)
	{
		if (framebuffer != null && size > 0)
		{	
			//recupere la date systemeen milliseconde
			long time = System.currentTimeMillis();
			
			//Creation d'une image Bitmap a partir du frame buffer
			Bitmap bm = Bitmap.createBitmap(RdscnX, RdscnY, Bitmap.Config.ARGB_8888);
			ByteBuffer bytepicture = ByteBuffer.wrap(framebuffer);
			bm.copyPixelsFromBuffer(bytepicture);

			//Enregistrement du png dans sdcard/DCIM/Camera
			ByteArrayOutputStream bytes = new ByteArrayOutputStream();
			bm.compress(Bitmap.CompressFormat.PNG, 100, bytes);
			
			//you can create a new file name "test.jpg" in sdcard folder.
			File f = new File(Environment.getExternalStorageDirectory() + File.separator + "DCIM/Camera" + File.separator + "ImagyGame_Screenshot_" + time + ".png");
			try 
			{
				//boolean value = f.mkdirs();
				if(f.createNewFile()) {
					//write the bytes in file
					FileOutputStream fo = new FileOutputStream(f);
					fo.write(bytes.toByteArray());
					fo.close();
				}
			} 
			catch (IOException e)
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			MediaStore.Images.Media.insertImage(kigsmainActivity.getContext().getContentResolver(), bm, "ImagyGame_Screenshot_" + time + ".png", null);
			
		}
	}
	
	static public ActivityState mState=ActivityState.Uninit;
	
	static public int	mResolutionX,mResolutionY;
}
