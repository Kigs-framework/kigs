#include "..\..\..\Headers\CustomUI\SequenceMap\UIMap.h"
#include "..\..\..\Headers\CustomUI\SequenceMap\PointInteretVignette.h"
#include "..\..\..\Headers\CustomUI\SequenceMap\UIPointInteret.h"
#include "..\..\..\Headers\UserPositionGPS\UserPositionGPS.h"

#include "NotificationCenter.h"
#include "ModuleInput.h"
#include "GeolocationDevice.h"
#include "CoreBaseApplication.h"

using namespace Kigs::Gps;
using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(UIMap);

UIMap::UIMap(const std::string& name, CLASS_NAME_TREE_ARG) :
	UIImage(name, PASS_CLASS_NAME_TREE_ARG)
	, mCoordGPSTopLeftPoint_Lat(*this, false, "GPSTopLeftPoint_Lat", 0)
	, mCoordGPSTopLeftPoint_Long(*this, false, "GPSTopLeftPoint_Long", 0)
	, mCoordGPSBottomRightPoint_Lat(*this, false, "GPSBottomRightPoint_Lat", 0)
	, mCoordGPSBottomRightPoint_Long(*this, false, "GPSBottomRightPoint_Long", 0)
	, mUserGPS_Lat(*this, false, "GPSUser_Lat", 0)
	, mUserGPS_Long(*this, false, "GPSUser_Long", 0)
	, mClickControl(*this, false, "ClickControl", true)
	, mUser(nullptr)
	, mUserImage(*this, false, "User","")
	, mGPS(NULL)
	, mCompas(NULL)
	, mHasClick(false)
{
}

void UIMap::Update(const Timer& a_Timer, void* addParam)
{
	UIImage::Update(a_Timer, addParam);

	if (mCompas)
	{
		Quaternion q(0, 0, 0, 1);
		mCompas->getRotationQuaternion(q.V.x, q.V.y, q.V.z, q.w);
		q.Normalize();

		if (q.w != 0.0)
		{
			Vector3D view(0, 0, -1);
			view = q*view;
			Vector3D view_proj = ProjectOnPlane(view, Point3D(0, 0, 0), Vector3D(0, 0, 1));
			view_proj.Normalize();
			float angle = acos(Dot(Vector3D(1,0,0), view_proj));
			Vector3D side_vector;
			side_vector.CrossProduct(Vector3D(1, 0, 0), view_proj);
			if (Dot(Vector3D(0, 0, 1), side_vector) < 0) angle = -angle;
			mUser->SetDirection(fPI/2.0f - angle);
		}
	}

	if (mClickControl)
		return;

	if (mGPS == nullptr)
		return;

	double lat, lon;
	mGPS->GetCoord(lat, lon);
	ChangeUserCoord(lat, lon);
}

void UIMap::NotifyUpdate(const unsigned int 	labelid)
{
	if ((labelid == mUserGPS_Lat.getLabelID()) || (labelid == mUserGPS_Long.getLabelID()))
	{
		ChangeUserCoord(mUserGPS_Lat, mUserGPS_Long);
	}
	else if (labelid == mUserImage.getLabelID())
	{
		mUser->setValue("User", mUserImage);
	}
	else if (labelid == mClickControl.getLabelID())
	{
		if( mGPS == nullptr )
		{
			auto input = KigsCore::GetModule<Input::ModuleInput>();
			mGPS = input->GetGeolocation();
		}

		if (mClickControl)
		{
			if (mGPS)
				mGPS->Release();
		}
		else
		{
			if (mGPS)
			{
				mGPS->setValue("Rate", 1000);
				mGPS->setValue("MinDistance", 1.0f);
				mGPS->Aquire();
			}
		}
	}
	else
	{
		Node2D::NotifyUpdate(labelid);
	}
}

void UIMap::InitModifiable()
{
	UIImage::InitModifiable();

	if (!_isInit)
	{
		UninitModifiable();
		return;
	}

	mUser = KigsCore::GetInstanceOf(getName() + "_myUser", "UserPositionGPS");
	mUser->setValue("Map", getName());
	mUser->setValue("User", mUserImage);
	addItem((CMSP&)mUser);
	

	mUserGPS_Lat.changeNotificationLevel(Owner);
	mUserGPS_Long.changeNotificationLevel(Owner);
	mUserImage.changeNotificationLevel(Owner);
	mClickControl.changeNotificationLevel(Owner);

	mPointsOfInterest.clear();

	auto input = KigsCore::GetModule<Input::ModuleInput>();
	mGPS = input->GetGeolocation();
	mCompas = input->GetGyroscope();
	if (mCompas)
	{
		mCompas->setValue("Rate", 0);
		mCompas->Aquire();
	}
	if (mGPS)
	{
		mGPS->setValue("Rate", 100);
		mGPS->setValue("MinDistance", 0.5f);
		mGPS->Aquire();	
	}

	// retreive direct touch for visual hover / push ...
	input->getTouchManager()->registerEvent(this, "ManageDirectTouchEvent", DirectTouch, EmptyFlag);
}

void UIMap::AddPointOfInterest(UIPointInteret *_PI)
{
	mPointsOfInterest.push_back(_PI);
}

void UIMap::ActivatePointInteret(UIPointInteret *_PI)
{
	KigsCore::GetNotificationCenter()->postNotificationName("HideVignette", nullptr, this);
	_PI->ActivePoint(true);
}

void UIMap::GetPositionsGPS(double & TLLong, double &TLLat, double &BRLong, double &BRLat)
{
	TLLong = mCoordGPSTopLeftPoint_Long;
	TLLat = mCoordGPSTopLeftPoint_Lat;
	BRLong = mCoordGPSBottomRightPoint_Long;
	BRLat = mCoordGPSBottomRightPoint_Lat;
}

UIMap::~UIMap()
{
	if (mGPS)
		mGPS->Release();

	if (mCompas)
		mCompas->Release();
}

void UIMap::ChangeUserCoord(double lat, double lon)
{
	if (mUser->SetCoordGPS(lat, lon))
	{
		if (mHasClick && mGPS)
		{
			mGPS->setValue("Latitude", lat); 
			mGPS->setValue("Longitude", lon);
		}

		for (int i = 0; i < mPointsOfInterest.size(); i++)
		{
			mPointsOfInterest[i]->ActivePoint(lat, lon);
		}
	}
}

bool UIMap::ManageDirectTouchEvent(DirectTouchEvent& direct_touch)
{
	if (!mClickControl)
		return false;

	bool allow = CanInteract(direct_touch.position.xy);
	if (direct_touch.state == StatePossible) // check for hover
	{
		return allow;
	}

	if (direct_touch.state == StateBegan)
	{
		double Rx, Ry, lon, lat;
		double topLeftLong = 0, botRightLong = 0, topLeftLat = 0, botRightLat = 0;
		switch (direct_touch.touch_state)
		{
		case DirectTouchEvent::TouchHover:
			break;
		case DirectTouchEvent::TouchDown:
			mHasClick = true;
			GetPositionsGPS(topLeftLong, topLeftLat, botRightLong, botRightLat);

			mMousePosX = direct_touch.position.x;
			mMousePosY = direct_touch.position.y;

			 Rx = (double)mMousePosX / (double)mSize[0];
			 Ry = (double)mMousePosX / (double)mSize[1];

			 lon = (botRightLong - topLeftLong)*Rx + topLeftLong;
			 lat = (botRightLat - topLeftLat)*Ry + topLeftLat;

			ChangeUserCoord(lat, lon);
			break;
		case DirectTouchEvent::TouchUp:
			mHasClick = false;
			break;
		}
	}
	else if (direct_touch.state == StateChanged)
	{

		if (direct_touch.touch_state == DirectTouchEvent::TouchHover)
		{
			if (mHasClick)
			{
				mMousePosX = direct_touch.position.x;
				mMousePosY = direct_touch.position.y;

				double Rx, Ry, lon, lat;
				double topLeftLong = 0, botRightLong = 0, topLeftLat = 0, botRightLat = 0;
				GetPositionsGPS(topLeftLong, topLeftLat, botRightLong, botRightLat);

				Rx = (double)mMousePosX / (double)mSize[0];
				Ry = (double)mMousePosY / (double)mSize[1];

				lon = (botRightLong - topLeftLong)*Rx + topLeftLong;
				lat = (botRightLat - topLeftLat)*Ry + topLeftLat;

				ChangeUserCoord(lat, lon);
			}
			// hover moved
			/**direct_touch.swallow_mask |= (
			(1 << InputEventType::DirectTouch) |
			(1 << InputEventType::Click) |
			(1 << InputEventType::Swipe)
			);*/
			*direct_touch.swallow_mask = 0xFFFFFFFF;
		}
		else
		{
			// should not happend
		}
		//	printf("direct touch changed");
	}
	else if (direct_touch.state == StateEnded)
	{
		if (direct_touch.touch_state == DirectTouchEvent::TouchHover)
		{
		}
		else
		{
			// should not happend
		}
		//	printf("direct touch ended");
	}
	return false;
}


/*bool UIMap::TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)
{
	mHasClick = false;
	if (myIsEnabled == false) // mouse click catch
		return false;

	if (!mClickControl)
		return false;

	double topLeftLong = 0, botRightLong = 0, topLeftLat = 0, botRightLat = 0;
	GetPositionsGPS(topLeftLong, topLeftLat, botRightLong, botRightLat);

	mMousePosX = X;
	mMousePosY = Y;

	double Rx = (double)X / (double)mySizeX;
	double Ry = (double)Y / (double)mySizeY;

	double lon = (botRightLong - topLeftLong)*Rx + topLeftLong;
	double lat = (botRightLat - topLeftLat)*Ry + topLeftLat;

	ChangeUserCoord(lat, lon);

	bool down = buttonState&UIInputEvent::LEFT;
	catchClick = down;
	mHasClick = down;

	return false; // dispatch event on POI
}
*/

/*bool UIMap::TriggerMouseMove(bool over, float _MouseDeltaX, float _MouseDeltaY)
{
	if (mHasClick)
	{
		mMousePosX += _MouseDeltaX;
		mMousePosY += _MouseDeltaY;

		double topLeftLong = 0, botRightLong = 0, topLeftLat = 0, botRightLat = 0;
		GetPositionsGPS(topLeftLong, topLeftLat, botRightLong, botRightLat);

		double Rx = (double)mMousePosX / (double)mySizeX;
		double Ry = (double)mMousePosY / (double)mySizeY;

		double lon = (botRightLong - topLeftLong)*Rx + topLeftLong;
		double lat = (botRightLat - topLeftLat)*Ry + topLeftLat;

		ChangeUserCoord(lat, lon);
	}
	return false;
}*/