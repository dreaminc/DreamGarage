#include "SandboxApp.h"

SandboxApp::SandboxApp() :
	m_pPathManager(NULL),
	m_pOpenGLRenderingContext(NULL),
	m_pSceneGraph(NULL),
	m_pCloudController(nullptr)
{
	// empty
}

SandboxApp::~SandboxApp() {
	// empty stub
}

inline PathManager * SandboxApp::GetPathManager() {
	return m_pPathManager; 
}

inline OpenGLRenderingContext * SandboxApp::GetOpenGLRenderingContext() {
	return m_pOpenGLRenderingContext; 
}

RESULT SandboxApp::Initialize() {
	RESULT r = R_PASS;

	m_pSceneGraph = new SceneGraph();
	CNM(m_pSceneGraph, "Failed to allocate Scene Graph");

	CRM(InitializeCloudController(), "Failed to initialize cloud controller");

	// TODO: Show this be replaced with individual initialization of each component?
	CRM(InitializeSandbox(), "Failed to initialize sandbox");

Error:
	return r;
}