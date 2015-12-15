#import "OSXAppDelegate.h"
#import "../RESULT/EHM.h"

#import <AppKit/AppKit.h>

RESULT InitializeOSXWindow() {
    RESULT r = R_PASS;
    
    NSArray *tl;
    NSApplication *application = [NSApplication sharedApplication];
    [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:application topLevelObjects:&tl];
    
    OSXAppDelegate *osxAppDelegate = [[OSXAppDelegate alloc] init];      // Instantiate App  delegate
    [application setDelegate:osxAppDelegate];                      // Assign delegate to the NSApplication
    [application run];                                                  // Call the Apps Run method
    
Error:
    return r;
}