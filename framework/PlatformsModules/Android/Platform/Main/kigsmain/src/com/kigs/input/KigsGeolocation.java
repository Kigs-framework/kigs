package com.kigs.input;


import java.util.HashMap;

import com.kigs.kigsmain.kigsmainActivity;

import android.content.Context;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
 

public class KigsGeolocation {
    /** indicates whether or not Geolocation is running */
    private static boolean running = false;
    private static boolean activated = false;
    private static LocationManager mLocationManager = null;
    
    private  static	double mLatitude;
    private  static	double mLongitude;
    private  static	double mAltitude;
    private  static	float mAccuracy;
    private	 static HashMap<String,Integer> mProviderState = new HashMap<String, Integer>();

	public static long mMinTime;
	public static float	mMinDistance;
    
    
    public static double getLatitude(){return mLatitude;}
    public static double getLongitude(){return mLongitude;}
    public static double getAltitude(){return mAltitude;}
    public static float getAccuracy(){return mAccuracy;}
    public static boolean isActive(){return activated;}
    
    
    public static void Activate(boolean activate, int minTime, float minDistance)
    {
		mMinTime = minTime;
		mMinDistance = minDistance;

   	   	if (activate!=activated) 
    	{
    		activated = activate;

	    	Handler mainHandler = new Handler(Looper.getMainLooper());
	    	
	    	Runnable myRunnable = new Runnable()
	    	{
	    		@Override 
	    		public void run() 
	    		{
	         		if (activated)
	        		{
	        			startListening();
	        		}
	        		else
	        		{
	        			stopListening(false);
	        		}
	    	   }
	    	};
	    	mainHandler.post(myRunnable);
    	}
    }
 
    /**
     * Returns true if the manager is listening to orientation changes
     */
    public static boolean isListening() {
        return running;
    }
 
    /**
     * Unregisters listeners
     */
    public static void stopListening(boolean onlyPaused)  {
    	if (running)
    	{
	        running = onlyPaused;
	        if (mLocationManager!=null) mLocationManager.removeUpdates(mListener);
    	}
    }

	public static void Resume() 
	{
		if(running)
		{
			running = false;
			startListening();
		}
	}
 
 
    /**
     * Registers a listener and start listening
     */
    public static void startListening() {
    	if (activated && !running)
    	{	
	    	mLocationManager = (LocationManager) kigsmainActivity.getContext().getSystemService(Context.LOCATION_SERVICE);
	 
	    	if (mLocationManager.isProviderEnabled(LocationManager.GPS_PROVIDER))
	    	{
	    		try
	    		{
	    			mLocationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, mMinTime, mMinDistance, mListener);
	    		}
	    		catch(Exception e)
	    		{
	    			Log.e("LM",e.getLocalizedMessage());
	    		}
	    	} 
	    	try {
	    		mLocationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, mMinTime, mMinDistance, mListener);  
	    	}
	    	catch(Exception e)
	    	{
	    		Log.e("LM",e.getLocalizedMessage());
	    	}
	    	running = true;
    	}
    }
    private static LocationListener mListener = new LocationListener ()
    {
		@Override
		public void onLocationChanged(Location location) {
			mLatitude = location.getLatitude();
			mLongitude = location.getLongitude();
			mAltitude = location.getAltitude();
			mAccuracy = location.getAccuracy();
		}

		@Override
		public void onStatusChanged(String provider, int status, Bundle extras) {
			mProviderState.put(provider,status);
		}

		@Override
		public void onProviderEnabled(String provider) {
		}

		@Override
		public void onProviderDisabled(String provider) {
		}    	
    }; 
}