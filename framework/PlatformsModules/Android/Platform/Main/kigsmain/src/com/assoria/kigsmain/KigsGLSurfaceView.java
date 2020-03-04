package com.assoria.kigsmain;

import java.util.ArrayList;
import java.util.Vector;

import com.assoria.input.KigsGeolocation;
import com.assoria.input.KigsTouchEvent;
import com.assoria.input.KigsTouchEventList;
import com.assoria.input.TouchPoint;
import com.assoria.kigsmain.KigsRenderer.ActivityState;


import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

import android.util.Log;

import javax.microedition.khronos.egl.*;


public class KigsGLSurfaceView extends GLSurfaceView {

	static final int MAXTOUCH = 5;
	TouchPoint[] mTouchs;


	public class MyConfigChooser implements GLSurfaceView.EGLConfigChooser {
    @Override
    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
        int attribs[] = {
            EGL10.EGL_LEVEL, 0,
            EGL10.EGL_RENDERABLE_TYPE, 4,  // EGL_OPENGL_ES2_BIT
            EGL10.EGL_COLOR_BUFFER_TYPE, EGL10.EGL_RGB_BUFFER,
            EGL10.EGL_RED_SIZE, 8,
            EGL10.EGL_GREEN_SIZE, 8,
            EGL10.EGL_BLUE_SIZE, 8,
            EGL10.EGL_DEPTH_SIZE, 16,
			EGL10.EGL_STENCIL_SIZE, 8,
            EGL10.EGL_SAMPLE_BUFFERS, 0,
            EGL10.EGL_NONE
        };

		//@TODO Get nb of stencil bits required from renderingscreen
        
        int[] configCounts = new int[1];

		// retreive matching configs
		egl.eglChooseConfig(display, attribs, null, 0, configCounts);

		EGLConfig[] configs = new EGLConfig[configCounts[0]];


        egl.eglChooseConfig(display, attribs, configs, configCounts[0], configCounts);

		setPreserveEGLContextOnPause(true);
		
        if (configCounts[0] == 0) {
            // Failed! Error handling.
            return null;
        } else {
            return configs[0];
        }
    }
}
	public KigsGLSurfaceView(Context context)
	{
		super(context);
		synchronized(KigsRenderer.mState)
		{
			mMainActivity = (kigsmainActivity) context;
			
			mTouchs = new TouchPoint[MAXTOUCH];
			myTouchEventList = new KigsTouchEventList[MAXTOUCH];
			mEventBuffer = new ArrayList<ArrayList<KigsTouchEvent>>();
	        for (int i=0;i<MAXTOUCH;i++)
	        {
	        	mTouchs[i] = new TouchPoint();
	        	myTouchEventList[i] = new KigsTouchEventList();
	        	mEventBuffer.add(new ArrayList<KigsTouchEvent>()); 
	        }
					
			KigsMainManager.SetSurfaceView(this);
			
			setEGLContextClientVersion(context.getResources().getInteger(kigsmainActivity.getResourceIdByName(context.getPackageName(), "integer", "GLESVersion")));
			//setEGLConfigChooser(8,8,8,0,16,0);
			setEGLConfigChooser(new MyConfigChooser());
			mRenderer = new KigsRenderer();
			setRenderer(mRenderer);

			// receive events
			setFocusable(true);
			setFocusableInTouchMode(true);
			requestFocus();
		}	
	}
	
	public	SurfaceHolder getSurfaceHolder()
	{
		return this.getHolder();
	}
	
	public void SoftPause()
	{
		synchronized(KigsRenderer.mState)
		{
			if(KigsRenderer.mState == ActivityState.Play)
			{
				//Log.i("kigsPrintf","call SoftPause KigsMainManager.Pause();");
				KigsMainManager.Pause();
				KigsRenderer.setState(ActivityState.Pause);
			}
		}
	}
	
	public void	SoftResume()
	{
		synchronized(KigsRenderer.mState)
		{
			if(KigsRenderer.mState != ActivityState.Play)
			{
				//Log.i("kigsPrintf","call SoftResume KigsMainManager.Resume();");
				KigsMainManager.Resume(false);
				KigsRenderer.setState(ActivityState.Play);
			}
		}
	}
	
	@Override
	public void onPause()
	{
		//Log.i("kigsPrintf","KigsGLSurfaceView super on pause");
		super.onPause();
		//Log.i("kigsPrintf","KigsGLSurfaceView on pause wait synchro");
		synchronized(KigsRenderer.mState)
		{
			//Log.i("kigsPrintf","KigsGLSurfaceView  synchro ok");
			if(KigsRenderer.mState == ActivityState.Play)
			{
				//Log.i("kigsPrintf","call onPause KigsMainManager.Pause();");
				KigsMainManager.Pause();
				//Log.i("kigsPrintf","end call onPause KigsMainManager.Pause();");
				KigsRenderer.setState(ActivityState.Pause);
			}
		}
		//Log.i("kigsPrintf","KigsGLSurfaceView on pause finished");
		
	}
	
	public void onStop()
	{
		synchronized(KigsRenderer.mState)
		{
			if(KigsRenderer.mState != ActivityState.Pause)
			{
				//Log.i("kigsPrintf","call onStop KigsMainManager.Pause();");
				KigsMainManager.Pause();
				KigsRenderer.setState(ActivityState.Pause);
				
			}
		}
		//Log.i("kigsPrintf","call onStop super.onPause();");
		super.onPause();
	}
	
	public void onDestroy()
	{
		synchronized(KigsRenderer.mState)
		{
			//Log.i("kigsPrintf","call onDestroy Close;");
			KigsRenderer.setState(ActivityState.Destroy);
			//KigsMainManager.Close();
		}
	}

	//@Override
	//public void onResume()
	//{
	//	super.onResume();
	//}
	
/*	@Override
	public boolean onTouchEvent(final MotionEvent event) 
	{	
		synchronized(mEventBuffer)
		{
			// check that we moved from previous pos before queuing the move event
			if (event.getAction() == MotionEvent.ACTION_MOVE) 
			{
				if ((event.getX() != mLastX) || (event.getY() != mLastY)) 
				{
					mLastX=event.getX();
					mLastY=event.getY();

					mEventBuffer.add(event);
				}
			} 
			else if ((event.getAction() == MotionEvent.ACTION_DOWN)
					|| (event.getAction() == MotionEvent.ACTION_UP)) 
			{

				mLastX=event.getX();
				mLastY=event.getY();

				mEventBuffer.add(event);
			}
		}	
		
		try {
			Thread.sleep(15,0);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        return true;
    }*/
	
		void storeTouch(int index)
		{
			synchronized(mEventBuffer)
			{
				mEventBuffer.get(index).add(new KigsTouchEvent(1-mTouchs[index].getState(),mTouchs[index].getX(),mTouchs[index].getY()));
			}			
		}
	

	   int getTouchIndex(int pointerId)
	    {
	    	int firstNotAssigned = -1;
	    	for (int i=0;i<5;i++)
	    	{
	    		if (firstNotAssigned==-1 && mTouchs[i].getId()==-1) firstNotAssigned = i;
	    		if (mTouchs[i].getId()==pointerId) return i;
	    	}
	    	return firstNotAssigned;
	    }
	
	
	   @Override
	    public boolean onTouchEvent(MotionEvent ev) {
	        final int action = ev.getAction();
	        
			switch (action & MotionEvent.ACTION_MASK) {
	            case MotionEvent.ACTION_DOWN:
	            	//mTouchs[0].down(ev.getX(),ev.getY(),ev.getPointerId(0));
	            	//storeTouch(0);
	                //break;
	                
	            case MotionEvent.ACTION_POINTER_DOWN:
	            {
	            	int pointerIndex = (ev.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
	        		int pointerId = ev.getPointerId(pointerIndex);
	        		pointerIndex = getTouchIndex(pointerId);
					//Log.i("TEST", pointerIndex + "touch is down");
	        		if (pointerIndex!=-1)
	        		{
	        			mTouchs[pointerIndex].down(ev.getX(pointerIndex),ev.getY(pointerIndex),pointerId);
	        			storeTouch(pointerIndex);
	        		}
	            }
				break;

	            case MotionEvent.ACTION_MOVE:
	            	for (int i=0;i<Math.min(MAXTOUCH,ev.getPointerCount());i++)
	            	{
	            		final int id = ev.getPointerId(i);
	            		final int idx = getTouchIndex(id);
	            		if (idx!=-1)
	            		{
	            			if (mTouchs[idx].update(ev.getX(i),ev.getY(i)))
	            			{
	            				storeTouch(idx);
	            			}
	            		}
	            	}
	                break;
			

	            case MotionEvent.ACTION_UP:
	            case MotionEvent.ACTION_CANCEL:
	            case MotionEvent.ACTION_POINTER_UP: {
	                int pointerIndex = (ev.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
	                int pointerId = ev.getPointerId(pointerIndex);
	                pointerIndex = this.getTouchIndex(pointerId);
					//Log.i("TEST", pointerIndex + "touch is up");
	                if (pointerIndex!=-1)
	                {
	                	mTouchs[pointerIndex].up();
	                	storeTouch(pointerIndex);
	                }
	            }
	                break;	            
			}
			
			try {
				Thread.sleep(15,0);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
	        return true;
	    }	
	
	
	static public KigsTouchEventList	getTouchList(int index)
	{
		return myTouchEventList[index];
	}
	
	static public int getMaxTouch()
	{
		return MAXTOUCH;
	}

	public void Update() {
		synchronized (mEventBuffer)
		{
			for (int i=0;i<MAXTOUCH;i++)
			{
				ArrayList<KigsTouchEvent> l = mEventBuffer.get(i);
				if(!l.isEmpty()) {
					for (KigsTouchEvent event : l) {
						myTouchEventList[i].addEvent(event);
					}
					l.clear();
				}
			}
		}
	}

	static public KigsTouchEventList[]	myTouchEventList;
	
	ArrayList<ArrayList<KigsTouchEvent>>	mEventBuffer;
	
	KigsRenderer 			mRenderer;
	kigsmainActivity 		mMainActivity;
	float					mLastX,mLastY;	
}
