#ifndef _UIMAP_H_
#define _UIMAP_H_

#include "UI\UIImage.h"
#include "maReference.h"
#include "GyroscopeDevice.h"
#include "TouchInputEventManager.h"
class GeolocationDevice;
class UserPositionGPS;
class UIPointInteret;

// ****************************************
// * UIMap class
// * --------------------------------------
/**
* \file	UIMap.h
* \class	UIMap
* \ingroup GeoLoc
* \brief	 An UIMap is a class for add a map
* \author	K�nig Jolan
* \version ukn
* \date	14/02/16
*
* The map is configured with 2 coordinates. Then the user can see his position on the map, and we can add some interest points.
*/
// ****************************************

class UIMap :
	public UIImage
{
public:
	DECLARE_CLASS_INFO(UIMap, UIImage, 2DLayers);

	UIMap(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void GetPositionsGPS(double &, double &, double &, double &);
	void GetPositionGPSUser(double & _Lat, double& _Long) { _Lat = mUserGPS_Lat; _Long = mUserGPS_Long; }

	void NotifyUpdate(const unsigned int /* labelid */)override;
	void InitModifiable()override;


	void AddPointOfInterest(UIPointInteret *_PI);
	void ActivatePointInteret(UIPointInteret *_PI);

protected:
	~UIMap()override;

	bool ManageDirectTouchEvent(DirectTouchEvent& direct_touch);
	WRAP_METHODS(ManageDirectTouchEvent);

	//bool TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)override;
	//bool TriggerMouseMove(bool over, float _MouseDeltaX, float _MouseDeltaY)override;
	void Update(const Timer& a_Timer, void* addParam)override;

	void ChangeUserCoord(double lat, double lon);

	SP<UserPositionGPS> mUser;
	maString mUserImage;

	maDouble mUserGPS_Lat;
	maDouble mUserGPS_Long;

	maDouble mCoordGPSTopLeftPoint_Lat;
	maDouble mCoordGPSTopLeftPoint_Long;

	maDouble mCoordGPSBottomRightPoint_Lat;
	maDouble mCoordGPSBottomRightPoint_Long;

	maBool mClickControl;

	kstl::vector<UIPointInteret*> mPointsOfInterest;

	GeolocationDevice* mGPS;
	GyroscopeDevice* mCompas;

	float mMousePosX;
	float mMousePosY;
	bool mHasClick;

};

#endif //_UIMAP_H_