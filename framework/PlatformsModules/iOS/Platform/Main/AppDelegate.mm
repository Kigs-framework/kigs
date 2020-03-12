#import "AppDelegate.h"
#import "GameViewController.h"
#include "../../../../../framework/PlatformsModules/iOS/Platform/Main/KigsInterface.h"

@interface AppDelegate ()

@end

CMMotionManager*	motionManager;
CLLocationManager*	locationManager;
extern structGeolocation currentGeo;
extern float	newAcc[3];

@implementation AppDelegate


-(void)outputAccelerationData:(CMAcceleration)acceleration
{
    newAcc[0] = acceleration.x;
    newAcc[1] = acceleration.y;
    newAcc[2] = acceleration.z;
}

-(void) locationManager:(CLLocationManager*) manager didUpdateLocations:(NSArray *)locations
{
    CLLocation* newLocation = locations[[locations count] -1];
    currentGeo.latitude = newLocation.coordinate.latitude;
    currentGeo.longitude = newLocation.coordinate.longitude;
    currentGeo.altitude = newLocation.altitude;
    currentGeo.accuracy = newLocation.horizontalAccuracy;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.

	 //
    //Motion Manager
    //
    motionManager = [[CMMotionManager alloc] init];
    motionManager.accelerometerUpdateInterval = 0.2;
    motionManager.gyroUpdateInterval = 0.2;
    [motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue] withHandler:^(CMAccelerometerData* accelerometerData, NSError* error)
    {
        [self outputAccelerationData:accelerometerData.acceleration];
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
    

    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
	AppSleep();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
	AppResume();
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
