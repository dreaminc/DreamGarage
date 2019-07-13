#include "Win64OGLRenderingContext.h"

#include "sandbox/win64/Win64Sandbox.h"

Win64OGLRenderingContext::Win64OGLRenderingContext() {
	RESULT r = R_PASS;

	Validate();
	return;
}

Win64OGLRenderingContext::~Win64OGLRenderingContext() {
	wglDeleteContext(m_hglrc);
	m_hglrc = NULL;
}


// TODO: Might make sense to push this into the Sandbox since the cast might
// cost some performance
inline RESULT Win64OGLRenderingContext::MakeCurrentContext() {
	Win64Sandbox *pWin64Sandbox = reinterpret_cast<Win64Sandbox*>(GetParentSandbox());

	if (!wglMakeCurrent(pWin64Sandbox->GetDeviceContext(), m_hglrc))
		return R_FAIL;

	return R_PASS;
}

inline RESULT Win64OGLRenderingContext::ReleaseCurrentContext() {
	if (!wglMakeCurrent(NULL, NULL))
		return R_FAIL;

	return R_PASS;
}

//RESULT Win64OpenGLRenderingContext::InitializeRenderingContext(int versionMajor, int versionMinor) {
RESULT Win64OGLRenderingContext::InitializeRenderingContext(version versionOGL) {
	RESULT r = R_PASS;
	Win64Sandbox *pWin64Sandbox = reinterpret_cast<Win64Sandbox*>(GetParentSandbox());

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, versionOGL.major(),
		WGL_CONTEXT_MINOR_VERSION_ARB, versionOGL.minor(),
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	// TODO: Move to extensions object
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	CNM(wglCreateContextAttribsARB, "wglCreateContextAttribsARB cannot be NULL");

	HGLRC hglrcTemp = m_hglrc;
	m_hglrc = wglCreateContextAttribsARB(pWin64Sandbox->GetDeviceContext(), 0, attribs);
	DWORD werr = GetLastError();
	DEBUG_LINEOUT("Created OpenGL Rendering Context 0x%x", werr);

	// Init swap interval control	
	if (WGLExtensionSupported("WGL_EXT_swap_control"))
	{
		PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
		PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;

		// Extension is supported, init pointers.
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

		// this is another function from WGL_EXT_swap_control extension
		wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

		if (wglSwapIntervalEXT)
		{
			// Setting it to infinite rendering 
			wglSwapIntervalEXT(0);
		}
	}

	CBM(wglMakeCurrent(NULL, NULL), "Failed to release rendering context");
	CBM(wglDeleteContext(hglrcTemp), "Failed to delete temporary rendering context");

	CNM(m_hglrc, "OpenGL 3.x RC was not created!");

Error:
	return r;
}

RESULT Win64OGLRenderingContext::InitializeRenderingContext() {
	RESULT r = R_PASS;
	Win64Sandbox *pWin64Sandbox = reinterpret_cast<Win64Sandbox*>(GetParentSandbox());
	HGLRC hglrcTemp = NULL;

	
	memset(&m_pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	m_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	m_pfd.nVersion = 1;
	m_pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	//m_pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	m_pfd.iPixelType = PFD_TYPE_RGBA;
	m_pfd.cColorBits = 32;
	m_pfd.cDepthBits = 32;
	m_pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(pWin64Sandbox->GetDeviceContext(), &m_pfd);

	CBM((nPixelFormat != NULL), "nPixelFormat is NULL with error 0x%x", GetLastError());
	CBM((SetPixelFormat(pWin64Sandbox->GetDeviceContext(), nPixelFormat, &m_pfd)), "Failed to SetPixelFormat %d", nPixelFormat);

	hglrcTemp = wglCreateContext(pWin64Sandbox->GetDeviceContext());
	CNM(hglrcTemp, "Failed to Create GL Context");

	CBM((wglMakeCurrent(pWin64Sandbox->GetDeviceContext(), hglrcTemp)), "Failed OGL wglMakeCurrent");
	

Error:
	m_hglrc = hglrcTemp;
	return r;
}

bool Win64OGLRenderingContext::WGLExtensionSupported(const char* extension_name)
{
    // this is pointer to function which returns pointer to string with list of all wgl extensions
    PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

    // determine pointer to wglGetExtensionsStringEXT function
    _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");

    if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
    {
        // string was not found
        return false;
    }

    // extension is supported
    return true;
}