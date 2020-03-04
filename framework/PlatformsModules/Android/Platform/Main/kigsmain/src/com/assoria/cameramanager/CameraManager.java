package com.assoria.cameramanager;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.List;

import com.assoria.kigsmain.KigsGLSurfaceView;
import com.assoria.kigsmain.kigsmainActivity;

import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.media.CamcorderProfile;
import android.media.MediaRecorder;
import android.media.MediaScannerConnection;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.os.Environment;
import android.os.FileObserver;
import android.util.Log;

public class CameraManager implements SurfaceTexture.OnFrameAvailableListener {

	protected static CameraPreviewHandler myCallback = null;
	protected static CameraTakePicture myTakePictureCallBack = null;
	protected static CameraManager myCurrentCameraManager = null;
	protected static MediaRecorder myRecorder = null;
	protected static String myRecordPath = "";

	protected static Camera myCurrentCamera = null;
	protected static KigsGLSurfaceView myView = null;
	protected static Size myPreviewSize;
	protected static int myPreviewFormat;
	protected static boolean myIsInPreview = false;
	protected static boolean myWasPausedInPreview = false;
	protected static boolean myIsRecording = false;
	protected static List<String> mySupportedWhiteBalance;

	private static int[] hTex;
	private static SurfaceTexture mSTexture;

	public class CameraPreviewHandler implements Camera.PreviewCallback {

		public void onPreviewFrame(byte[] arg0, Camera arg1) {
			//Camera.Parameters params = myCurrentCamera.getParameters();
			//Log.i("White Balance", params.getWhiteBalance());
			//Log.i("Exposure", "comp : " + params.getExposureCompensation() + " min " + params.getMinExposureCompensation() + " max " + params.getMaxExposureCompensation());

			synchronized(myCurrentCamera)
			{
				CameraManager.CameraCallback(myPreviewSize.width, myPreviewSize.height, arg0);
				//Log.i("INFO DEBUG", "CameraCallback = " + myPreviewSize.width + " // " + myPreviewSize.height);
				arg1.addCallbackBuffer(arg0);
			}
		}

	}

	public class CameraTakePicture implements Camera.PictureCallback {

		protected int mySizeX = 0;
		protected int mySizeY = 0;

		public void onPictureTaken(byte[] data, Camera camera) {

			if (data != null) {
				//transform my byte array to Bitmap and convert this one in ARGB_8888 to send it later
				byte[] tempdata = data;
				Bitmap bm = BitmapFactory.decodeByteArray(tempdata, 0, tempdata.length);
				Bitmap bm8888 = bm.copy(Config.ARGB_8888, true);
				ByteBuffer buffer = ByteBuffer.allocate((mySizeX * mySizeY) * 4);
				bm8888.copyPixelsToBuffer(buffer);

				//call C++ method with data array in param
				CameraManager.nativeSetPictureToKigs(buffer.array(), mySizeX, mySizeY);

				//Log.i("DEBUG INFO",""+ mySizeX +"   //   " + mySizeY);
			} else {
				Log.i("DEBUG INFO", "onPictureTaken data is NULL");
			}
		}

		public void Set_SizeX(int _value) {
			mySizeX = _value;
		}

		public void Set_SizeY(int _value) {
			mySizeY = _value;
		}
	}

	private static Size myDefaultSize;

	CameraManager() {
		if (myCallback == null) {

			myCallback = new CameraPreviewHandler();
			myTakePictureCallBack = new CameraTakePicture();
			myRecorder = new MediaRecorder();

			myIsInPreview = true;

			initTex();
			mSTexture = new SurfaceTexture(hTex[0]);
			mSTexture.setOnFrameAvailableListener(this);
			myCurrentCamera = Camera.open();

			Camera.Parameters params = myCurrentCamera.getParameters();

			List<Size> sizes = params.getSupportedPreviewSizes();

			for(Size size : sizes)
			{
				Log.i("Size", "" + size.width + "x" + size.height);
			}

			myDefaultSize = params.getPreviewSize();

			mySupportedWhiteBalance = params.getSupportedWhiteBalance();
			/*for (String elem : mySupportedWhiteBalance)
			{
			    Log.i("White Balance", elem);
			}*/
			if (mySupportedWhiteBalance.size() > 0)
				params.setWhiteBalance(mySupportedWhiteBalance.get(0));

			List<int[]> fps_ranges = params.getSupportedPreviewFpsRange();
			params.setPreviewFpsRange(fps_ranges.get(0)[1], fps_ranges.get(0)[1]);

			Log.i("FPS Range", "min: " + fps_ranges.get(0)[0] + " max: " + fps_ranges.get(0)[1]);

			//params.setSceneMode(Camera.Parameters.SCENE_MODE_SPORTS);

			Log.i("kigs camera params", params.flatten());

			myCurrentCamera.setParameters(params);

			myCurrentCamera.setPreviewCallbackWithBuffer(myCallback);

			try {
				myCurrentCamera.setPreviewTexture(mSTexture);
			} catch (IOException ioe) {
			}
		}
	}

	public static void initCameraManager() {
		if (myCurrentCameraManager == null) {
			myCurrentCameraManager = new CameraManager();
		}

		if (myCurrentCamera != null) {
			Camera.Parameters params = myCurrentCamera.getParameters();

			myPreviewSize = params.getPreviewSize();
			myPreviewFormat = params.getPreviewFormat();

			kigsmainActivity activity = (kigsmainActivity) kigsmainActivity.getContext();
			myView = activity.getSurfaceView();
			myIsInPreview = false;
		}
	}

	private static void initTex() {
		hTex = new int[1];
		GLES20.glGenTextures(1, hTex, 0);
		GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, hTex[0]);
		GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
		GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
		GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
		GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
	}

	public static int getCameraPreviewW() {
		if (myCurrentCamera != null) {
			return myPreviewSize.width;
		}
		return -1;
	}

	public static int getCameraPreviewH() {
		if (myCurrentCamera != null) {
			return myPreviewSize.height;
		}
		return -1;
	}

	public static int getCameraPreviewBytePerPix() {
		if (myCurrentCamera != null) {
			return ImageFormat.getBitsPerPixel(myCurrentCamera.getParameters().getPreviewFormat());
		}
		return -1;
	}

	public static int getCameraPreviewFormat() {
		if (myCurrentCamera != null) {
			return myPreviewFormat;
		}
		return -1;
	}

	public static void setCameraBuffer(byte[] arr) {
		if (myCurrentCamera != null) {
			myCurrentCamera.addCallbackBuffer(arr);
		}
	}

	public static int askCameraPreviewFormat(int format) {
		Camera.Parameters params = myCurrentCamera.getParameters();
		List<Integer> list = params.getSupportedPreviewFormats();

		// set format if its a valid one, or take the first one
		if (list.contains(format)) {
			params.setPreviewFormat(format);
			try {
				myCurrentCamera.setParameters(params);
			} catch (RuntimeException badparam) {
				return -1;
			}
		} else
			params.setPreviewFormat(list.get(0));

		params = myCurrentCamera.getParameters();
		myPreviewFormat = params.getPreviewFormat();
		return myPreviewFormat;
	}

	public static void start() {
		if ((myCurrentCamera != null) && (myIsInPreview == false)) {
			myCurrentCamera.startPreview();
			myCurrentCamera.setPreviewCallbackWithBuffer(myCallback);
			myIsInPreview = true;
		}
	}

	public static void stop() {
		synchronized(myCurrentCamera)
		{
			//Log.i("INFO DEBUG", "CAMERA stop");
			if ((myCurrentCamera != null) && (myIsInPreview == true)) {
				myCurrentCamera.stopPreview();
				myIsInPreview = false;
			}
		}
	}

	public static void pause() {
		//Log.i("INFO DEBUG", "CAMERA pause");
		myWasPausedInPreview = false;
		if (myCurrentCamera != null) {
			if (myIsInPreview == true) {
				myWasPausedInPreview = true;
				stop();
			}

			myCurrentCamera.setPreviewCallbackWithBuffer(null);
			myCurrentCamera.release();
		}
	}

	public static void resume() {
		//Log.i("INFO DEBUG", "CAMERA Resume");

		if (myCurrentCamera != null) {
			myCurrentCamera = Camera.open();

			myIsInPreview = false;

			myCurrentCamera.setPreviewCallbackWithBuffer(myCallback);

			if (myCurrentCamera != null) {
				if (myWasPausedInPreview == true) {
					start();
				}
			}
		}
	}

	public static boolean startRecording(String path) {
		if (myCurrentCamera != null && !myIsRecording) {

			myIsRecording = true;
			myCurrentCamera.unlock();
			myRecorder.setCamera(myCurrentCamera);

			myRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
			myRecorder.setAudioSource(MediaRecorder.AudioSource.CAMCORDER);
			myRecorder.setProfile(
					CamcorderProfile.get(Camera.CameraInfo.CAMERA_FACING_BACK, CamcorderProfile.QUALITY_HIGH));

			//myRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);

			String doc = Environment.getExternalStorageDirectory().getPath();
			myRecordPath = doc + File.separator + path;

			Log.i("INFO DEBUG", "Recording to " + myRecordPath);
			myRecorder.setOutputFile(myRecordPath);

			//pause();
			try {
				myRecorder.prepare();
				myRecorder.start();

				myCurrentCamera.setPreviewCallbackWithBuffer(myCallback);

			} catch (IOException e) {
				Log.i("ERROR", "Cannot start video recording");
				myIsRecording = false;
				myRecorder.reset();
				try {
					myCurrentCamera.reconnect();
				} catch (IOException e1) {
				}

				return false;
			}
			return true;
		}
		return false;
	}

	public static void stopRecording() {
		if (myCurrentCamera != null && myIsRecording) {
			Log.i("INFO DEBUG", "Stopping recording");
			myIsRecording = false;

			FileObserver fo = new FileObserver(myRecordPath, FileObserver.CLOSE_WRITE) {
				@Override
				public void onEvent(int event, String path) {
					if (event == CLOSE_WRITE) {
						MediaScannerConnection.scanFile(kigsmainActivity.getContext(), new String[] { myRecordPath },
								null, null);
						stopWatching();
					}
				}
			};
			fo.startWatching();

			myRecorder.stop();
			myRecorder.reset();

			try {
				myCurrentCamera.reconnect();
				myCurrentCamera.setPreviewCallbackWithBuffer(myCallback);
			} catch (IOException e) {
				Log.i("ERROR", "Could not reconnect camera");
			}
		}
	}

	public static void takePicture() {
		if ((myCurrentCamera != null) && (myIsInPreview == true) && (myTakePictureCallBack != null)) {
			Camera.Size Size = myCurrentCamera.getParameters().getPictureSize();

			myCurrentCamera.getParameters().setPictureFormat(17);

			myTakePictureCallBack.Set_SizeX(Size.width);
			myTakePictureCallBack.Set_SizeY(Size.height);

			myCurrentCamera.takePicture(null, null, myTakePictureCallBack);
		}
	}

	public static float getFOV() {
		if (myCurrentCamera == null)
			return 0.0f;

		return myCurrentCamera.getParameters().getVerticalViewAngle();
	}

	public static void setWhiteBalance(int i) {
		if (myCurrentCamera != null && i >= 0 && i < mySupportedWhiteBalance.size()) {
			Camera.Parameters params = myCurrentCamera.getParameters();
			params.setWhiteBalance(mySupportedWhiteBalance.get(i));
			myCurrentCamera.setParameters(params);
		}
	}

	public static int getSupportedWhiteBalanceCount() {
		return mySupportedWhiteBalance.size();
	}

	public static void getFocal() {
		if ((myCurrentCamera != null) && (myIsInPreview == true)) {

			float FocalLenght = myCurrentCamera.getParameters().getFocalLength();
			CameraManager.nativeSetCameraFocalToKigs(FocalLenght);
		}
	}

	public static void SetCameraPreviewSize(int _sizeX, int _sizeY, boolean AllowHigherValue) {
		if (myCurrentCamera != null) {
			Camera.Parameters params = myCurrentCamera.getParameters();

			if(_sizeX == 0 || _sizeY == 0){
				myPreviewSize = myDefaultSize;
			}
			else{
				List<Size> list = params.getSupportedPreviewSizes();
				myPreviewSize = list.get(0);
				int delta = Math.abs((myPreviewSize.width - _sizeX) + (myPreviewSize.height - _sizeY));

				for (Size s : list) {
					// remove higher values if not allowed
					if(!AllowHigherValue && (s.width >_sizeX|| s.height >_sizeY))
						continue;

					int ndelta = Math.abs((s.width - _sizeX) + (s.height - _sizeY));
					if (ndelta < delta) {
						myPreviewSize = s;
						delta = ndelta;
					}

					// check _sizeX for matching the ration as well
					if (ndelta == 0 && s.width == _sizeX)
						break;
				}
			}
			params.setPreviewSize(myPreviewSize.width, myPreviewSize.height);
			myCurrentCamera.setParameters(params);

			//params = myCurrentCamera.getParameters();
			//myPreviewSize = params.getPreviewSize();

			Log.i("INFO DEBUG", "SET CAMERA PREVIEW SIZE = " + myPreviewSize.width + " // " + myPreviewSize.height);
		}
	}

	private static native void CameraCallback(int width, int height, byte[] pixels);

	private static native void nativeSetPictureToKigs(byte[] data, int SizeX, int SizeY);

	private static native void nativeSetCameraFocalToKigs(float _focalLenght);

	@Override
	public void onFrameAvailable(SurfaceTexture surfaceTexture) {
		Log.i("INFO DEBUG", "frame available");
		//CameraCallback(myPreviewSize.width, myPreviewSize.height, myPreviewBuffer);
		/*myView.queueEvent(new Runnable() {
			@Override
			public void run() {
				//GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, hTex[0]);
				//Size r = myPreviewSize;
		
				//byte[] buf = new byte[r.width * r.height * 4];
				//ByteBuffer pixels = ByteBuffer.wrap(myPreviewBuffer);
		
				//GLES20.glReadPixels(0, 0, r.width, r.height, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, pixels);
		
				CameraCallback(myPreviewSize.width, myPreviewSize.height, myPreviewBuffer);// pixels.array());
			}
		});*/

	}

}
