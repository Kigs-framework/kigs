package com.kigs.kigsmain;

import android.content.Context;

// manage JNI calls
public class KigsMainManager {
	
	public static void LoadDll(Context a_context)
	{
		
		System.loadLibrary(a_context.getResources().getString(kigsmainActivity.getResourceIdByName(a_context.getPackageName(), "string", "SO_Name")));
    }
	
	//public static native boolean isARMv7();
	
	public static native void  Init();
	public static native void  Pause();
	public static native void  Resume(boolean reinitTexture);
	public static native void  GLResize(int w,int h);
	public static native void  Update();
	public static native void  Close();
	public static native boolean  NeedExit();

	public static KigsGLSurfaceView mSurfaceView;
	public static void SetSurfaceView(KigsGLSurfaceView sv){mSurfaceView = sv; }
	public static void UpdateSurfaceView(){mSurfaceView.Update();}
	
}
