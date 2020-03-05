package com.kigs.cameramanager;


import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Looper;
import android.os.Handler;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import com.kigs.kigsmain.kigsmainActivity;

import java.nio.ByteBuffer;
import java.util.LinkedList;
import java.util.List;


public class CameraManager2 {

    private static android.hardware.camera2.CameraManager mCameraManager=null;
    private static String mCurrentCameraID=null;
    private static CameraDevice mCurrentCamera=null;
    private static CameraCaptureSession mCaptureSession=null;
    private static CameraCharacteristics mCurrentCameraCharacteristics=null;
    private static Size mStreamSize=null;
    private static int mStreamFormat=-1;

    private static ImageReader mImageReader=null;
    private static byte[] mBuffer=null;

    private static LooperThread mLooper=null;

    private static native void CameraCallback(int width, int height, byte[] pixels);

    CameraManager2() {
    }

    public static void initCameraManager() {
        Log.i("kigs", "initCameraManager()");
        Context context = kigsmainActivity.getContext();
        mCameraManager = (android.hardware.camera2.CameraManager) context.getSystemService(Context.CAMERA_SERVICE);

        mCurrentCameraID = getCameraId(mCameraManager, CameraCharacteristics.LENS_FACING_BACK);

        try {
            mLooper = new LooperThread();
            mLooper.start();

            mCameraManager.openCamera(mCurrentCameraID, mStateCallback, mLooper.mHandler);
        }
        catch (Exception e){e.printStackTrace();}
    }

    public static boolean start(byte[] b)
    {
        while (mCurrentCamera==null)
        {
            return false;
        }

        mBuffer = b;
        Log.i("kigs", "Camera up and running");

            try {

                // create and init ImageReader
                mImageReader = ImageReader.newInstance(mStreamSize.getWidth(), mStreamSize.getHeight(), mStreamFormat, 3);
                mImageReader.setOnImageAvailableListener(new ImageReader.OnImageAvailableListener() {
                    @Override
                    public void onImageAvailable(ImageReader reader) {
                        Image image = null;
                        try {
                            image = mImageReader.acquireLatestImage();
                            final Image.Plane[] planes = image.getPlanes();
                            final ByteBuffer buffer = planes[0].getBuffer();
                            final byte[] data = new byte[buffer.capacity()];
                            buffer.get(data);

                            CameraCallback(mStreamSize.getWidth(), mStreamSize.getHeight(), data);

                        } catch (Exception e) {
                            if (image != null)
                                image.close();
                        }
                    }
                }, mLooper.mHandler);

                List<Surface> outputSurfaces = new LinkedList<>();
                outputSurfaces.add(mImageReader.getSurface());
                mCurrentCamera.createCaptureSession(outputSurfaces, mCaptureStateCallback, mLooper.mHandler);
            } catch (Exception e) {
                e.printStackTrace();
            }

        return true;
    }

    public static void stop()
    {
        if (mCurrentCamera!=null)
        {
            mCurrentCamera.close();
        }
    }

    public static String getCameraId(android.hardware.camera2.CameraManager aCM, int aMode){
        try {
            for (final String cameraId : aCM.getCameraIdList()) {
                CameraCharacteristics characteristics = aCM.getCameraCharacteristics(cameraId);
                int cOrientation = characteristics.get(CameraCharacteristics.LENS_FACING);
                if (cOrientation == aMode)
                {
                    mCurrentCameraCharacteristics = characteristics;
                    return cameraId;
                }
            }
        }
        catch (Exception e){e.printStackTrace();}

        return null;
    }

    public static void SelectFormatAndSize(int _format, int _sizeX, int _sizeY) {
        if (mCurrentCameraCharacteristics != null) {
            StreamConfigurationMap configs = mCurrentCameraCharacteristics.get(
                    CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);

            //Log.i("kigs", configs.toString());

            mStreamFormat = -1;
            for (int f:configs.getOutputFormats()) {
                if(f==_format)
                    mStreamFormat = f;
            }

            // take default format if not found
            if(mStreamFormat==-1)
                mStreamFormat = ImageFormat.YUV_420_888;

            int bestDelta=_sizeX+_sizeY;
            Size bestSize =new Size(0,0);
            for (Size s:configs.getOutputSizes(mStreamFormat))
            {
                int delta = (_sizeX-s.getWidth()) + (_sizeY-s.getHeight());
                if(bestDelta>delta)
                {
                    bestDelta = delta;
                    bestSize = s;

                    if(bestDelta==0)
                        break;
                }
            }

            mStreamSize = bestSize;
        }

        Log.i("INFO DEBUG", "SET CAMERA PREVIEW SIZE = " + mStreamSize.getWidth() + "x" + mStreamSize.getHeight());
    }

    public static int getCameraPreviewW() {
        if (mStreamSize != null) {
            return mStreamSize.getWidth();
        }
        return -1;
    }

    public static int getCameraPreviewH() {
        if (mStreamSize != null) {
            return mStreamSize.getHeight();
        }
        return -1;
    }

    public static int getCameraPreviewFormat() {
        return mStreamFormat;
    }


    private static CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice camera) {
            Log.i("kigs", "onOpened");
            mCurrentCamera = camera;
        }

        @Override
        public void onDisconnected(CameraDevice camera) {
            Log.i("kigs", "onDisconnected");
        }

        @Override
        public void onError(CameraDevice camera, int error) {
            Log.i("kigs", "onError");
        }
    };

    static class LooperThread extends Thread {
        public Handler mHandler;

        @Override
        public void run() {
            // Initialize the current thread as a Looper
            // (this thread can have a MessageQueue now)
            Looper.prepare();

            mHandler = new Handler() {
            };

            // Run the message queue in this thread
            Looper.loop();
        }
    }

    private static CameraCaptureSession.StateCallback mCaptureStateCallback = new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(CameraCaptureSession session) {
            mCaptureSession = session;

            // create and init a CaptureRequest
            try {
                CaptureRequest.Builder captureBuilder = mCurrentCamera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                captureBuilder.addTarget(mImageReader.getSurface());
                CaptureRequest request = captureBuilder.build();
                mCaptureSession.setRepeatingRequest(request, null, null);
            }
            catch (Exception e) {e.printStackTrace();}
        }

        @Override
        public void onConfigureFailed(CameraCaptureSession session) {

        }
    };
}