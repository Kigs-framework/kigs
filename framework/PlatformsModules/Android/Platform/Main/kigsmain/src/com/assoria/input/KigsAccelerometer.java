package com.assoria.input;

import java.util.List;
import java.nio.*;

import com.assoria.kigsmain.kigsmainActivity;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

public class KigsAccelerometer 
{

	/** Accuracy configuration */
	private static Sensor sensor;
	private static SensorManager sensorManager;
	private static int rate = SensorManager.SENSOR_DELAY_NORMAL;

	/** indicates whether or not Accelerometer Sensor is supported */
	private static Boolean supported=null;
	private static boolean WorldCoordSupported=false;

	/** indicates whether or not Accelerometer Sensor is running */
	private static boolean running = false;

	// all values in the same array (avoid several jni call)
	// 3 raw accelerometer vals
	// 3 world coords accelerometer vals
	// 3 world coords integrated velocity vals
	// 3 world coords integrated position vals
	static ByteBuffer Values = ByteBuffer.allocate(12*4);

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
		if (!running)
			return;

		running = onlyPaused;
		try 
		{
			if (sensorManager != null && sensorEventListener != null) 
			{
				sensorManager.unregisterListener(sensorEventListener);
			}
		} 
		catch (Exception e) {}
	}

	/**
	 * Returns true if at least one Accelerometer sensor is available
	 */
	public static boolean isSupported() 
	{
		WorldCoordSupported=false;
		if (supported == null) 
		{	
			if (kigsmainActivity.getContext() != null) 
			{
				sensorManager = (SensorManager) kigsmainActivity.getContext().getSystemService(Context.SENSOR_SERVICE);
				List<Sensor> sensors = sensorManager.getSensorList(Sensor.TYPE_ACCELEROMETER);
				supported = Boolean.valueOf(sensors.size() > 0);
				
				WorldCoordSupported=supported.booleanValue();

				if(supported)
				{
					sensors = sensorManager.getSensorList(Sensor.TYPE_GRAVITY);
					WorldCoordSupported = WorldCoordSupported && (sensors.size() > 0);

					sensors = sensorManager.getSensorList(Sensor.TYPE_MAGNETIC_FIELD);
					WorldCoordSupported = WorldCoordSupported && (sensors.size() > 0);

				}
			} 
			else 
			{
				supported = Boolean.FALSE;
			}
		}

		return supported;
	}


	/**
	 * Returns true if at least one Accelerometer sensor is available
	 */
	public static boolean isWorldCoordinatesSupported() 
	{
		if (WorldCoordSupported == false) 
		{	
			if (kigsmainActivity.getContext() != null) 
			{
				sensorManager = (SensorManager) kigsmainActivity.getContext().getSystemService(Context.SENSOR_SERVICE);
				List<Sensor> sensors = sensorManager.getSensorList(Sensor.TYPE_ACCELEROMETER);
				WorldCoordSupported = (sensors.size() > 0);
				
				sensors = sensorManager.getSensorList(Sensor.TYPE_GRAVITY);
				WorldCoordSupported = WorldCoordSupported && (sensors.size() > 0);

				sensors = sensorManager.getSensorList(Sensor.TYPE_MAGNETIC_FIELD);
				WorldCoordSupported = WorldCoordSupported && (sensors.size() > 0);
				
			} 
			else 
			{
				WorldCoordSupported = false;
			}
		}

		return WorldCoordSupported;
	}

	/**
	 * Registers a listener and start listening
	 * @param accelerometerListener
	 *             callback for accelerometer events
	 */
	public static void startListening(int aRate) 
	{
		if (running)
			return;

		rate = aRate;
		Values.order(ByteOrder.nativeOrder());

		sensorManager = (SensorManager) kigsmainActivity.getContext().getSystemService(Context.SENSOR_SERVICE);

		List<Sensor> sensors = sensorManager.getSensorList(Sensor.TYPE_ACCELEROMETER);
		if (sensors.size() > 0) 
		{
			sensor = sensors.get(0);
			running = sensorManager.registerListener(sensorEventListener, sensor, rate);
		}

		sensors = sensorManager.getSensorList(Sensor.TYPE_GRAVITY);
		if (sensors.size() > 0) 
		{
			sensor = sensors.get(0);
			running = sensorManager.registerListener(sensorEventListener, sensor, rate);
		}

		sensors = sensorManager.getSensorList(Sensor.TYPE_MAGNETIC_FIELD);
		if (sensors.size() > 0) 
		{
			sensor = sensors.get(0);
			running = sensorManager.registerListener(sensorEventListener, sensor, rate);
		}
	}

	public static void Resume() 
	{
		if (running) 
		{
			running = false;
			startListening(rate);
		}
	}


	private static float[] gravityValues = null;
	private static float[] magneticValues = null;

	private static long timestamp=0;
	private static final float NS2S = 1.0f / 1000000000.0f;
	private static float absorb_time = 0.0f;
	
	private static float[] rawaccel = new float[3];
	private static float[] wcaccel = new float[3];

	private static float[] rawaccumul = new float[3];
	private static float[] wcaccumul = new float[3];

	private static float[] velocity = new float[3];
	private static float[] position = new float[3];

	private static float count;

	/**
	 * The listener that listen to events from the accelerometer listener
	 */
	public static SensorEventListener sensorEventListener = new SensorEventListener() 
	{

		public void onSensorChanged(SensorEvent event) 
		{
			if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER)
			{
				if(timestamp!=0)
				{
					final float dt = (event.timestamp - timestamp)*NS2S;
					float[] deviceRelativeAcceleration = new float[4];
					deviceRelativeAcceleration[0] = event.values[0];
					deviceRelativeAcceleration[1] = event.values[1];
					deviceRelativeAcceleration[2] = event.values[2];
					deviceRelativeAcceleration[3] = 0;

					if( (gravityValues != null) && (magneticValues != null) )
					{

						// Compute acceleration in a fixed basis
						float[] R = new float[16], I = new float[16], earthAcc = new float[4];
						SensorManager.getRotationMatrix(R, I, gravityValues, magneticValues);
						float[] inv = new float[16];
						android.opengl.Matrix.invertM(inv, 0, R, 0);
						android.opengl.Matrix.multiplyMV(earthAcc, 0, inv, 0, deviceRelativeAcceleration, 0);


						rawaccel[0]=event.values[0];
						rawaccel[1]=event.values[1];
						rawaccel[2]=event.values[2];

						count+=1.0;
	
						rawaccumul[0] += rawaccel[0];
						rawaccumul[1] += rawaccel[1];
						rawaccumul[2] += rawaccel[2];

						wcaccumul[0] += earthAcc[0];
						wcaccumul[1] += earthAcc[1];
						wcaccumul[2] += earthAcc[2];

						velocity[0] += 0.5f*(earthAcc[0] + wcaccel[0])*dt;
						velocity[1] += 0.5f*(earthAcc[1] + wcaccel[1])*dt;
						velocity[2] += 0.5f*(earthAcc[2] + wcaccel[2])*dt;

						wcaccel[0] = earthAcc[0];
						wcaccel[1] = earthAcc[1];
						wcaccel[2] = earthAcc[2];

						if(wcaccel[0]*wcaccel[0] + wcaccel[1]*wcaccel[1] < 1.0f)
						{
							absorb_time += dt;
						}
						else
						{
							absorb_time = 0.0f;
						}

						if(absorb_time > 0.33f)
						{
							velocity[0] = 0;
							velocity[1] = 0;
							velocity[2] = 0;
						}

						position[0] += velocity[0]*dt;
						position[1] += velocity[1]*dt;
						position[2] += velocity[2]*dt;
					}
					else // only raw values
					{
						rawaccel[0]=event.values[0];
						rawaccel[1]=event.values[1];
						rawaccel[2]=event.values[2];

						count+=1.0;
	
						rawaccumul[0] += rawaccel[0];
						rawaccumul[1] += rawaccel[1];
						rawaccumul[2] += rawaccel[2];

						velocity[0] = 0;
						velocity[1] = 0;
						velocity[2] = 0;

						position[0] = 0;
						position[1] = 0;
						position[2] = 0;
					}
				}
				else
				{
					rawaccel[0] = 0;
					rawaccel[1] = 0;
					rawaccel[2] = 0;

					rawaccumul[0] = 0;
					rawaccumul[1] = 0;
					rawaccumul[2] = 0;

					velocity[0] = 0;
					velocity[1] = 0;
					velocity[2] = 0;

					position[0] = 0;
					position[1] = 0;
					position[2] = 0;

					count = 0.0f;
				}
				timestamp = event.timestamp;

			}
			else if (running && event.sensor.getType() == Sensor.TYPE_GRAVITY) 
			{
				gravityValues = event.values;
				
			}
			else if (running && event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD) 
			{
				magneticValues = event.values;
				
			}
		}

		public void onAccuracyChanged(Sensor arg0, int arg1) 
		{
			// TODO Auto-generated method stub

		}

	};

	public static byte[] getValue() 
	{

		Values.clear();
		if(count > 0.0f)
		{
			Values.putFloat(rawaccumul[0]/count);
			Values.putFloat(rawaccumul[1]/count);
			Values.putFloat(rawaccumul[2]/count);
		}
		else
		{
			Values.putFloat(rawaccel[0]);
			Values.putFloat(rawaccel[1]);
			Values.putFloat(rawaccel[2]);
		}

		if(WorldCoordSupported==true)
		{
			if(count > 0.0f)
			{
				Values.putFloat(wcaccumul[0]/count);
				Values.putFloat(wcaccumul[1]/count);
				Values.putFloat(wcaccumul[2]/count);
			}
			else
			{
				Values.putFloat(wcaccel[0]);
				Values.putFloat(wcaccel[1]);
				Values.putFloat(wcaccel[2]);
			}

			Values.putFloat(velocity[0]);
			Values.putFloat(velocity[1]);
			Values.putFloat(velocity[2]);

			Values.putFloat(position[0]);
			Values.putFloat(position[1]);
			Values.putFloat(position[2]);

			wcaccumul[0] = 0;
			wcaccumul[1] = 0;
			wcaccumul[2] = 0;

		}
		else
		{
			Values.putFloat(0);
			Values.putFloat(0);
			Values.putFloat(0);

			Values.putFloat(0);
			Values.putFloat(0);
			Values.putFloat(0);

			Values.putFloat(0);
			Values.putFloat(0);
			Values.putFloat(0);
		}

		rawaccumul[0] = 0;
		rawaccumul[1] = 0;
		rawaccumul[2] = 0;


		count = 0.0f;


		return Values.array();
	}


}