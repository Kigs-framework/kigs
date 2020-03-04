
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "RenderingView.h"
// #include "MouseIPhone.h"
// #include "CoreIncludes.h"
#import "IphoneMain.h"
#include <vector>

//@interface IPhoneRenderingView


/****** resolution ******/
/*   true  -> 960*640   */
/*   false -> 480*320   */
// extern bool highDefinition;
/************************/

//@end

@implementation RenderingView


// override default layer class

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

-(void) ResetTouchStack
{
    structCoord L_initValues;
    L_initValues.Touch = NULL;
    L_initValues.active = false;
    L_initValues.mouseCoordinate = CGPointZero;
    L_initValues.tapCount = 0;
    
    //Touch Management
    for(int i = 0; i < 5; i++)
        listCoordinate[i] = L_initValues;
}
- (id)initWithFrame:(CGRect)frame
{
    [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
    if((self = [super initWithFrame:frame]))
    {
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;

        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:YES], kEAGLDrawablePropertyRetainedBacking,kEAGLColorFormatRGB565,kEAGLDrawablePropertyColorFormat,nil];
        
        
        [self ResetTouchStack];
        [self setMultipleTouchEnabled:YES];
    }
    
    return self;
}

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
    for (UITouch *touch in touches)
    {
        structCoord clicCoord;
        clicCoord.mouseCoordinate = [touch locationInView:self];
        clicCoord.mouseCoordinate.x *= self.contentScaleFactor;
        clicCoord.mouseCoordinate.y *= self.contentScaleFactor;
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
#ifdef DEBUG
        if(!L_Found)
            NSLog(@"Touch Began Error");
#endif
    }
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
    for (UITouch *touch in touches)
    {
        bool L_Found= false;
        for(int i = 0; i < 5; i++)
        {
            if(listCoordinate[i].Touch == touch )
            {
                //listCoordinate[i].mouseCoordinate = [touch locationInView:self];
                /*listCoordinate[i].mouseCoordinate.x *= self.contentScaleFactor;
                listCoordinate[i].mouseCoordinate.y *= self.contentScaleFactor;*/
                listCoordinate[i].phase = touch.phase;
                listCoordinate[i].tapCount = (int)touch.tapCount;
                listCoordinate[i].active = false;
                L_Found = true;
                break;
            }
        }
#ifdef DEBUG
        if(!L_Found)
            NSLog(@"Touch Ended Error");
#endif
    }
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
    for (UITouch *touch in touches)
    {
        bool L_Found= false;
        for(int i = 0; i < 5; i++)
        {
            if(listCoordinate[i].Touch == touch )
            {
                listCoordinate[i].mouseCoordinate = [touch locationInView:self];
                listCoordinate[i].mouseCoordinate.x *= self.contentScaleFactor;
                listCoordinate[i].mouseCoordinate.y *= self.contentScaleFactor;
                listCoordinate[i].phase = touch.phase;
                listCoordinate[i].tapCount = (int)touch.tapCount;
                listCoordinate[i].active = true;
                L_Found = true;
                break;
            }
        }
#ifdef DEBUG
        if(!L_Found)
            NSLog(@"Touch Moved Error");
#endif

    }
}

- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event
{
    for (UITouch *touch in touches)
    {
        bool L_Found= false;
        for(int i = 0; i < 5; i++)
        {
            if(listCoordinate[i].Touch == touch )
            {
                listCoordinate[i].mouseCoordinate = [touch locationInView:self];
                /*NSLog(@"Cancel touch : %f || %f",listCoordinate[i].mouseCoordinate.x,listCoordinate[i].mouseCoordinate.y);
                listCoordinate[i].mouseCoordinate.x *= self.contentScaleFactor;
                 listCoordinate[i].mouseCoordinate.y *= self.contentScaleFactor;*/
                listCoordinate[i].phase = touch.phase;
                listCoordinate[i].tapCount = (int)touch.tapCount;
                listCoordinate[i].active = false;
                L_Found = true;
                break;
            }
        }
#ifdef DEBUG
        if(!L_Found)
            NSLog(@"Touch Canceled Error");
#endif
    }
}

- (void) touchesStationary: (NSSet*) touches withEvent: (UIEvent*) event
{
    for (UITouch *touch in touches)
    {
        bool L_Found= false;
        for(int i = 0; i < 5; i++)
        {
            if(listCoordinate[i].Touch == touch )
            {
                listCoordinate[i].mouseCoordinate = [touch locationInView:self];
                /*NSLog(@"stationary touch : %f || %f",listCoordinate[i].mouseCoordinate.x,listCoordinate[i].mouseCoordinate.y);
                listCoordinate[i].mouseCoordinate.x *= self.contentScaleFactor;
                 listCoordinate[i].mouseCoordinate.y *= self.contentScaleFactor;*/
                listCoordinate[i].mouseCoordinate.x *= self.contentScaleFactor;
                listCoordinate[i].mouseCoordinate.y *= self.contentScaleFactor;
                listCoordinate[i].phase = touch.phase;
                listCoordinate[i].tapCount = (int)touch.tapCount;
                listCoordinate[i].active = true;
                L_Found = true;
                break;
            }
        }
#ifdef DEBUG
        if(!L_Found)
            NSLog(@"Touch Stationary Error");
#endif
    }
}

@end
































