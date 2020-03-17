
// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************

//
//  Created by steph on 11/15/10.
//  Copyright 2010 Assoria. All rights reserved.
//
//#define NO_THREAD
#import "IPhoneMain.h"

//! this file must define KIGS_APPLICATION_CLASS with the name of your application class
#include "KigsApplication.h"
#include <fstream>
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "OpenGLRenderingScreen.h"
UIWindow *gApplicationWindow;
EAGLSharegroup *gShareGroup;

/******** Touch ********/


structCoord listCoordinate[5];
/***********************/

/**** accelerometre ****/
Vector3D	newAcc;
/***********************/

/****** Geolocalisation ******/
structGeolocation currentGeo;


CMMotionManager* motionManager;
CLLocationManager* locationManager;

/******* threads *******/
pthread_mutex_t	eventMutex;
NSCondition* myCondition;
bool myPauseThread;
/***********************/

/****** resolution ******/
/*   true  -> 640*960   */
/*   false -> 320*480   */
bool highDefinition = false;
/************************/

/******* messages *******/
struct structMessage {
	int types;
	int param;
};
structMessage mesMessage;
std::list<structMessage> listMessage;
/************************/

bool update = false;

@implementation IOSViewController

- (id)init
{
    self = [super init];
    
    [[UIDevice currentDevice] setValue:[NSNumber numberWithInteger: UIInterfaceOrientationPortrait] forKey:@"orientation"];
    
    return self;
}
@end


@implementation MainIPhoneAppDelegate


@synthesize window;

#pragma mark -
#pragma mark Application lifecycle

#define ApplicationName(a) Stringify(a)
#define Stringify(a) #a

-(void) dealloc
{
    myContext = nil;
    gShareGroup = nil;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    mybUpdating = false;
    if([[UIScreen mainScreen] respondsToSelector:@selector(nativeBounds)])
        self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] nativeBounds]];
    else
        self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    [self.window makeKeyAndVisible];
    
    // Override point for customization after application launch.
    self.window.backgroundColor = [UIColor redColor];
    
    [UIApplication sharedApplication].idleTimerDisabled = YES;
    
    gShareGroup = [[EAGLSharegroup alloc] init];
    myContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1 sharegroup:gShareGroup];
    
    gApplicationWindow=window;
    
    //Hide navigator
    [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
    
    @autoreleasepool
    {
        //! First thing to do
        Core::Init(false);

		// no need to register app to factory
		DECLARE_CLASS_INFO_WITHOUT_FACTORY(KIGS_APPLICATION_CLASS, ApplicationName(KIGS_APPLICATION_CLASS));
		myApp = (CoreBaseApplication*)KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));
			
        myApp->InitApp(BASE_DATA_PATH,true);
        
        [window makeKeyAndVisible];
        
        CADisplayLink *aDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateGame)];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    //lock to portrait
    [[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationPortrait animated:NO];
    
    //
    //Motion Manager
    //
    motionManager = [[CMMotionManager alloc] init];
    motionManager.accelerometerUpdateInterval = 0.2;
    motionManager.gyroUpdateInterval = 0.2;
    [motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue] withHandler:^(CMAccelerometerData* accelerometerData, NSError* error)
    {
        [self outputAccelerationData:accelerometerData.acceleration];
#ifdef DEBUG
        if(error)
        {
            NSLog(@"%@", error);
        }
#endif
    }];

    //
    //Location Manager
    //
    locationManager = [[CLLocationManager alloc] init];
    locationManager.delegate = self;
    locationManager.desiredAccuracy = kCLLocationAccuracyBest;
    locationManager.distanceFilter = kCLDistanceFilterNone;
    
    
    if([locationManager respondsToSelector:@selector(requestWhenInUseAuthorization)])
    {
        [[UIApplication sharedApplication] sendAction:@selector(requestWhenInUseAuthorization) to:locationManager from:self forEvent:nil];
    }
    
    
    [locationManager startMonitoringSignificantLocationChanges];
    [locationManager startUpdatingLocation];
    
    update = true;
    
    return YES;
}

- (void) manageMessage
{
	std::list<structMessage>::iterator i;
	i = listMessage.begin();
	while (i != listMessage.end())
    {
		mesMessage = listMessage.front();
		switch (mesMessage.types)
        {
			case 2:
				// [self LaunchShop];
				break;
			default:
				break;
		}
		i = listMessage.erase(i);
	}
}

- (void)updateGame
{
    
    if(mybUpdating)
        return;
    mybUpdating = true;
    if(!myApp->NeedExit())
    {
        if(update)
            myApp->UpdateApp();
        mybUpdating = false;
    }
    else
    {
        myApp->CloseApp();
        myApp->Destroy();
        Core::Close();
        mybUpdating = false;
        exit(0);
    }
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
	/*
	 Methode appelée losqu'on passe de l'état actif a l'inactif (pause ...)
	 */
	//NSLog(@"applicationWillResignActive\n");
	myApp->Sleep();
	myPauseThread = true;
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    
    update = false;
    mybUpdating = false;
    structCoord L_initValues;
    L_initValues.Touch = NULL;
    L_initValues.active = false;
    L_initValues.mouseCoordinate = CGPointZero;
    L_initValues.tapCount = 0;
    
    //Touch Management
    for(int i = 0; i < 5; i++)
        listCoordinate[i] = L_initValues;
#ifdef DEBUG
    NSLog(@"application did enter background");
#endif
	
}


- (void)applicationWillEnterForeground:(UIApplication *)application
{
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
	/*
	 Methode appelée losque l'application passe de l'état de tache de fond a actif
	 */
	//[application UIApplicationState:UIApplicationStateActive];
#ifdef DEBUG
    NSLog(@"application will enter foreground");
#endif
}


- (void)applicationDidBecomeActive:(UIApplication *)application
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
	/*
	 Methode appelée losqu'on passe de l'état inactif a actif
	 */
    /*
	[myCondition lock];
	[myCondition signal];
	[myCondition unlock];
     */
    update = true;
#ifdef DEBUG
    NSLog(@"application did become active");
#endif
}

#pragma mark -
#pragma mark Memory management

-(void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}

-(void)outputAccelerationData:(CMAcceleration)acceleration
{
    newAcc.x = acceleration.x;
    newAcc.y = acceleration.y;
    newAcc.z = acceleration.z;
}

-(void) locationManager:(CLLocationManager*) manager didUpdateLocations:(NSArray *)locations
{
    CLLocation* newLocation = locations[[locations count] -1];
    currentGeo.latitude = newLocation.coordinate.latitude;
    currentGeo.longitude = newLocation.coordinate.longitude;
    currentGeo.altitude = newLocation.altitude;
    currentGeo.accuracy = newLocation.horizontalAccuracy;
}

- (NSUInteger)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window
{
    return UIInterfaceOrientationMaskPortrait;
}

- (void)application:(UIApplication*)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings
{
    [application registerForRemoteNotifications];
}

@end


