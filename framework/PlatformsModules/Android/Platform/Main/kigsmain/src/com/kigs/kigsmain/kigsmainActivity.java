
package com.kigs.kigsmain;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.media.AudioManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Bundle;
import android.os.PowerManager;
import android.os.Build;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
import android.media.MediaPlayer;
import android.media.RingtoneManager;
import android.media.Ringtone;
import android.os.Vibrator;
import java.io.File;

import android.content.pm.ConfigurationInfo;

//import com.kigs.cameramanager.CameraManager;
import com.kigs.filemanager.FileManager;
import com.kigs.httprequest.AsyncHTTPRequest;
import com.kigs.imageLoader.imageLoader;
import com.kigs.input.KigsAccelerometer;
import com.kigs.input.KigsGeolocation;
import com.kigs.input.KigsGyroscope;
import com.kigs.input.KigsKeyboard;
import com.kigs.soundmanager.AudioBuffer;

// market
/*
import com.android.vending.licensing.AESObfuscator;
import com.android.vending.licensing.LicenseChecker;
import com.android.vending.licensing.LicenseCheckerCallback;
import com.android.vending.licensing.ServerManagedPolicy;*/
import com.kigs.sqlite.KigsDB;

public class kigsmainActivity extends Activity {

	public static final boolean DoCheckLicense = false;
	private static final int SHOP_MESSAGE = 2;
	private static final int VIBRATOR_MESSAGE = 3;

	public static int getResourceIdByName(String packageName, String className, String name) {
		Class r = null;
		int id = 0;
		try {
			r = Class.forName(packageName + ".R");

			Class[] classes = r.getClasses();
			Class desireClass = null;

			for (int i = 0; i < classes.length; i++) {
				if (classes[i].getName().split("\\$")[1].equals(className)) {
					desireClass = classes[i];

					break;
				}
			}

			if (desireClass != null) {
				id = desireClass.getField(name).getInt(desireClass);
			}

		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		}

		return id;
	}

	//private static final String BASE64_PUBLIC_KEY = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAgYIa1fiPKrfQDHlNRumUwXd1LoRbwb4iFShhxmEXet4ogcaxrUjuakb84DT193sta2kw7Z+5DqffoomD+rl6r+nI7bnIBwMdeYi0IBTvjpSa6wuOkVQhBLlt5HDkfn9t599MV1//8nkSlarh2cJ8YpWbIPRH7MRo/g60vpQhcByh6m3+R86OrkaPA/hBhW33Z8IVe6V2fqn0eq+RYeTifyera1TNJMTk+1Cu4Ddc8RYkC6HL9rx3Qwk+okwnzxEqnI1qdttaJZlDJhNDjoeN1akmQ3s6sd9f14cpV8e0i5+ZcYZgQEidU/SPpl/OUvqh+0ipmm1cjscwYF5hS+rBYQIDAQAB";

	// Generate your own 20 random bytes, and put them here.
	/*private static final byte[] SALT = new byte[] {
	    -24, 32, 25, -32, -120, 80, -12, -54, 14, 124, -74, 56, -77, -87, -21, -53, -99, 45, -14, 79
	};
	
	private LicenseCheckerCallback 	mLicenseCheckerCallback;
	private LicenseChecker 			mChecker;*/
	// A handler on the UI thread.
	//  private Handler mHandler;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		KigsMainManager.LoadDll(this);

		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

		setContentView(getResourceIdByName(getPackageName(), "layout", "main"));
		//mHandler = new Handler();

		/*if(DoCheckLicense)
		{    	       
			// Try to use more data here. ANDROID_ID is a single point of attack.
		    String deviceId = Secure.getString(getContentResolver(), Secure.ANDROID_ID);
		
		    // Library calls this when it's done.
		    mLicenseCheckerCallback = new MyLicenseCheckerCallback();
		    // Construct the LicenseChecker with a policy.
		    mChecker = new LicenseChecker(
		        this, new ServerManagedPolicy(this,
		            new AESObfuscator(SALT, getPackageName(), deviceId)),
		        BASE64_PUBLIC_KEY);
		}*/
		//Log.i("kigsPrintf","lets go");

		myExitConfirmed = false;
		myCurrentActivity = this;

		FileManager.initFileManager(getAssets(), this);

		imageLoader.initImageLoader(getAssets(), this);

		KigsDB.initKigsDB();
		AsyncHTTPRequest.initAsyncHTTPRequest();

		AudioBuffer.Init(getAssets());

		//Log.i("kigsPrintf","Just to activate logs");

		this.setVolumeControlStream(AudioManager.STREAM_MUSIC);


		ActivityManager am = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        Log.i("kigs", "opengl es " + info.reqGlEsVersion);

		synchronized (KigsRenderer.mState) {
			myKeyBackPressed = false;
			myKeyMenuPressed = false;

			myHasAccelerometer = KigsAccelerometer.isSupported();
			myHasGyroscope = KigsGyroscope.isSupported();

			LockPowerManager();

			if (mAlreadyInit) {
				if (mGLView != null) {
					mGLView.onPause();
				}
				mGLView = null;
				endCheck();
			} else {

				// check only if first init (if check failed, mAlreadyInit is not set to true
				doCheck();
			}
		}
	}

	protected void endCheck() {
		mAlreadyInit = true;

		mGLView = new KigsGLSurfaceView(this);
		setContentView(mGLView);
	}

	private void doCheck() {
		endCheck();
	}

	@Override
	protected void onPause() {
		super.onPause();

		//		CameraManager.pause();

		if (mAlreadyInit) {
			//Log.i("kigsPrintf","onPause");
			//Log.i("kigsPrintf","onPause mGLView.onPause()");
			mGLView.onPause();
			if (myHasAccelerometer)
				KigsAccelerometer.stopListening(true);

			if (myHasGyroscope)
				KigsGyroscope.stopListening(true);

			KigsGeolocation.stopListening(true);
			//Log.i("kigsPrintf","onPause ReleasePowerManager()");
			ReleasePowerManager();
			//Log.i("kigsPrintf","onPause super.onPause()");
		}
	}

	@Override
	protected void onStop() {
		if (mAlreadyInit) {
			ReleasePowerManager();
			mGLView.onStop();
		}
		super.onStop();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		if (mAlreadyInit) {
			//Log.i("kigsPrintf","onDestroy");
			if (myHasAccelerometer)
				KigsAccelerometer.stopListening(false);

			if (myHasGyroscope)
				KigsGyroscope.stopListening(false);

			KigsGeolocation.stopListening(false);
			ReleasePowerManager();
			mGLView.onDestroy();
			if (myExitConfirmed) {
				AudioBuffer.Close();
			}
		}
		if (myExitConfirmed) {
			System.runFinalizersOnExit(true);
			System.exit(0);
		}
	}

	@Override
	protected void onResume() {
		//Log.i("kigsPrintf","onResume");
		super.onResume();

		if (mAlreadyInit) {
			if (myHasAccelerometer)
				KigsAccelerometer.Resume();

			if (myHasGyroscope)
				KigsGyroscope.Resume();

			KigsGeolocation.Resume();
			LockPowerManager();
			mGLView.onResume();
		}
		//		CameraManager.resume();
	}

	// back key management
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			myKeyBackPressed = true;
			return true;
		} else if (keyCode == KeyEvent.KEYCODE_MENU) {
			myKeyMenuPressed = true;
			return true;
		}

		return super.onKeyDown(keyCode, event);
	}

	public boolean onKeyUp(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			myKeyBackPressed = false;
			return true;
		} else if (keyCode == KeyEvent.KEYCODE_MENU) {
			myKeyMenuPressed = false;
			return true;
		}

		return super.onKeyUp(keyCode, event);
	}

	static public void askExit() {
		myExitConfirmed = true;
		kigsmainActivity.myCurrentActivity.finish();
	}

	static public void SendMessage(int mtype, int Params) {
		if (mtype == SHOP_MESSAGE) {
			Intent marketIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(
					/*   "http://market.android.com/details?id=" + kigsmainActivity.myCurrentActivity.getPackageName()));*/
					"tstore://PRODUCT_VIEW/0000221700/0"));
			kigsmainActivity.myCurrentActivity.startActivity(marketIntent);  
			kigsmainActivity.myCurrentActivity.finish();
		}
		if (mtype == VIBRATOR_MESSAGE) {
			kigsmainActivity.myCurrentActivity.vibrate(Params);
			kigsmainActivity.myCurrentActivity.playNotification();
			//kigsmainActivity.myCurrentActivity.playSound();
		}
	}

	static public void OpenLink(String a_link) {
		Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(a_link));
		kigsmainActivity.myCurrentActivity.startActivity(browserIntent);
	}

	static public boolean CheckConnexion() {
		ConnectivityManager cm = (ConnectivityManager) kigsmainActivity.getContext()
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo netInfo = cm.getActiveNetworkInfo();

		return (netInfo != null && netInfo.isConnectedOrConnecting());
	}

	public static Context getContext() {
		return myCurrentActivity;
	}

	public static void LockPowerManager() {
		if (myNoSleepWakeLock != null) {
			if (!myNoSleepWakeLock.isHeld())
				try {
					myNoSleepWakeLock.acquire();
				} catch (Exception e) {
					e.printStackTrace();
				}

		}
	}

	public static void ReleasePowerManager() {
		if (myNoSleepWakeLock != null) { 
			if (myNoSleepWakeLock.isHeld())
				try {
					myNoSleepWakeLock.release();
				} catch (Exception e) {
					e.printStackTrace();
				}

		}
	}

	public static boolean GetBackKeyState() {
		return myKeyBackPressed;
	}

	public static boolean GetMenuKeyState() {
		return myKeyMenuPressed;
	}

	public KigsGLSurfaceView getSurfaceView() {
		return mGLView;
	}

	public void playNotification() {
		Uri notification = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
		Ringtone r = RingtoneManager.getRingtone(myCurrentActivity.getApplicationContext(), notification);
		r.play();
	}

	/*public void playSound() { 
		try{
		Uri myUri = Uri.fromFile(new File("//assets/Sound/sample.mp3"));
		//Uri myUri = Uri.parse("file:///android_asset/Sound/sample.mp3");
		MediaPlayer mediaPlayer = new MediaPlayer();
		mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
		mediaPlayer.setDataSource(myCurrentActivity.getApplicationContext(), myUri);
		mediaPlayer.prepare();
		mediaPlayer.start();
		}
		catch(Exception e)
		{
		}
	}*/

	public void vibrate(int mslen) {
		Vibrator mVibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);

		if (mVibrator != null && mVibrator.hasVibrator()) { 
			mVibrator.vibrate(mslen);
		}
	}
		
	static public int osVersionCode() {
		return android.os.Build.VERSION.SDK_INT;
	}

	static public String osVersion() {
		return android.os.Build.BOARD;
	}
	
	private KigsGLSurfaceView mGLView;

	private static boolean mAlreadyInit = false;
	static private kigsmainActivity myCurrentActivity;

	static private boolean myHasAccelerometer = false;
	static private boolean myHasGyroscope = false;

	static private PowerManager myPowerManager = null;
	static private PowerManager.WakeLock myNoSleepWakeLock = null;

	static public boolean myKeyBackPressed = false;
	static public boolean myKeyMenuPressed = false;

	static public boolean myExitConfirmed = false;

	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		KigsKeyboard.dispatchKeyEvent(event);
		return super.dispatchKeyEvent(event);
	}

}
