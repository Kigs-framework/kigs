package com.assoria.input;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

import com.assoria.kigsmain.kigsmainActivity;

import java.util.List;
import java.nio.*;


public class KigsGyroscope
{
	private static float[] tmpArray = new float[4]; 

	private static final double EPSILON = 0.1f;


	/** Accuracy configuration */
	private static Sensor sensor;
	private static SensorManager sensorManager;
	private static int     rate=SensorManager.SENSOR_DELAY_NORMAL;

	/** indicates whether or not Gyroscope Sensor is supported */
	private static Boolean supported;
	/** indicates whether or not Gyroscope Sensor is running */
	private static boolean running = false;

	/**
	 * Returns true if the manager is listening to orientation changes
	 */
	public static boolean isListening() 
	{
		return running;
	}

	/**
	 * Unregisters listeners
	 */
	public static void stopListening(boolean onlyPaused)
	{
		if(!running)
			return;

		running = onlyPaused;
		try 
		{
			if (sensorManager != null && sensorEventListener != null) 
			{
				sensorManager.unregisterListener(sensorEventListener);
			}
		} catch (Exception e) {}
	}

	/**
	 * Returns true if at least one Accelerometer sensor is available
	 */
	public static boolean isSupported() 
	{
		if (supported == null) 
		{
			if (kigsmainActivity.getContext() != null) 
			{
				sensorManager = (SensorManager) kigsmainActivity.getContext().getSystemService(Context.SENSOR_SERVICE);
				List<Sensor> sensors = sensorManager.getSensorList(Sensor.TYPE_GYROSCOPE);
				supported = Boolean.valueOf(sensors.size() > 0);

				sensors = sensorManager.getSensorList(Sensor.TYPE_ROTATION_VECTOR);
				supported = supported && Boolean.valueOf(sensors.size() > 0);
			} 
			else 
			{
				supported = Boolean.FALSE;
			}
		}
		return supported;
	}

	public static void startListening(int aRate) 
	{
		if(running)
			return;

		rate = aRate;
		Quaternion.order(ByteOrder.nativeOrder());
		Velocity.order(ByteOrder.nativeOrder());

		sensorManager = (SensorManager) kigsmainActivity.getContext().getSystemService(Context.SENSOR_SERVICE);
		List<Sensor> sensors = sensorManager.getSensorList(Sensor.TYPE_ROTATION_VECTOR);
		if (sensors.size() > 0) 
		{
			sensor = sensors.get(0);
			running = sensorManager.registerListener(sensorEventListener, sensor, rate);       
		}

		sensors = sensorManager.getSensorList(Sensor.TYPE_GYROSCOPE);
		if (sensors.size() > 0) 
		{
			sensor = sensors.get(0);
			running = sensorManager.registerListener(sensorEventListener, sensor, rate);
		}
	}

	public static void Resume() 
	{
		if(running)
		{
			running = false;
			startListening(rate);
		}
	}


	/**
	 * The listener that listen to events from the Gyroscope listener
	 */
	public static SensorEventListener sensorEventListener = 
	new SensorEventListener() 
	{

		public void onSensorChanged(SensorEvent event) 
		{
			if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) 
			{
				SensorManager.getQuaternionFromVector(tmpArray, event.values);
				Quaternion.clear();
				Quaternion.putFloat(tmpArray[1]);
				Quaternion.putFloat(tmpArray[2]);
				Quaternion.putFloat(tmpArray[3]);
				Quaternion.putFloat(-tmpArray[0]);
			} 
			else if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE) 
			{
					// Process Gyroscope 
				float axisX = event.values[0];
				float axisY = event.values[1];
				float axisZ = event.values[2];
				Velocity.clear();
				Velocity.putFloat(axisX);
				Velocity.putFloat(axisY);
				Velocity.putFloat(axisZ);
			}
		}

		public void onAccuracyChanged(Sensor arg0, int arg1) 
		{
		}
	};

	static ByteBuffer Velocity = ByteBuffer.allocate(12);
	public static byte[] getVelocity(){ return Velocity.array(); }

	static ByteBuffer Quaternion = ByteBuffer.allocate(16);
	public static byte[] getQuaternion()
	{
		return Quaternion.array();
	}

}