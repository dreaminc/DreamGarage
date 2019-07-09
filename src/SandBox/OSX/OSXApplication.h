#ifndef OSX_APPLICATION_H_
#define OSX_APPLICATION_H_

#include "RESULT/EHM.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Sandbox/OSX/OSXApplication.h
// Dream OS OSX Application OBJ-C++ Container

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>

#import "OSXAppDelegate.h"

class OSXApplication : public valid {
public:
    OSXApplication();
    ~OSXApplication();
    
    RESULT InitializeOSXWindow();
    
private:
    OSXAppDelegate *m_pOSXAppDelegate;
    NSApplication *m_pNSApp;
};



#endif // ! OSX_APP_H_