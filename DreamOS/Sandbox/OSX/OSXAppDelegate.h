#ifndef OSX_APP_DELEGATE_H_
#define OSX_APP_DELEGATE_H_

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>

#import "OSXOpenGLView.h"

@class OSXAppDelegate;

@interface OSXAppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate> {
    NSWindow *m_window;
    OSXOpenGLView *m_osxOpenGLView;
}

//@property (assign) IBOutlet NSWindow *m_window;
//@property (assign) IBOutlet NSView *m_view;

@end


#endif // !OSX_APP_DELEGATE_H_