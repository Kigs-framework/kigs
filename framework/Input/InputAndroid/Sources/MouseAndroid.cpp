#include "MouseAndroid.h"
#include "Core.h"
#include "DeviceItem.h"
#include "NotificationCenter.h"
#include "MultiTouchDevice.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>

extern android_app* 			mAndroidApp;

using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(MouseAndroid)

IMPLEMENT_CONSTRUCTOR(MouseAndroid)
{
	mPosX = 0;
	mPosY = 0;	
   	mFrameCountSinceLastValidTouched = 1000; //Any big value
	
	KigsCore::GetNotificationCenter()->addObserver(this, "ReinitCB", "ResetContext");
}

MouseAndroid::~MouseAndroid()
{  

}    

bool	MouseAndroid::Aquire()
{
	if (MouseDevice::Aquire())
	{
		mPosX = 0;
		mPosY = 0;
		mFrameCountSinceLastValidTouched = 1000; //Any big value
		return true;
	}
	return false;
}

bool	MouseAndroid::Release()
{
	return MouseDevice::Release();
}

void	MouseAndroid::UpdateDevice()
{
	if (mMultiTouchPointer)
	{
		auto state = mMultiTouchPointer->getTouchState(0);
		float x, y;
		mMultiTouchPointer->getTouchPos(0, x, y);
		mDeviceItems[0]->getState()->SetValue((float)((int)x - (int)(kfloat)mPosX));
		mDeviceItems[1]->getState()->SetValue((float)((int)y - (int)(kfloat)mPosY));
		mDeviceItems[2]->getState()->SetValue(state);
	}
	else
	{
		auto *inputBuffer = android_app_swap_input_buffers(mAndroidApp);
		
		bool hasmoved=false;
		
		if (inputBuffer) 
		{
			// handle motion events (motionEventsCounts can be 0).
			for (auto i = 0; i < inputBuffer->motionEventsCount; i++) {
				auto &motionEvent = inputBuffer->motionEvents[i];
				auto action = motionEvent.action;

				// Find the pointer index, mask and bitshift to turn it into a readable value.
				auto pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
						>> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
						
				auto &pointer = motionEvent.pointers[pointerIndex];
				
				if( ( mCurrentActionPointer == (uint32_t)0xFFFFFFFF ) || ( pointer.id == mCurrentActionPointer ) )
				{
					mCurrentActionPointer = pointer.id;
					// get the x and y position of this event if it is not ACTION_MOVE.
					
					auto x = GameActivityPointerAxes_getX(&pointer);
					auto y = GameActivityPointerAxes_getY(&pointer);

					// determine the action type and process the event accordingly.
					switch (action & AMOTION_EVENT_ACTION_MASK) {
						case AMOTION_EVENT_ACTION_DOWN:
						case AMOTION_EVENT_ACTION_POINTER_DOWN:

							mDeviceItems[0]->getState()->SetValue(
									(float) (int) x - (int) (kfloat) mPosX);
							mDeviceItems[1]->getState()->SetValue(
									(float) (int) y - (int) (kfloat) mPosY);
							mDeviceItems[2]->getState()->SetValue((int) 0x80);
							hasmoved = true;
							break;

						case AMOTION_EVENT_ACTION_CANCEL:
							// treat the CANCEL as an UP event: doing nothing in the app, except
							// removing the pointer from the cache if pointers are locally saved.
							// code pass through on purpose.
						case AMOTION_EVENT_ACTION_UP:
						case AMOTION_EVENT_ACTION_POINTER_UP:

							if(mDeviceItems[2]->getState()->GetTypedValue(int)) {
								mDeviceItems[0]->getState()->SetValue(
										(float) (int) x - (int) (kfloat) mPosX);
								mDeviceItems[1]->getState()->SetValue(
										(float) (int) y - (int) (kfloat) mPosY);
								mDeviceItems[2]->getState()->SetValue((int) 0x00);
								mCurrentActionPointer = (uint32_t) 0xFFFFFFFF;
								hasmoved = true;
							}
							break;

						case AMOTION_EVENT_ACTION_MOVE:
							// There is no pointer index for ACTION_MOVE, only a snapshot of
							// all active pointers; app needs to cache previous active pointers
							// to figure out which ones are actually moved.
							for (auto index = 0; index < motionEvent.pointerCount; index++) {
								pointer = motionEvent.pointers[index];

								if (pointer.id == mCurrentActionPointer) {
									x = GameActivityPointerAxes_getX(&pointer);
									y = GameActivityPointerAxes_getY(&pointer);
									mDeviceItems[0]->getState()->SetValue(
											(float) (int) x - (int) (kfloat) mPosX);
									mDeviceItems[1]->getState()->SetValue(
											(float) (int) y - (int) (kfloat) mPosY);
									mDeviceItems[2]->getState()->SetValue((int) 0x80);
									hasmoved = true;
								}
							}

							break;
						default:
							break;
					}
				}
				
			}
			android_app_clear_motion_events(inputBuffer);
		}
		
		if(!hasmoved)
		{
			mDeviceItems[0]->getState()->SetValue((float)0);
			mDeviceItems[1]->getState()->SetValue((float)0);
		}
	}
	// call father update
	MouseDevice::UpdateDevice();
}

void	MouseAndroid::DoInputDeviceDescription()
{
	mMultiTouchPointer = (MultiTouchDevice*)mMultiTouch;

	mButtonsCount=1;
	mDeviceItemsCount=3; // 2 for posx and posy

	DeviceItem**	devicearray=new DeviceItem*[mDeviceItemsCount];
	
	int currentDevice=0;

	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(0.0f));
	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(0.0f));

	int currentButton;
	for(currentButton=0;currentButton<mButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(mDeviceItemsCount,devicearray);

	for(currentDevice=0;currentDevice<mDeviceItemsCount;currentDevice++)
	{
		delete devicearray[currentDevice];
	}

	delete[] devicearray;

}

DEFINE_METHOD(MouseAndroid, ReinitCB)
{
	printf("reinit MouseAndroid\n");
	if (mMultiTouchPointer) return false;
	
	return false;
}
