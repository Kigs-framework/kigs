package com.assoria.cameramanager;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.Properties;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.assoria.kigsmain.KigsGLSurfaceView;
import com.assoria.kigsmain.KigsMainManager;
import com.assoria.kigsmain.kigsmainActivity;

import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.SurfaceHolder;


// manage file open / read / write / close ...
public class CameraManager implements  SurfaceTexture.OnFrameAvailableListener{
	
	protected static CameraPreviewHandler					myCallback=null;
	protected static CameraTakePicture						myTakePictureCallBack=null;
	protected static CameraManager							myCurrentCameraManager=null;
	
	
	protected static Camera 				myCurrentCamera=null;
	protected static KigsGLSurfaceView		myView=null;
	protected static Size					myPreviewSize;	
	protected static int					myPreviewFormat;
	protected static boolean				myIsInPreview=false;
	protected static boolean				myWasPausedInPreview=false;
	
	protected static byte[]					myPreviewBuffer=null;

	private static int[] hTex;
	 private static SurfaceTexture mSTexture;
	static {
		
		/*if(KigsMainManager.isARMv7())
		{
			System.loadLibrary("KigsMain-jniv7");
		}
		else
		{
			System.loadLibrary("KigsMain-jni");
		}*/
    }
	
	//public static native boolean isARMv7();
	
	public class CameraPreviewHandler implements Camera.PreviewCallback {

		public void onPreviewFrame(byte[] arg0, Camera arg1) {
			
			CameraManager.CameraCallback(myPreviewSize.width,myPreviewSize.height,arg0);
			//Log.i("INFO DEBUG", "CameraCallback = " + myPreviewSize.width + " // " + myPreviewSize.height);
			arg1.addCallbackBuffer(arg0);
		}
		
	}
	
	public class CameraTakePicture implements Camera.PictureCallback {
		
		protected int 				mySizeX = 0;
		protected int 				mySizeY = 0;
		
		public void onPictureTaken(byte[] data, Camera camera){
			
			if ( data != null)
			{
				//transform my byte array to Bitmap and convert this one in ARGB_8888 to send it later
				byte[] tempdata = data;
				Bitmap bm = BitmapFactory.decodeByteArray(tempdata, 0, tempdata.length);
				Bitmap bm8888 = bm.copy(Config.ARGB_8888, true);
				ByteBuffer buffer = ByteBuffer.allocate((mySizeX*mySizeY)*4);
				bm8888.copyPixelsToBuffer(buffer);
				
				//call C++ method with data array in param
				CameraManager.nativeSetPictureToKigs(buffer.array(),mySizeX,mySizeY);
				
				Log.i("DEBUG INFO",""+ mySizeX +"   //   " + mySizeY);
			}
			else
			{
				Log.i("DEBUG INFO","ERROR NULL");
			}
		}
		
		public void Set_SizeX (int _value){mySizeX = _value;}
		public void Set_SizeY (int _value){mySizeY = _value;}
	}
	CameraManager()
	{
		if(myCallback == null)
		{
			myCallback=new CameraPreviewHandler();
			myTakePictureCallBack = new CameraTakePicture();
			myIsInPreview=true;
			
			 initTex();
			    mSTexture = new SurfaceTexture ( hTex[0] );
			    mSTexture.setOnFrameAvailableListener(this);
			    myCurrentCamera = Camera.open();
			    
			    //création de la callback
			  /*  if(myPreviewBuffer == null)
				{
			    	Camera.Parameters params=myCurrentCamera.getParameters();
					myPreviewSize=params.getPreviewSize();
					myPreviewBuffer=new byte[myPreviewSize.width * myPreviewSize.height * 4];
					myCurrentCamera.addCallbackBuffer (myPreviewBuffer);
					myCurrentCamera.setPreviewCallbackWithBuffer(myCallback);
				}*/
			    
			    
			    
			    try {
			    	myCurrentCamera.setPreviewTexture(mSTexture);
			    } catch ( IOException ioe ) {
			    }
		}
	}

	public static void initCameraManager()
	{
		if(myCurrentCameraManager == null)
		{
			myCurrentCameraManager=new CameraManager();
		}
		
		//myCurrentCamera=Camera.open();
		
		if(myCurrentCamera != null)
		{
			Camera.Parameters params=myCurrentCamera.getParameters();
			myPreviewSize=params.getPreviewSize();
				
			myPreviewFormat=params.getPreviewFormat();
			
			kigsmainActivity activity=(kigsmainActivity)kigsmainActivity.getContext();
			KigsGLSurfaceView surface=activity.getSurfaceView();
			myView=surface;			
		//	myView.getHolder().setFormat(ImageFormat.YUY2);
			myIsInPreview=false;
			
			/*
			try {
				myCurrentCamera.setPreviewDisplay(myView.getSurfaceHolder());
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			*/
			
		
		}
	}
	 private static void initTex() {
		    hTex = new int[1];
		    GLES20.glGenTextures ( 1, hTex, 0 );
		    GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, hTex[0]);
		    GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
		    GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
		    GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
		    GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
		  }
	public	static int	getCameraPreviewW()
	{
		if(myCurrentCamera != null)
		{
			return myPreviewSize.width;
		}
		return -1;
	}
	
	public	static int	getCameraPreviewH()
	{
		if(myCurrentCamera != null)
		{
			return myPreviewSize.height;
		}
		return -1;
	}

	
	public	static int	getCameraPreviewFormat()
	{
		if(myCurrentCamera != null)
		{
			return myPreviewFormat;
		}
		return -1;
	}
	
	public	static int	askCameraPreviewFormat(int format)
	{
		if(myCurrentCamera != null)
		{
			Camera.Parameters params=myCurrentCamera.getParameters();
			params.setPreviewFormat(format);
			try
			{
				myCurrentCamera.setParameters(params);
			}
			catch(RuntimeException badparam)
			{
				return -1;
			}
			
			params=myCurrentCamera.getParameters();
			myPreviewFormat=params.getPreviewFormat();
			return myPreviewFormat;
			
		}
		return -1;
	}
	
	
	public	static void	start()
	{
		//Log.i("INFO DEBUG", "CAMERA start");
		if((myCurrentCamera!=null) && (myIsInPreview==false))
		{
			/*try {
				myCurrentCamera.setPreviewDisplay(myView.getSurfaceHolder());
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}*/
			// set buffer if not done before
			if(myPreviewBuffer == null)
			{
				myPreviewBuffer=new byte[myPreviewSize.width * myPreviewSize.height * 4];
				myCurrentCamera.addCallbackBuffer (myPreviewBuffer);
				myCurrentCamera.setPreviewCallbackWithBuffer(myCallback);
			}
			
			
			myCurrentCamera.startPreview();
			
			
		}
		
	}
	
	public	static void	stop()
	{
		//Log.i("INFO DEBUG", "CAMERA stop");
		if((myCurrentCamera!=null) && (myIsInPreview==true))
		{
			myCurrentCamera.stopPreview();
			myIsInPreview=false;
		}
	}
	
	public	static void	pause()
	{
		//Log.i("INFO DEBUG", "CAMERA pause");
		myWasPausedInPreview=false;
		if(myCurrentCamera!=null)
		{
			if(myIsInPreview == true)
			{
				myWasPausedInPreview=true;
				stop();
			}
			myCurrentCamera.setPreviewCallbackWithBuffer(null);
			myCurrentCamera.release();
		}
	}
	
	public	static void	resume()
	{
		//Log.i("INFO DEBUG", "CAMERA Resume");

		if(myCurrentCamera!=null)
		{
			myCurrentCamera=Camera.open();

			myIsInPreview=false;
			
			
			if(myPreviewBuffer == null)
			{
				myPreviewBuffer=new byte[myPreviewSize.width * myPreviewSize.height * 4];
			}
			myCurrentCamera.addCallbackBuffer (myPreviewBuffer);
			myCurrentCamera.setPreviewCallbackWithBuffer(myCallback);
			if(myCurrentCamera!=null)
			{
				if(myWasPausedInPreview == true)
				{
					start();
				}
			}
		}
	}
	
	public	static void	takePicture()
	{
		if((myCurrentCamera!=null) && (myIsInPreview==true) && (myTakePictureCallBack != null))
		{	
			Camera.Size Size =  myCurrentCamera.getParameters().getPictureSize();
			
			myCurrentCamera.getParameters().setPictureFormat(17);
			
			myTakePictureCallBack.Set_SizeX(Size.width);
			myTakePictureCallBack.Set_SizeY(Size.height);
			
			myCurrentCamera.takePicture(null, null, myTakePictureCallBack);
		}
	}
	
	public static void getFocal()
	{
		if((myCurrentCamera!=null) && (myIsInPreview==true))
		{
			
			float FocalLenght = myCurrentCamera.getParameters().getFocalLength();		
			CameraManager.nativeSetCameraFocalToKigs(FocalLenght);
		}
	}
	
	public static void SetCameraPreviewSize(int _sizeX, int _sizeY)
	{
		if (myCurrentCamera != null)
		{
			Camera.Parameters params=myCurrentCamera.getParameters();
			params.setPreviewSize(_sizeX, _sizeY);
			myCurrentCamera.setParameters(params);
			
			params=myCurrentCamera.getParameters();
			myPreviewSize = params.getPreviewSize();
			
			// realloc byte array at next start
			if(myPreviewBuffer != null)
			{
				myPreviewBuffer=null;
			}
			
			Log.i("INFO DEBUG", "SET CAMERA PREVIEW SIZE = " + myPreviewSize.width + " // " + myPreviewSize.height);
		}
	}
	private static native void  CameraCallback(int width, int height,byte[] pixels);
	private static native void 	nativeSetPictureToKigs(byte[] data, int SizeX, int SizeY);
	private static native void  nativeSetCameraFocalToKigs(float _focalLenght);

	@Override
	public void onFrameAvailable(SurfaceTexture surfaceTexture) {
		// TODO Auto-generated method stub
		
	}

	
	
}
