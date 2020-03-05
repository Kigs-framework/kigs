//
//  GameViewController.h
//  Game
//
//  Created by Stephane Capo on 16/02/2015.
//  Copyright (c) 2015 Stephane Capo. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface IOSViewController : GLKViewController
{
	unsigned int _program;
    
    GLKMatrix4 _modelViewProjectionMatrix;
    GLKMatrix3 _normalMatrix;
    float _rotation;
    
	unsigned int _vertexArray;
	unsigned int _vertexBuffer;
    
    int _sens;

    GLKView* view;
	EAGLContext* context;
	GLKBaseEffect* effect;
}

@property (strong, nonatomic) GLKView* view;
@property (strong, nonatomic) EAGLContext* context;
@property (strong, nonatomic) GLKBaseEffect* effect;

- (void)setupGL;
- (void)tearDownGL;

- (BOOL)loadShaders;
- (BOOL)compileShader:(unsigned int *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(unsigned int)prog;
- (BOOL)validateProgram:(unsigned int)prog;

@end
