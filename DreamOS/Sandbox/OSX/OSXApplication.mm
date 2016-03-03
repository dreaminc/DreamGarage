#include "OSXApplication.h"

OSXApplication::OSXApplication() :
    m_pNSApp(NULL),
    m_pOSXAppDelegate(NULL)
{
    RESULT r = R_PASS;
    
    CRM(InitializeOSXWindow(), "Failed to Initialize OSX Window");
    
    Validate();
Error:
    Invalidate();
}

OSXApplication::~OSXApplication() {
    // empty
}

RESULT OSXApplication::InitializeOSXWindow() {
    RESULT r = R_PASS;
    
    m_pNSApp = [NSApplication sharedApplication];
    
    //[[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:application topLevelObjects:&tl];
    
    m_pOSXAppDelegate = [[OSXAppDelegate alloc] init];      // Instantiate App  delegate
    [m_pNSApp setDelegate:m_pOSXAppDelegate];               // Assign delegate to the NSApplication
    [m_pNSApp run];                                         // Call the Apps Run method
    
Error:
    return r;
}