package com.kigs.input;

import java.util.List;
import java.nio.*;

import com.kigs.kigsmain.kigsmainActivity;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

public class KigsCompass {

	/** Accuracy configuration */
	private static Sensor sensor;
	private static SensorManager sensorManager;
	private static int rate = SensorManager.SENSOR_DELAY_NORMAL;

	/** indicates whether or not Compass Sensor is supported */
	private static Boolean supported;
	/** indicates whether or not Compass Sensor is running */
	private static boolean running = false;

	static ByteBuffer Values = ByteBuffer.allocate(12);

	/**
	 * Returns true if the manager is listening to orientation changes
	 */
	public static boolean isListening() {
		return running;
	}

	/**
	 * Unregisters listeners
	 */
	public static void stopListening(boolean isPause) {
		if (!running)
			return;

		running = isPause;
		try {
			if (sensorManager != null && sensorEventListener != null) {
				sensorManager.unregisterListener(sensorEventListener);
			}
		} catch (Exception e) {
		}
	}

	/**
	 * Returns true if at least one Compass sensor is available
	 */
	public static boolean isSupported() {
		if (supported == null) {
			if (kigsmainActivity.getContext() != null) {
				sensorManager = (SensorManager) kigsmainActivity.getContext().getSystemService(Context.SENSOR_SERVICE);
				List<Sensor> sensors = sensorManager.getSensorList(Sensor.TYPE_ORIENTATION);
				supported = Boolean.valueOf(sensors.size() > 0);
			} else {
				supported = Boolean.FALSE;
			}
		}
		return supported;
	}

	/**
	 * Registers a listener and start listening
	 * @param CompassListener
	 *             callback for Compass events
	 */
	public static void startListening(int aRate) {
		if (running)
			return;

		rate = aRate;
		Values.order(ByteOrder.nativeOrder());

		sensorManager = (SensorManager) kigsmainActivity.getContext().getSystemService(Context.SENSOR_SERVICE);

		List<Sensor> sensors = sensorManager.getSensorList(Sensor.TYPE_ORIENTATION);
		if (sensors.size() > 0) {
			sensor = sensors.get(0);
			running = sensorManager.registerListener(sensorEventListener, sensor, rate);
		}
	}

	public static void Resume() {
		if (running) {
			running = false;
			startListening(rate);
		}
	}

	/**
	 * The listener that listen to events from the Compass listener
	 */
	public static SensorEventListener sensorEventListener = new SensorEventListener() {

		public void onSensorChanged(SensorEvent event) {
			Values.clear();
			Values.putFloat(event.values[0]);
			Values.putFloat(event.values[1]);
			Values.putFloat(event.values[2]);
		}

		public void onAccuracyChanged(Sensor arg0, int arg1) {
			// TODO Auto-generated method stub

		}

	};

	public static byte[] getValue() {
		return Values.array();
	}

}