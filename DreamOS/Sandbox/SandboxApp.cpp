#include "SandboxApp.h"

SandboxApp::SandboxApp() :
	m_pPathManager(NULL),
	m_pOpenGLRenderingContext(NULL),
	m_pSceneGraph(NULL)
{
	m_pSceneGraph = new SceneGraph();
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

RESULT SandboxApp::AddLight(light *pLight) {
	RESULT r = R_PASS;

	CR(AddObject(pLight));

Error:
	return r;
}