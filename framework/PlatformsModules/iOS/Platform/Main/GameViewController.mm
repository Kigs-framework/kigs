#import "GameViewController.h"
#import <OpenGLES/ES2/glext.h>
#include "../../../../../framework/PlatformsModules/iOS/Platform/Main/KigsInterface.h"

#include <pthread.h>
#import <memory>

structGeolocation currentGeo;

pthread_mutex_t	eventMutex;

NSBundle*		mainBundle;

structCoord listCoordinate[5];

void*				globalGLContext=0;
unsigned int		globalScreenSizeX=0;
unsigned int		globalScreenSizeY=0;

@interface GameViewController () {}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation GameViewController
{
   
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Get the main bundle for the app.
	mainBundle = [NSBundle mainBundle];

	NSString *GLESVersion = [mainBundle objectForInfoDictionaryKey:@"GLESVersion"];

	EAGLRenderingAPI api=kEAGLRenderingAPIOpenGLES1;

	if(GLESVersion)
	{
		if ([GLESVersion isEqualToString:@"1"])
		{
			api=kEAGLRenderingAPIOpenGLES1;
		}
		else if ([GLESVersion isEqualToString:@"2"])
		{
			api=kEAGLRenderingAPIOpenGLES2;
		}
	}
    self.context = [[EAGLContext alloc] initWithAPI:api];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }

	globalGLContext=(__bridge void*)self.context;

    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;

	view.multipleTouchEnabled =YES;

    [self setupGL];



	AppInit();
}

- (void)dealloc
{
    [self tearDownGL];

	AppClose();

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;

        [self tearDownGL];

        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];

	globalScreenSizeX=self.view.bounds.size.width;
	globalScreenSizeY=self.view.bounds.size.height;
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];

	//  Cube_tearDownGL();
}

#pragma mark - GLKView and GLKViewController delegate methods

extern float _rotation;

- (void)update
{
 //Cube_update(/*self.timeSinceLastUpdate, self.view.bounds.size.width, self.view.bounds.size.height*/);

}


- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
pthread_mutex_lock( &eventMutex );
    for (UITouch *touch in touches)
    {
        structCoord clicCoord;

		CGPoint touchPoint = [touch locationInView:self.view];

        clicCoord.mouseCoordinate[0] = touchPoint.x*self.view.contentScaleFactor;
        clicCoord.mouseCoordinate[1] = touchPoint.y*self.view.contentScaleFactor;
        clicCoord.phase = touch.phase;
        clicCoord.tapCount = (int)touch.tapCount;
        clicCoord.active = true;
        clicCoord.Touch = touch;
        bool L_Found= false;
        
        //Search if this touch ptr already exist
        for(int i = 0; i < 5; i++)
        {
            if(listCoordinate[i].Touch == touch )
            {
                listCoordinate[i] = clicCoord;
                L_Found = true;
                break;
            }
        }
        if(!L_Found)
        {
            for(int i = 0; i < 5; i++)
            {
                if(listCoordinate[i].Touch == NULL )
                {
                    listCoordinate[i] = clicCoord;
                    L_Found = true;
                    break;
                }
            }
        }

    }
		pthread_mutex_unlock( &eventMutex );
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
pthread_mutex_lock( &eventMutex );
    for (UITouch *touch in touches)
    {
        bool L_Found= false;
        for(int i = 0; i < 5; i++)
        {
            if(listCoordinate[i].Touch == touch )
            {
                listCoordinate[i].phase = touch.phase;
                listCoordinate[i].tapCount = (int)touch.tapCount;
                listCoordinate[i].active = false;
                L_Found = true;
                break;
            }
        }
    }
			pthread_mutex_unlock( &eventMutex );
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
pthread_mutex_lock( &eventMutex );
    for (UITouch *touch in touches)
    {
        bool L_Found= false;
        for(int i = 0; i < 5; i++)
        {
            if(listCoordinate[i].Touch == touch )
            {
               CGPoint touchPoint = [touch locationInView:self.view];

				listCoordinate[i].mouseCoordinate[0] = touchPoint.x*self.view.contentScaleFactor;
				listCoordinate[i].mouseCoordinate[1] = touchPoint.y*self.view.contentScaleFactor;
                listCoordinate[i].phase = touch.phase;
                listCoordinate[i].tapCount = (int)touch.tapCount;
                listCoordinate[i].active = true;
                L_Found = true;
                break;
            }
        }
    }
			pthread_mutex_unlock( &eventMutex );
}

- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event
{
pthread_mutex_lock( &eventMutex );
    for (UITouch *touch in touches)
    {
        bool L_Found= false;
        for(int i = 0; i < 5; i++)
        {
            if(listCoordinate[i].Touch == touch )
            {
                listCoordinate[i].phase = touch.phase;
                listCoordinate[i].tapCount = (int)touch.tapCount;
                listCoordinate[i].active = false;
                L_Found = true;
                break;
            }
        }
    }
			pthread_mutex_unlock( &eventMutex );
}

- (void) touchesStationary: (NSSet*) touches withEvent: (UIEvent*) event
{
pthread_mutex_lock( &eventMutex );
    for (UITouch *touch in touches)
    {
        bool L_Found= false;
        for(int i = 0; i < 5; i++)
        {
            if(listCoordinate[i].Touch == touch )
            {
				CGPoint touchPoint = [touch locationInView:self.view];

				listCoordinate[i].mouseCoordinate[0] = touchPoint.x*self.view.contentScaleFactor;
				listCoordinate[i].mouseCoordinate[1] = touchPoint.y*self.view.contentScaleFactor;
                listCoordinate[i].phase = touch.phase;
                listCoordinate[i].tapCount = (int)touch.tapCount;
                listCoordinate[i].active = true;
                L_Found = true;
                break;
            }
        }
    }
			pthread_mutex_unlock( &eventMutex );
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	// Cube_prepare();
   // Cube_draw();
     AppUpdate();
}

@end

void fopenBundle(MAC_FILE** file,const char* filename,const char* ext,const char* directory)
{
	NSString* mallocFileName = [[NSString alloc] initWithUTF8String:filename];
    NSString* fileext =0;
	NSString* shortfilename = mallocFileName;
   
    NSRange range;
    range = [shortfilename rangeOfString:@"." options:NSBackwardsSearch];
        
    if(range.length)
    {
        fileext = [shortfilename substringFromIndex:range.location+1];
        //cut extension
        shortfilename = [shortfilename substringToIndex:range.location];
    }
        
    NSString* path = [mainBundle pathForResource:shortfilename ofType:fileext];
	if(path)
    {
		(*file)=new MAC_FILE();
        NSData* theData = [NSData dataWithContentsOfFile:path];
        (*file)->myData = theData;
        (*file)->myCurrentPos = 0;
        (*file)->myFile = 0;
        (*file)->mySize = theData.length;
       
	}

}

bool checkstateBundle(const char* filename)
{
	bool result=false; 
	NSString* mallocFileName = [[NSString alloc] initWithUTF8String:filename];
    NSString* fileext = 0;
    NSString* fileshortname = 0;
    NSString* convertFilename = mallocFileName;
        
    NSRange range;
    range = [convertFilename rangeOfString:@"." options:NSBackwardsSearch];
        
    if(range.length)
    {
        fileext = [convertFilename substringFromIndex:range.location+1];
        //cut extension
        convertFilename = [convertFilename substringToIndex:range.location];
    }
    range = [convertFilename rangeOfString:@"/" options:NSBackwardsSearch];
        
    if(range.length)
    {
        fileshortname = [convertFilename substringFromIndex:range.location+1];
        //Keep only path
        convertFilename = [convertFilename substringToIndex:range.location];
    }
    else
    {
        fileshortname = convertFilename;
        //nothing else
        convertFilename = 0;
    }
        
    NSString* path = [[NSBundle mainBundle] pathForResource:fileshortname ofType: fileext];
    if(path)
	{
		result=true;
	}   

	return result;
}

long int  freadBundle(void * ptr, long size, long count, MAC_FILE* file)
{
	NSData* currentdata = file->myData;
        
    int range = count*size;
        
    if( (range + file->myCurrentPos) > file->mySize)
        range = file->mySize - file->myCurrentPos;
        
    NSRange theRange = NSMakeRange(file->myCurrentPos, range);
    [currentdata getBytes:ptr range:theRange];
        
    file->myCurrentPos += range;
    return range;
}

 int  fcloseBundle(MAC_FILE* file)
 {
	file->myData=0;
	return 0;
 }

void setCurrentGLContext(void* context)
{
[EAGLContext setCurrentContext:(__bridge EAGLContext*)context];
}

void getDocumentFolder(char* folder)
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *libraryDirectory = paths.firstObject;
	const char* asciiString = [libraryDirectory UTF8String];
	size_t len = strlen(asciiString);
	memcpy(folder, asciiString, len);
	folder[len]=0;
}

void getApplicationFolder(char* folder)
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
	NSString *libraryDirectory = paths.firstObject;
	const char* asciiString = [libraryDirectory UTF8String];
	size_t len = strlen(asciiString);
	memcpy(folder, asciiString, len);
	folder[len]=0;
}
