#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <Foundation/Foundation.h>
#include <stdio.h>


/****** Touch ******/
struct structCoord
{
	float mouseCoordinate[2];
	unsigned int phase;
	int tapCount;
	bool active;
	UITouch* Touch;
};

extern structCoord listCoordinate[5];
/*******************/


/****** Geolocalisation ******/
struct structGeolocation {
	double latitude;
	double longitude;
	double altitude;
	float accuracy;
};

struct  MAC_FILE {
	NSData*			myData;
	unsigned int	myCurrentPos;
	unsigned int	mySize;
	FILE* myFile;
};

extern structGeolocation currentGeo;

// general use mutex
extern pthread_mutex_t	eventMutex;

// glsetup
extern void*			globalGLContext;
extern unsigned int		globalScreenSizeX;
extern unsigned int		globalScreenSizeY;

extern	NSBundle*		mainBundle;

extern void setCurrentGLContext(void* context);

extern void fopenBundle(MAC_FILE** file,const char* filename,const char* ext,const char* directory);
extern long int  freadBundle(void * ptr, long size, long count, MAC_FILE* file);
extern bool checkstateBundle(const char* filename);
extern int  fcloseBundle(MAC_FILE* file);
extern void getDocumentFolder(char* folder);
extern void getApplicationFolder(char* folder);

@interface GameViewController : GLKViewController

@end
