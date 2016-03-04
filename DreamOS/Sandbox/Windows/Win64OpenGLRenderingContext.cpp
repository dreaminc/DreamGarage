#include "Win64OpenGLRenderingContext.h"
#include "Windows64App.h"

Win64OpenGLRenderingContext::Win64OpenGLRenderingContext() {
	RESULT r = R_PASS;

	Validate();
	return;

Error:
	Invalidate();
	return;
}

Win64OpenGLRenderingContext::~Win64OpenGLRenderingContext() {
	wglDeleteContext(m_hglrc);
	m_hglrc = NULL;
}


// TODO: Might make sense to push this into the SandboxApp since the cast might
// cost some performance
RESULT Win64OpenGLRenderingContext::MakeCurrentContext() {
	Windows64App *pWin64App = reinterpret_cast<Windows64App*>(GetParentApp());

	if (!wglMakeCurrent(pWin64App->GetDeviceContext(), m_hglrc))
		return R_FAIL;

	return R_PASS;
}

RESULT Win64OpenGLRenderingContext::ReleaseCurrentContext() {
	if (!wglMakeCurrent(NULL, NULL))
		return R_FAIL;

	return R_PASS;
}

RESULT Win64OpenGLRenderingContext::InitializeRenderingContext(int versionMajor, int versionMinor) {
	RESULT r = R_PASS;
	Windows64App *pWin64App = reinterpret_cast<Windows64App*>(GetParentApp());

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, versionMajor,
		WGL_CONTEXT_MINOR_VERSION_ARB, versionMinor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	// TODO: Move to extensions object
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	CNM(wglCreateContextAttribsARB, "wglCreateContextAttribsARB cannot be NULL");

	HGLRC hglrcTemp = m_hglrc;
	m_hglrc = wglCreateContextAttribsARB(pWin64App->GetDeviceContext(), 0, attribs);
	DWORD werr = GetLastError();
	DEBUG_LINEOUT("Created OpenGL Rendering Context 0x%x", werr);

	CBM(wglMakeCurrent(NULL, NULL), "Failed to release rendering context");
	CBM(wglDeleteContext(hglrcTemp), "Failed to delete temporary rendering context");

	CNM(m_hglrc, "OpenGL 3.x RC was not created!");

Error:
	return r;
}

RESULT Win64OpenGLRenderingContext::InitializeRenderingContext() {
	RESULT r = R_PASS;
	Windows64App *pWin64App = reinterpret_cast<Windows64App*>(GetParentApp());
	HGLRC hglrcTemp = NULL;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(pWin64App->GetDeviceContext(), &pfd);

	CBM((nPixelFormat != NULL), "nPixelFormat is NULL");
	CBM((SetPixelFormat(pWin64App->GetDeviceContext(), nPixelFormat, &pfd)), "Failed to SetPixelFormat");

	hglrcTemp = wglCreateContext(pWin64App->GetDeviceContext());
	CNM(hglrcTemp, "Failed to Create GL Context");

	CBM((wglMakeCurrent(pWin64App->GetDeviceContext(), hglrcTemp)), "Failed OGL wglMakeCurrent");

Error:
	m_hglrc = hglrcTemp;
	return r;
}