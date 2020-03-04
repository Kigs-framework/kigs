
// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************


//
//  Created by steph on 11/15/10.
//  Copyright 2010 Assoria. All rights reserved.

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include <string>
#include <vector>
#import <CoreMotion/CoreMotion.h>
#import <CoreLocation/CoreLocation.h>
//#import "Macros.h"


extern EAGLSharegroup*	gShareGroup;
/****** Touch ******/
struct structCoord
{
    CGPoint mouseCoordinate;
    UITouchPhase phase;
    int tapCount;
    bool active;
    UITouch* Touch;
};

extern structCoord listCoordinate[5];
/*******************/


/****** Geolocalisation ******/
struct structGeolocation{
    double latitude;
    double longitude;
    double altitude;
    float accuracy;
};
extern structGeolocation currentGeo;

extern pthread_mutex_t	eventMutex;	

/****** resolution ******/
/*   true  -> 640*960   */
/*   false -> 320*480   */
extern bool highDefinition;
/************************/

// extern UIDeviceOrientation myOrientation;

/******* messages *******
extern struct structMessage {
	int type;
	int param;
};
extern structMessage mesMessage;
extern std::list<structMessage> listMessage;
/************************/

// used to sync between game and event threads

@interface IOSViewController :  UIViewController{


}

@end

extern UIWindow*		gApplicationWindow;

class CoreBaseApplication;

/*
@interface MainIPhoneAppDelegate : NSObject <UIApplicationDelegate, CLLocationManagerDelegate> {
    UIWindow*               window;
	UIResponder*			respond;
	EAGLContext *			myContext;
	bool					userTouchEnabled;
	CoreBaseApplication*	myApp;
	UIAccelerationValue     accelerometerValues[3];
    volatile bool           mybUpdating;
}*/

extern UIWindow*            window;
extern UIResponder*			respond;
extern EAGLContext *		myContext;
extern bool					userTouchEnabled;
extern CoreBaseApplication*	myApp;
extern UIAccelerationValue     accelerometerValues[3];
extern volatile bool           mybUpdating;
/*
@property (nonatomic, retain)  UIWindow *window;
- (void)manageMessage;
-(void) dealloc;
@end

*/


