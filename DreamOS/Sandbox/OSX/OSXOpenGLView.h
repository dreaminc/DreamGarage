#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

@class NSOpenGLContext, NSOpenGLPixelFormat;

// TODO: Custom View
//@interface OSXOpenGLView : NSView {
@interface OSXOpenGLView : NSOpenGLView {
@private
    NSOpenGLContext         *m_oglctx;
    NSOpenGLPixelFormat *m_pixelFormat;
}

/*
+ (NSOpenGLPixelFormat*)defaultPixelFormat;
- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format;
- (void)setOpenGLContext:(NSOpenGLContext*)context;
- (NSOpenGLContext*)openGLContext;
- (void)clearGLContext;
- (void)prepareOpenGL;
- (void)update;
- (void)setPixelFormat:(NSOpenGLPixelFormat*)pixelFormat;
- (NSOpenGLPixelFormat*)pixelFormat;
 */

@end
