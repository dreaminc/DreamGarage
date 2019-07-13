#import "OSXAppDelegate.h"
#import <AppKit/AppKit.h>

//#include "MasterViewController.h"
//#import "ScaryBugDoc.h"

@interface  OSXAppDelegate() {

//@property (nonatomic,strong) IBOutlet MasterViewController *masterViewController;

}

@end

@implementation OSXAppDelegate

-(id)init {
    if(self = [super init]) {
        NSRect contentSize = NSMakeRect(500.0, 500.0, 1000.0, 1000.0);
        NSUInteger windowStyleMask = NSTitledWindowMask | NSResizableWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
        
        m_window = [[NSWindow alloc] initWithContentRect:contentSize styleMask:windowStyleMask backing:NSBackingStoreBuffered defer:YES];
        m_window.backgroundColor = [NSColor greenColor];
        m_window.title = @"Dream OS v0.1 OSX Sandbox";
        
        /*
        // Setup Preference Menu Action/Target on MainMenu
        NSMenu *mm = [NSApp mainMenu];
        NSMenuItem *myBareMetalAppItem = [mm itemAtIndex:0];
        NSMenu *subMenu = [myBareMetalAppItem submenu];
        NSMenuItem *prefMenu = [subMenu itemWithTag:100];
        
        prefMenu.target = self;
        prefMenu.action = @selector(showPreferencesMenu:);
         */
        
        // Create the OpenGL View
        m_osxOpenGLView = [[OSXOpenGLView alloc] initWithFrame: contentSize];
    }
    
    return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    
    
    // 1. Create the master View Controller
    //self.masterViewController = [[MasterViewController alloc] initWithNibName:@"MasterViewController" bundle:nil];
    
    
    // 2. Add the view controller to the Window's content view
    //[m_window.contentView addSubview:m_view];
    //self.masterViewController.view.frame = ((NSView*)self.window.contentView).bounds;
    [m_window setContentView:m_osxOpenGLView];
    
    [m_window makeKeyAndOrderFront:self];     // Show the window
}

-(void)applicationWillFinishLaunching:(NSNotification *)notification {
    // Hook the view up to the window
    [m_window setContentView:m_osxOpenGLView];
}

@end


