#include "OSXSandboxApp.h"
#include "Sandbox/PathManagerFactory.h"

//#include "HAL/opengl/OpenGLImp.h"

//#include "OSXKeyboard.h"
//#include "OSXMouse.h"

OSXSandboxApp::OSXSandboxApp(char* pszClassName) :
    m_pOSXApplication(NULL),
    m_pszClassName(pszClassName),
    m_pxWidth(DEFAULT_WIDTH),
    m_pxHeight(DEFAULT_HEIGHT),
    m_fFullscreen(DEFAULT_FULLSCREEN)
{
    RESULT r = R_PASS;
    
    // Default title
    m_pszWindowTitle = "Dream OS Sandbox";
    
    // Initialize Path Manager
    m_pPathManager = PathManagerFactory::MakePathManager(PATH_MANAGER_OSX);
    CNM(m_pPathManager, "Failed to make path manager!");
    m_pPathManager->PrintPaths();
    
    // Set up the OSX Application Window
    m_pOSXApplication = new OSXApplication();
    CVM(m_pOSXApplication, "Failed to create OSX Application");.
    
    
    // TODO: Move into Sandbox virtual function
    /*
    // Create the Keyboard and Mouse
    m_pWin64Keyboard = new Win64Keyboard(this);
    m_pWin64Mouse = new Win64Mouse(this);
    
    // Initialize Mouse
    m_pWin64Mouse->CaptureMouse();
    m_pWin64Mouse->CenterMousePosition();
     */
    
    Validate();
    return;
    
Error:
    Invalidate();
}

OSXSandboxApp::~OSXSandboxApp() {
    // empty stub for now
}

RESULT OSXSandboxApp::SetDimensions(int pxWidth, int pxHeight) {
    RESULT r = R_PASS;
    
    m_pxWidth = pxWidth;
    m_pxHeight = pxHeight;
    
    // TODO: Add OpengL imp
    // OpenGL Resize the view after the window had been resized
    // CRM(m_pOpenGLImp->Resize(m_pxWidth, m_pxHeight), "Failed to resize OpenGL Implemenation");
    
Error:
    return r;
}

/* TODO: Move to Sandbox
RESULT OSXSandboxApp::RegisterImpKeyboardEvents() {
    RESULT r = R_PASS;
    
    CR(m_pOSXKeyboard->RegisterSubscriber(VK_LEFT, m_pOpenGLImp));
    CR(m_pOSXKeyboard->RegisterSubscriber(VK_UP, m_pOpenGLImp));
    CR(m_pOSXKeyboard->RegisterSubscriber(VK_DOWN, m_pOpenGLImp));
    CR(m_pOSXKeyboard->RegisterSubscriber(VK_RIGHT, m_pOpenGLImp));
    
    for (int i = 0; i < 26; i++) {
        CR(m_pWin64Keyboard->RegisterSubscriber((SK_SCAN_CODE)('A' + i), m_pOpenGLImp));
    }
    
Error:
    return r;
}

RESULT Windows64App::RegisterImpMouseEvents() {
    RESULT r = R_PASS;
    
    CR(m_pWin64Mouse->RegisterSubscriber(SENSE_MOUSE_MOVE, m_pOpenGLImp));
    CR(m_pWin64Mouse->RegisterSubscriber(SENSE_MOUSE_LEFT_BUTTON, m_pOpenGLImp));
    CR(m_pWin64Mouse->RegisterSubscriber(SENSE_MOUSE_RIGHT_BUTTON, m_pOpenGLImp));
    
Error:
    return r;
}
 */

// Note this call will never return and will actually run the event loop
// TODO: Thread it?
RESULT OSXSandboxApp::ShowSandbox() {
    RESULT r = R_PASS;
    bool fQuit = false;
    

    // TODO: Add OpenGL Implementation
    //m_pOpenGLImp = new OpenGLImp(this);
    //CNM(m_pOpenGLImp, "Failed to create OpenGL Implementation");
    
    CRM(SetDimensions(m_posX, m_posY), "Failed to resize OpenGL Implemenation");
    
    DEBUG_LINEOUT("Launching OSX Sandbox ...");
    
    // TODO: Move to Sandbox function
    /*
    CR(RegisterImpKeyboardEvents(), "Failed to register keyboard events");
    CR(RegisterImpMouseEvents(), "Failed to register mouse events");
    */
    

    
    // Launch main message loop
    while (!fQuit) {

        
        // Update the mouse
        // TODO: This is wrong architecture, this should
        // be parallel 
        //m_pWin64Mouse->UpdateMousePosition();
        
        /*
        if(m_pOpenGLImp != NULL)
            m_pOpenGLImp->Render();
        */
        
        // Moved into implementation?
        //SwapBuffers(m_hDC);			// Swap buffers
    }
    
Error:
    return r;
}

RESULT OSXSandboxApp::ShutdownSandbox() {
    RESULT r = R_PASS;
    
    // Shutdown and delete GL Rendering Context
    /*
    if (m_pOpenGLImp != NULL) {
        CRM(m_pOpenGLImp->ShutdownImplementaiton(), "Failed to shutdown opengl implemenation");
        delete m_pOpenGLImp;
        m_pOpenGLImp = NULL;
    }
    */
    
    // If full screen, change back to original res
    if (m_fFullscreen)	
        RecoverDisplayMode();
    
Error:
    return r;
}

RESULT OSXSandboxApp::RecoverDisplayMode() {
    RESULT r = R_PASS;
    
    // TODO: What the hell is this?
    
Error:
    return r;
}
