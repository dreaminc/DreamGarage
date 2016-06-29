#include "OpenGLImp.h"
#include "OGLObj.h"
#include "OGLFramebuffer.h"

#include "Primitives/ProjectionMatrix.h"
#include "Primitives/TranslationMatrix.h"
#include "Primitives/RotationMatrix.h"

#include "../DreamOS/Sandbox/FileLoader.h"
#include <vector>

#include "../DreamOS/Profiler/Profiler.h"

OpenGLImp::OpenGLImp(OpenGLRenderingContext *pOpenGLRenderingContext) :
	m_versionOGL(0),
	m_versionGLSL(0),
	m_pOGLRenderProgram(nullptr),
	m_pOGLSkyboxProgram(nullptr),
	m_pOGLOverlayProgram(nullptr),
	m_pOpenGLRenderingContext(pOpenGLRenderingContext),
	m_pCamera(nullptr),
	m_pHMD(nullptr)
{
	RESULT r = R_PASS;

	CRM(InitializeGLContext(), "Failed to Initialize OpenGL Context");
	CRM(PrepareScene(), "Failed to prepare GL Scene");

	Validate();
	return;
Error:
	Invalidate();
	return;
}

OpenGLImp::~OpenGLImp() {
	m_pOGLProfiler.release();

	if (m_pOGLRenderProgram != nullptr) {
		delete m_pOGLRenderProgram;
		m_pOGLRenderProgram = nullptr;
	}

	if (m_pOGLSkyboxProgram != nullptr) {
		delete m_pOGLSkyboxProgram;
		m_pOGLSkyboxProgram = nullptr;
	}

	if (m_pOGLOverlayProgram != nullptr) {
		delete m_pOGLOverlayProgram;
		m_pOGLOverlayProgram = nullptr;
	}
}

RESULT OpenGLImp::InitializeOpenGLVersion() {
	// For all versions
	char* pszVersion = (char*)glGetString(GL_VERSION); // Ver = "3.2.0"
	int vMajor = 0, vMinor = 0, vDblMinor = 0;	// TODO: use minor?
	DEBUG_LINEOUT("OpenGL Version %s", pszVersion);

	vMajor = pszVersion[0] - '0';
	vMinor = pszVersion[2] - '0';
	vDblMinor = pszVersion[4] - '0';

	// GL 3.x+
	if (vMajor >= 3) {
		glGetIntegerv(GL_MAJOR_VERSION, &vMajor); // major = 3
		glGetIntegerv(GL_MINOR_VERSION, &vMinor); // minor = 2
	}

	m_versionOGL.SetVersion(vMajor, vMinor);

	// GLSL
	// "1.50 NVIDIA via Cg compiler"
	pszVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION); 
	DEBUG_LINEOUT("OpenGL GLSL Version %s", pszVersion);

	vMajor = pszVersion[0] - '0';
	vMinor = pszVersion[2] - '0';
	vDblMinor = pszVersion[4] - '0';

	m_versionGLSL.SetVersion(vMajor, vMinor);

	return R_PASS;
}

/*
RESULT OpenGLImp::CreateGLProgram() {
	RESULT r = R_PASS;

	CBM((m_idOpenGLProgram == NULL), "Cannot CreateGLProgram if program id not null");

	m_idOpenGLProgram = m_OpenGLExtensions.glCreateProgram();
	CBM((m_idOpenGLProgram != 0), "Failed to create program id");

	GLboolean fIsProg = m_OpenGLExtensions.glIsProgram(m_idOpenGLProgram);
	CBM(fIsProg, "Failed to create program");

	CRM(CheckGLError(), "CreateGLProgram failed")

	DEBUG_LINEOUT("Created GL program ID %d", m_idOpenGLProgram);

Error:
	return r;
}
*/

RESULT OpenGLImp::InitializeGLContext() {
	RESULT r = R_PASS;

	CRM(m_pOpenGLRenderingContext->InitializeRenderingContext(), "Failed to initialize oglrc");
	CR(InitializeOpenGLVersion());
	//CBM((m_versionMajor >= 3 || (m_versionMajor == 3 && m_versionMinor >= 2)), "OpenGL 3.2 + Not Supported");
	CBM((m_versionOGL >= 3.2f), "OpengL 3.2+ Not Supported");

	//CR(InitializeShadersFolder());

	// Should be called after context is created and made current
	ACRM(m_OpenGLExtensions.InitializeExtensions(), "Failed to initialize extensions");
	
	// Lets create the 3.2+ context
	CRM(m_pOpenGLRenderingContext->InitializeRenderingContext(m_versionOGL), "Failed to initialize oglrc");
	
Error:
	return r;
}

RESULT OpenGLImp::CheckGLError() {
	RESULT r = R_PASS;

	GLenum glerr = glGetError();
	switch (glerr) {
		case GL_NO_ERROR: return R_PASS; break;
		case GL_INVALID_ENUM: CBRM(false, R_FAIL, "CheckGLError: GL_INVALID_ENUM"); break;
		case GL_INVALID_VALUE: CBRM(false, R_FAIL, "CheckGLError: GL_INVALID_VALUE"); break;
		case GL_INVALID_OPERATION: CBRM(false, R_FAIL, "CheckGLError: GL_INVALID_OPERATION"); break;
		case GL_STACK_OVERFLOW: CBRM(false, R_FAIL, "CheckGLError: GL_STACK_OVERFLOW"); break;
		case GL_STACK_UNDERFLOW: CBRM(false, R_FAIL, "CheckGLError: GL_STACK_UNDERFLOW"); break;
		case GL_OUT_OF_MEMORY: CBRM(false, R_FAIL, "CheckGLError: GL_OUT_OF_MEMORY"); break;
	}

Error:
	return r;
}

RESULT OpenGLImp::BindBufferBase(GLenum target, GLuint bindingPointIndex, GLuint bufferIndex) {
	RESULT r = R_PASS;
	GLenum glerr;
	DWORD werr;

	CR(glBindBufferBase(target, bindingPointIndex, bufferIndex));

	werr = GetLastError();
	DEBUG_LINEOUT("Bound uniform block binding point %d to base buffer %d err:0x%x", bindingPointIndex, bufferIndex, werr);

Error:
	return r;
}

RESULT OpenGLImp::MakeCurrentContext() {
	return m_pOpenGLRenderingContext->MakeCurrentContext();
}

RESULT OpenGLImp::ReleaseCurrentContext() {
	return m_pOpenGLRenderingContext->ReleaseCurrentContext();
}

// TODO: This should be moved to OpenGL Program arch/design
RESULT OpenGLImp::PrepareScene() {
	RESULT r = R_PASS;
	GLenum glerr = GL_NO_ERROR;

	CR(m_pOpenGLRenderingContext->MakeCurrentContext());

	// Clear Background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	// TODO(NTH): Add a program / render pipeline arch
	m_pOGLRenderProgram = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_BLINNPHONG_TEXTURE_BUMP, this, m_versionGLSL);
	//m_pOGLRenderProgram = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_MINIMAL, this, m_versionGLSL);
	CN(m_pOGLRenderProgram);

	m_pOGLSkyboxProgram = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_SKYBOX, this, m_versionGLSL);
	CN(m_pOGLSkyboxProgram);

	m_pOGLOverlayProgram = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_TEXTURE_BITBLIT, this, m_versionGLSL);
	CN(m_pOGLOverlayProgram);

	m_pOGLProfiler = std::make_unique<OGLProfiler>(this, m_pOGLOverlayProgram);

	// Depth testing
	glEnable(GL_DEPTH_TEST);	// Enable depth test
	glDepthFunc(GL_LEQUAL);		// Accept fragment if it closer to the camera than the former one

	// Face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Dithering 
	glEnable(GL_DITHER);

	// Allocate the camera
	// TODO: Wire this up directly to HMD
	m_pCamera = new stereocamera(point(0.0f, 0.0f, -10.0f), 100.0f, m_pxViewWidth, m_pxViewHeight);
	CN(m_pCamera);

	CR(m_pOpenGLRenderingContext->ReleaseCurrentContext());

Error:
	return r;
}

RESULT OpenGLImp::Resize(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	m_pxViewWidth = pxWidth;
	m_pxViewHeight = pxHeight;

	CR(m_pOpenGLRenderingContext->MakeCurrentContext());

	glViewport(0, 0, (GLsizei)m_pxViewWidth, (GLsizei)m_pxViewHeight);

	m_pCamera->ResizeCamera(m_pxViewWidth, m_pxViewHeight);

Error:
	//CR(m_pOpenGLRenderingContext->ReleaseCurrentContext());

	return r;
}

RESULT OpenGLImp::SetMonoViewTarget() {
	RESULT r = R_PASS;

	// Render to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, (GLsizei)m_pxViewWidth, (GLsizei)m_pxViewHeight);
	m_pCamera->ResizeCamera(m_pxViewWidth, m_pxViewHeight);

Error:
	return r;
}

// Assumes Context Current
RESULT OpenGLImp::SetStereoViewTarget(EYE_TYPE eye) {
	RESULT r = R_PASS;

	// Render to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	switch (eye) {
		case EYE_LEFT: {
			glViewport(0, 0, (GLsizei)m_pxViewWidth / 2, (GLsizei)m_pxViewHeight);
		} break;

		case EYE_RIGHT: {
			glViewport((GLsizei)m_pxViewWidth / 2, 0, (GLsizei)m_pxViewWidth / 2, (GLsizei)m_pxViewHeight);
		} break;

	}

	m_pCamera->ResizeCamera(m_pxViewWidth/2, m_pxViewHeight);

	return r;
}

// Assumes Context Current
RESULT OpenGLImp::SetStereoFramebufferViewTarget(EYE_TYPE eye) {
	m_pCamera->ResizeCamera(m_pHMD->GetEyeWidth(), m_pHMD->GetEyeHeight());
	return m_pHMD->BindFramebuffer(eye);
}

camera * OpenGLImp::GetCamera() {
	return m_pCamera;
}

RESULT OpenGLImp::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;

	/* This has been moved to the camera 
	DEBUG_LINEOUT("Rx kbe %d %d", kbEvent->KeyCode, kbEvent->KeyState);

	switch (kbEvent->KeyCode) {
		case (SK_SCAN_CODE)('A'):
		case SK_LEFT: {
			if (kbEvent->KeyState)
				m_pCamera->AddVelocity(0.1f, 0.0f, 0.0f);
			else
				m_pCamera->AddVelocity(-0.1f, 0.0f, 0.0f);
	
			if (kbEvent->KeyState)
				m_pCamera->Strafe(0.1f);

		} break;

		case (SK_SCAN_CODE)('D') :
		case SK_RIGHT: {
			if (kbEvent->KeyState)
				m_pCamera->AddVelocity(-0.1f, 0.0f, 0.0f);
			else
				m_pCamera->AddVelocity(0.1f, 0.0f, 0.0f);
			
			if (kbEvent->KeyState)
				m_pCamera->Strafe(-0.1f);
		} break;

		case (SK_SCAN_CODE)('W') :
		case SK_UP: {
			if (kbEvent->KeyState)
				m_pCamera->AddVelocity(0.0f, 0.0f, 0.1f);
			else
				m_pCamera->AddVelocity(0.0f, 0.0f, -0.1f);
			
			if (kbEvent->KeyState)
				m_pCamera->MoveForward(0.1f);
		} break;

		case (SK_SCAN_CODE)('S') :
		case SK_DOWN: {
			if (kbEvent->KeyState)
				m_pCamera->AddVelocity(0.0f, 0.0f, -0.1f);
			else
				m_pCamera->AddVelocity(0.0f, 0.0f, 0.1f);
			
			if (kbEvent->KeyState)
				m_pCamera->MoveForward(-0.1f);
		} break;
	}
	*/

	return r;
}

// TODO: Move to camera?
RESULT OpenGLImp::Notify(SenseMouseEvent *mEvent) {
	RESULT r = R_PASS;

	SenseMouse::PrintEvent(mEvent);

	float MouseMoveFactor = 0.1f;

	switch (mEvent->EventType) {
		case SENSE_MOUSE_MOVE: {
			CR(m_pCamera->RotateCameraByDiffXY(static_cast<camera_precision>(mEvent->dx), 
				static_cast<camera_precision>(mEvent->dy)));
		} break;

		case SENSE_MOUSE_LEFT_BUTTON: {
			// TODO: Do something?
		} break;

		case SENSE_MOUSE_RIGHT_BUTTON: {
			// TODO: Do something?
		} break;
	}

Error:
	return r;
}

RESULT OpenGLImp::UpdateCamera() {
	RESULT r = R_PASS;

	m_pCamera->UpdateCameraPosition();

	return r;
}

RESULT OpenGLImp::SetCameraOrientation(quaternion qOrientation) {
	m_pCamera->SetOrientation(qOrientation);
	return R_PASS;
}

RESULT OpenGLImp::SetCameraPositionDeviation(vector vDeviation) {
	m_pCamera->SetCameraPositionDeviation(vDeviation);
	return R_PASS;
}

#include "OGLVolume.h"

#include "OGLModel.h"
#include "OGLText.h"
#include "Primitives/font.h"
#include "OGLTriangle.h"
#include "OGLQuad.h"
#include "Sandbox/PathManager.h"

#include "OGLSphere.h"
#include "Primitives/light.h"
#include "OGLTexture.h"
#include "OGLSkybox.h"

#include "OGLProfiler.h"

light *g_pLight = NULL;

void LoadModel(SceneGraph* pSceneGraph, OpenGLImp* pOGLImp, const std::wstring& root_folder, const std::wstring& obj_file, point pos)
{
	FileLoaderHelper::multi_mesh_t v;
	FileLoaderHelper::LoadOBJFile(root_folder + obj_file, v);

	for (auto& m : v)
	{
		if (m.second.size() == 0)
		{
			continue;
		}

		OGLModel* pModel = new OGLModel(pOGLImp, std::move(m.second));
		std::string tex = m.first.map_Kd;
		std::wstring wstr(tex.begin(), tex.end());
		wstr = L"..\\" + obj_file.substr(0, obj_file.rfind('\\')) + L"\\" + wstr;
		texture *pText = new OGLTexture(pOGLImp, (wchar_t*)(wstr.c_str()), texture::TEXTURE_TYPE::TEXTURE_COLOR);
		pModel->SetColorTexture(pText);
		pModel->MoveTo(pos);
		pModel->UpdateOGLBuffers();
		pSceneGraph->PushObject(pModel);
	}
}

// TODO: Other approach 
RESULT OpenGLImp::LoadScene(SceneGraph *pSceneGraph, TimeManager *pTimeManager) {
	RESULT r = R_PASS;

	// Add lights
	light *pLight = NULL; 

	///*
	pLight = new light(LIGHT_POINT, 1.0f, point(0.0f, 3.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f));
	pSceneGraph->PushObject(pLight);
	//*/

	///*
	float lightHeight = 5.0f, lightSpace = 5.0f, lightIntensity = 1.0f;
	pLight = new light(LIGHT_POINT, lightIntensity, point(lightSpace, lightHeight, -(lightSpace / 2.0)), color(COLOR_BLUE), color(COLOR_BLUE), vector::jVector(-1.0f));
	pSceneGraph->PushObject(pLight);

	pLight = new light(LIGHT_POINT, lightIntensity, point(-lightSpace, lightHeight, -(lightSpace/2.0)), color(COLOR_RED), color(COLOR_RED), vector::jVector(-1.0f));
	pSceneGraph->PushObject(pLight);

	pLight = new light(LIGHT_POINT, lightIntensity, point(0.0f, lightHeight, lightSpace), color(COLOR_GREEN), color(COLOR_GREEN), vector::jVector(-1.0f));
	pSceneGraph->PushObject(pLight);
	//*/

	g_pLight = pLight;
	
	/*
	texture *pBumpTexture = new OGLTexture(this, L"crate_bump.png");
	texture *pColorTexture = new OGLTexture(this, L"crate_color.png");
	//*/

	//*
	texture *pBumpTexture = new OGLTexture(this, L"brickwall_bump.jpg", texture::TEXTURE_TYPE::TEXTURE_BUMP);
	texture *pBumpTexture2 = new OGLTexture(this, L"crate_bump.png", texture::TEXTURE_TYPE::TEXTURE_BUMP);
	//texture *pBumpTexture = new OGLTexture(this, L"bubbles_bump.jpg");
	texture *pColorTexture = new OGLTexture(this, L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	texture *pColorTexture2 = new OGLTexture(this, L"crate_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	//*/

	// TODO: This should be handled in a factory or other compositional approach (constructor or otherwise)
	///*
	OGLSkybox *pSkybox = new OGLSkybox(this);
	OGLTexture *pCubeMap = new OGLTexture(this, L"HornstullsStrand2", texture::TEXTURE_TYPE::TEXTURE_CUBE);
	pSkybox->SetCubeMapTexture(pCubeMap);
	pSkybox->OGLActivateCubeMapTexture();
	pSceneGraph->PushObject(pSkybox);
	//*/
	
	/*
	OGLVolume *pVolume = new OGLVolume(this, 1.0f);
	pVolume->SetColorTexture(pColorTexture);
	pVolume->SetBumpTexture(pBumpTexture);
	pSceneGraph->PushObject(pVolume);
	//m_pFragmentShader->SetTexture(reinterpret_cast<OGLTexture*>(pColorTexture));

	///*
	pVolume = new OGLVolume(this, 1.0f);
	pVolume->SetColorTexture(pColorTexture2);
	pVolume->translateX(2.0f);
	pVolume->SetBumpTexture(pBumpTexture2);
	pSceneGraph->PushObject(pVolume);
	//*/

	/*
	OGLVolume *pVolume = NULL;
	int num = 10;
	double size = 0.5f;
	int spaceFactor = 2;

	for (int i = 0; i < num; i++) {
		for (int j = 0; j < num; j++) {
			pVolume = new OGLVolume(this, size);

			pVolume->SetColorTexture(pColorTexture);
			pVolume->SetBumpTexture(pBumpTexture);

			//pVolume->SetRandomColor();
			pVolume->translate(static_cast<point_precision>(i * (size * 2) - (num * size)), 
				static_cast<point_precision>(0.0f),
				static_cast<point_precision>(j * (size * 2) - (num * size)));

			pVolume->UpdateOGLBuffers();
			//pTimeManager->RegisterSubscriber(TIME_ELAPSED, pVolume);
			pSceneGraph->PushObject(pVolume);
		}
	}
	//*/
		
	///*
	// TODO: All this should go into Model
	std::vector<vertex> v;

	// TODO: Should move to using path manager
	PathManager* pMgr = PathManager::instance();
	wchar_t*	path;
	pMgr->GetCurrentPath((wchar_t*&)path);
	std::wstring objFile(path);
	
	LoadModel(pSceneGraph, this, objFile, L"\\Models\\Bear\\bear-obj.obj",
		point(20.0, 0, 0));

	LoadModel(pSceneGraph, this, objFile, L"\\Models\\Bear\\bear-obj.obj",
		point(0.0, 0, 0));

	/*
	OGLSphere *pSphere = NULL;

	int num = 10;
	int sects = 40;
	double radius = 0.5f;
	double size = radius * 2;
	int spaceFactor = 4;

	for (int i = 0; i < num; i++) {
		for (int j = 0; j < num; j++) {
			pSphere = new OGLSphere(this, radius, sects, sects);

			pSphere->SetColorTexture(pColorTexture);
			pSphere->SetBumpTexture(pBumpTexture);
			//pVolume->SetRandomColor();
			pSphere->translate(i * (size * spaceFactor) - (num * size), 0.0f, j * (size * spaceFactor) - (num * size));
			pSphere->UpdateOGLBuffers();
			//pTimeManager->RegisterSubscriber(TIME_ELAPSED, pSphere);
			pSceneGraph->PushObject(pSphere);
		}
	}
	/*
	OGLQuad *pQuad = NULL;
	pQuad = new OGLQuad(this, 10.0f);
	pQuad->SetColorTexture(pColorTexture);
	pSceneGraph->PushObject(pQuad);

	//*/

	return r;
}

RESULT OpenGLImp::Render(SceneGraph *pSceneGraph) {
	RESULT r = R_PASS;
	SceneGraphStore *pObjectStore = pSceneGraph->GetSceneGraphStore();
	VirtualObj *pVirtualObj = NULL;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);

	CRM(m_pOGLRenderProgram->UseProgram(), "Failed to use OGLProgram");

	// Send lights to shader
	std::vector<light*> *pLights = NULL;
	CR(pObjectStore->GetLights(pLights));
	CN(pLights);
	CR(m_pOGLRenderProgram->SetLights(pLights));

	// Camera Projection Matrix
	SetMonoViewTarget();
	CR(m_pOGLRenderProgram->SetCamera(m_pCamera));

	// Send SceneGraph objects to shader
	pSceneGraph->Reset();
	while((pVirtualObj = pObjectStore->GetNextObject()) != NULL) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pVirtualObj);

		if (pDimObj == NULL)
			continue;
		else {
			CR(m_pOGLRenderProgram->RenderObject(pDimObj));
		}
	}

	skybox *pSkybox = nullptr;
	CR(pObjectStore->GetSkybox(pSkybox));
	if (pSkybox != nullptr) {
		CRM(m_pOGLSkyboxProgram->UseProgram(), "Failed to use OGLProgram");
		CR(m_pOGLSkyboxProgram->SetCamera(m_pCamera));
		CR(m_pOGLSkyboxProgram->RenderObject(pSkybox));
	}
	
	// Render profiler overlay
	m_pOGLProfiler->Render();

	glFlush();

Error:
	CheckGLError();
	return r;
}

RESULT OpenGLImp::RenderStereo(SceneGraph *pSceneGraph) {
	RESULT r = R_PASS;
	SceneGraphStore *pObjectStore = pSceneGraph->GetSceneGraphStore();
	VirtualObj *pVirtualObj = NULL;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CRM(m_pOGLRenderProgram->UseProgram(), "Failed to use OGLProgram");

	// Send lights to shader
	std::vector<light*> *pLights = NULL;
	CR(pObjectStore->GetLights(pLights));
	CN(pLights);
	CR(m_pOGLRenderProgram->SetLights(pLights));

	for (int i = 0; i < 2; i++) {
		EYE_TYPE eye = (i == 0) ? EYE_LEFT : EYE_RIGHT;

		SetStereoViewTarget(eye);
		CR(m_pOGLRenderProgram->SetStereoCamera(m_pCamera, eye));

		// Send SceneGraph objects to shader
		pSceneGraph->Reset();
		while ((pVirtualObj = pObjectStore->GetNextObject()) != NULL) {

			DimObj *pDimObj = dynamic_cast<DimObj*>(pVirtualObj);

			if (pDimObj == NULL)
				continue;
			else {
				CR(m_pOGLRenderProgram->RenderObject(pDimObj));
			}

		}

		skybox *pSkybox = nullptr;
		CR(pObjectStore->GetSkybox(pSkybox));
		if (pSkybox != nullptr) {
			CRM(m_pOGLSkyboxProgram->UseProgram(), "Failed to use OGLProgram");
			CR(m_pOGLSkyboxProgram->SetStereoCamera(m_pCamera, EYE_MONO));
			CR(m_pOGLSkyboxProgram->RenderObject(pSkybox));
		}
	}

	glFlush();

Error:
	return r;
}

/*
RESULT OpenGLImp::InitializeStereoFramebuffers(HMD *pHMD) {
	RESULT r = R_PASS;
	
	for (int i = 0; i < 2; i++) {
		m_pStereoFramebuffers[i] = new OGLFramebuffer(this, pHMD->GetEyeWidth(), pHMD->GetEyeHeight(), 3);
	}

Error:
	return r;
}
*/

RESULT OpenGLImp::SetHMD(HMD *pHMD) {
	RESULT r = R_PASS;

	m_pHMD = pHMD;

Error:
	return r;
}

// TODO: Naming is kind of lame since this hits the HMD
// TODO: Shared code should be consolidated
RESULT OpenGLImp::RenderStereoFramebuffers(SceneGraph *pSceneGraph) {
	RESULT r = R_PASS;
	SceneGraphStore *pObjectStore = pSceneGraph->GetSceneGraphStore();
	VirtualObj *pVirtualObj = NULL;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CRM(m_pOGLRenderProgram->UseProgram(), "Failed to use OGLProgram");

	// Send lights to shader
	std::vector<light*> *pLights = NULL;
	CR(pObjectStore->GetLights(pLights));
	CN(pLights);
	CR(m_pOGLRenderProgram->SetLights(pLights));

	m_pCamera->ResizeCamera(m_pHMD->GetEyeWidth(), m_pHMD->GetEyeHeight());

	for (int i = 0; i < 2; i++) {
		EYE_TYPE eye = (i == 0) ? EYE_LEFT : EYE_RIGHT;

		//SetStereoFramebufferViewTarget(eye);
		//SetCameraMatrix(eye);
		CR(m_pOGLRenderProgram->SetStereoCamera(m_pCamera, eye));
		m_pHMD->SetAndClearRenderSurface(eye);

		// Send SceneGraph objects to shader
		pSceneGraph->Reset();
		while ((pVirtualObj = pObjectStore->GetNextObject()) != NULL) {

			DimObj *pDimObj = dynamic_cast<DimObj*>(pVirtualObj);

			if (pDimObj == NULL)
				continue;
			else {
				CR(m_pOGLRenderProgram->RenderObject(pDimObj));
			}

		}		

		skybox *pSkybox = nullptr;
		CR(pObjectStore->GetSkybox(pSkybox));
		if (pSkybox != nullptr) {
			CRM(m_pOGLSkyboxProgram->UseProgram(), "Failed to use OGLProgram");
			CR(m_pOGLSkyboxProgram->SetStereoCamera(m_pCamera, EYE_MONO));
			CR(m_pOGLSkyboxProgram->RenderObject(pSkybox));
		}

		m_pHMD->UnsetRenderSurface(eye);
		m_pHMD->CommitSwapChain(eye);

	}

	glFlush();

Error:
	return r;
}

RESULT OpenGLImp::ShutdownImplementaiton() {
	RESULT r = R_PASS;

	//CBM((wglDeleteContext(m_hglrc)), "Failed to wglDeleteContext(hglrc)");

	/* TODO:  Add this stuff
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//--------------------------------
	m_pProgram->DetachShader(m_pVertSh);
	m_pProgram->DetachShader(m_pFragSh);

	delete m_pProgram;
	m_pProgram = NULL;

	delete m_pVertSh;
	m_pVertSh = NULL;
	delete m_pFragSh;
	m_pFragSh = NULL;

	wglMakeCurrent(NULL, NULL);
	//--------------------------------
	if (m_hrc)
	{
	wglDeleteContext(m_hrc);
	m_hrc = NULL;
	}
	*/

	if (m_pOpenGLRenderingContext != NULL) {
		delete m_pOpenGLRenderingContext;
		m_pOpenGLRenderingContext = NULL;
	}

	return r;
}

// Open GL / Wrappers

// OpenGL Program

RESULT OpenGLImp::CreateProgram(GLuint *pOGLProgramIndex) {
	RESULT r = R_PASS;

	*pOGLProgramIndex = m_OpenGLExtensions.glCreateProgram();
	CRM(CheckGLError(), "glCreateProgram failed");

Error:
	return r;
}

RESULT OpenGLImp::DeleteProgram(GLuint OGLProgramIndex) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDeleteProgram(OGLProgramIndex);
	CRM(CheckGLError(), "glDeleteProgram failed");

Error:
	return r;
}

RESULT OpenGLImp::UseProgram(GLuint OGLProgramIndex) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glUseProgram(OGLProgramIndex);
	CRM(CheckGLError(), "UseProgram failed");

Error:
	return r;
}

RESULT OpenGLImp::LinkProgram(GLuint OGLProgramIndex) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glLinkProgram(OGLProgramIndex);
	CRM(CheckGLError(), "glLinkProgram failed");

Error:
	return r;
}

RESULT OpenGLImp::glGetProgramInfoLog(GLuint programID, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramInfoLog(programID, bufSize, length, infoLog);
	CRM(CheckGLError(), "glGetProgramInfoLog failed");

Error:
	return r;
}

RESULT OpenGLImp::IsProgram(GLuint m_OGLProgramIndex) {
	RESULT r = R_PASS;

	CB((m_OpenGLExtensions.glIsProgram(m_OGLProgramIndex)));
	CRM(CheckGLError(), "glCreateProgram failed");

Error:
	return r;
}

RESULT OpenGLImp::glGetProgramiv(GLuint programID, GLenum pname, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramiv(programID, pname, params);
	CRM(CheckGLError(), "glGetProgramiv failed");

Error:
	return r;
}

RESULT OpenGLImp::glGetProgramInterfaceiv(GLuint program, GLenum programInterface, GLenum pname, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramInterfaceiv(program, programInterface, pname, params);  
	CRM(CheckGLError(), "glGetProgramInterfaceiv failed");

Error:
	return r;
}

RESULT OpenGLImp::glGetProgramResourceiv(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramResourceiv(program, programInterface, index, propCount, props, bufSize, length, params);
	CRM(CheckGLError(), "glGetProgramResourceiv failed");

Error:
	return r;
}

RESULT OpenGLImp::glGetProgramResourceName(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramResourceName(program, programInterface, index, bufSize, length, name);
	CRM(CheckGLError(), "glGetProgramResourceName failed");

Error:
	return r;
}

RESULT OpenGLImp::glGenVertexArrays(GLsizei n, GLuint *arrays) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenVertexArrays(n, arrays);  //create VAO container and get ID for it
	CRM(CheckGLError(), "glGenVertexArrays failed");

Error:
	return r;
}

// Bind Array to OpenGL context
RESULT OpenGLImp::glBindVertexArray(GLuint gluiArray) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindVertexArray(gluiArray);
	CRM(CheckGLError(), "glBindVertexArray failed");

Error:
	return r;
}

// FBO
RESULT OpenGLImp::glGenFramebuffers(GLsizei n, GLuint *framebuffers) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenFramebuffers(n, framebuffers);
	CRM(CheckGLError(), "glGenFramebuffers failed");

Error:
	return r;
}

RESULT OpenGLImp::glBindFramebuffer(GLenum target, GLuint gluiFramebuffer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindFramebuffer(target, gluiFramebuffer);
	CRM(CheckGLError(), "glBindFramebuffer failed");

Error:
	return r;
}

RESULT OpenGLImp::glGenRenderbuffers(GLsizei n, GLuint *renderbuffers) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenRenderbuffers(n, renderbuffers);
	CRM(CheckGLError(), "glGenRenderbuffers failed");

Error:
	return r;
}

RESULT OpenGLImp::glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindRenderbuffer(target, renderbuffer);
	CRM(CheckGLError(), "glBindRenderbuffer failed");

Error:
	return r;
}

RESULT OpenGLImp::glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glRenderbufferStorage(target, internalformat, width, height);
	CRM(CheckGLError(), "glRenderbufferStorage failed");

Error:
	return r;
}

RESULT OpenGLImp::glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
	CRM(CheckGLError(), "glFramebufferRenderbuffer failed");

Error:
	return r;
}

RESULT OpenGLImp::CheckFramebufferStatus(GLenum target) {
	RESULT r = R_PASS;

	GLenum glenumCheckFramebufferStatus = m_OpenGLExtensions.glCheckFramebufferStatus(target);
	CBM((glenumCheckFramebufferStatus == GL_FRAMEBUFFER_COMPLETE), "glCheckframebufferStatus failed with 0x%x", glenumCheckFramebufferStatus);
	CRM(CheckGLError(), "glFramebufferRenderbuffer failed");

Error:
	return r;
}

RESULT OpenGLImp::glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glFramebufferTexture2D(target, attachment, textarget, texture, level);
	CRM(CheckGLError(), "glFramebufferTexture2D failed");

Error:
	return r;
}

RESULT OpenGLImp::glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
	CRM(CheckGLError(), "glBlitFramebuffer failed");

Error:
	return r;
}

RESULT OpenGLImp::glDrawBuffers(GLsizei n, const GLenum *bufs) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDrawBuffers(n, bufs);
	CRM(CheckGLError(), "glFramebufferRenderbuffer failed");

Error:
	return r;
}

RESULT OpenGLImp::glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glFramebufferTexture(target, attachment, texture, level);
	CRM(CheckGLError(), "glFramebufferTexture failed");

Error:
	return r;
}

// VBO
RESULT OpenGLImp::glGenBuffers(GLsizei n, GLuint *buffers) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenBuffers(n, buffers);
	CRM(CheckGLError(), "glGenBuffers failed");

Error:
	return r;
}

RESULT OpenGLImp::glBindBuffer(GLenum target, GLuint gluiBuffer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindBuffer(target, gluiBuffer);
	CRM(CheckGLError(), "glBindBuffer failed");

Error:
	return r;
}

RESULT OpenGLImp::glDeleteBuffers(GLsizei n, const GLuint *buffers) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDeleteBuffers(n, buffers);
	CRM(CheckGLError(), "glDeleteBuffers failed");

Error:
	return r;
}

RESULT OpenGLImp::glDeleteVertexArrays(GLsizei n, const GLuint *arrays) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDeleteVertexArrays(n, arrays);
	CRM(CheckGLError(), "glDeleteVertexArrays failed");

Error:
	return r;
}

RESULT OpenGLImp::glBindAttribLocation(GLuint program, GLuint index, const GLchar *name) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindAttribLocation(program, index, name);
	CRM(CheckGLError(), "glBindAttribLocation failed");

Error:
	return r;
}

RESULT OpenGLImp::glGetAttribLocation(GLuint programID, const GLchar *pszName, GLint *pLocation) {
	RESULT r = R_PASS;

	*pLocation = m_OpenGLExtensions.glGetAttribLocation(programID, pszName);
	CRM(CheckGLError(), "glGetAttribLocation failed");

	return r;
Error:
	*pLocation = -1;
	return r;
}

RESULT OpenGLImp::glGetUniformBlockIndex(GLuint programID, const GLchar *pszName, GLint *pLocation) {
	RESULT r = R_PASS;

	*pLocation = m_OpenGLExtensions.glGetUniformBlockIndex(programID, pszName);
	CRM(CheckGLError(), "glGetUniformLocation failed");

	return r;
Error:
	*pLocation = -1;
	return r;
}

RESULT OpenGLImp::glUniformBlockBinding(GLuint programID, GLint uniformBlockIndex, GLint uniformBlockBindingPoint) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glUniformBlockBinding(programID, uniformBlockIndex, uniformBlockBindingPoint);
	CRM(CheckGLError(), "glGetUniformLocation failed");

Error:
	return r;
}

RESULT OpenGLImp::glBindBufferBase(GLenum target, GLuint bindingPointIndex, GLuint bufferIndex) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindBufferBase(target, bindingPointIndex, bufferIndex);
	CRM(CheckGLError(), "glBindBufferBase failed");

Error:
	return r;
}

RESULT OpenGLImp::glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices);
	CRM(CheckGLError(), "glGetUniformIndices failed");

Error:
	return r;
}

RESULT OpenGLImp::glGetUniformLocation(GLuint program, const GLchar *name, GLint *pLocation) {
	RESULT r = R_PASS;

	*pLocation = m_OpenGLExtensions.glGetUniformLocation(program, name);
	CRM(CheckGLError(), "glGetUniformLocation failed");

	return r;
Error:
	*pLocation = -1;
	return r;
}

RESULT OpenGLImp::glUniform1i(GLint location, GLint v0) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glUniform1i(location, v0);
	CRM(CheckGLError(), "glUniform1i failed");

Error:
	return r;
}

RESULT OpenGLImp::glUniform4fv(GLint location, GLsizei count, const GLfloat *value) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glUniform4fv(location, count, value);
	CRM(CheckGLError(), "glUniform4fv failed");

Error:
	return r;
}

RESULT OpenGLImp::glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glUniformMatrix4fv(location, count, transpose, value);
	CRM(CheckGLError(), "glUniformMatrix4fv failed");

Error:
	return r;
}

// OpenGL Shaders
RESULT OpenGLImp::CreateShader(GLenum type, GLuint *shaderID) {
	RESULT r = R_PASS;

	*shaderID = m_OpenGLExtensions.glCreateShader(type);
	CRM(CheckGLError(), "glCreateShader failed");

	return r;
Error:
	*shaderID = NULL;
	return r;
}

RESULT OpenGLImp::GetShaderiv(GLuint programID, GLenum pname, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetShaderiv(programID, pname, params);
	CRM(CheckGLError(), "glGetShaderiv failed");

Error:
	return r;
}

RESULT OpenGLImp::GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetShaderInfoLog(shader, bufSize, length, infoLog);
	CRM(CheckGLError(), "glGetShaderInfoLog failed");

Error:
	return r;
}

RESULT OpenGLImp::glAttachShader(GLuint program, GLuint shader) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glAttachShader(program, shader);
	CRM(CheckGLError(), "glAttachShader failed");

Error:
	return r;
}

RESULT OpenGLImp::ShaderSource(GLuint shaderID, GLsizei count, const GLchar *const*string, const GLint *length) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glShaderSource(shaderID, count, string, length);
	CRM(CheckGLError(), "glShaderSource failed");

Error:
	return r;
}

RESULT OpenGLImp::CompileShader(GLuint shaderID) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glCompileShader(shaderID);
	CRM(CheckGLError(), "glCompileShader failed");

Error:
	return r;
}

RESULT OpenGLImp::glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBufferData(target, size, data, usage);
	CRM(CheckGLError(), "glBufferData failed");

Error:
	return r;
}

RESULT OpenGLImp::glBufferSubData(GLenum target, GLsizeiptr offset, GLsizeiptr size, const void *data) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBufferSubData(target, offset, size, data);
	CRM(CheckGLError(), "glBufferSubData failed");

Error:
	return r;
}

RESULT OpenGLImp::glEnableVertexAtrribArray(GLuint index) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glEnableVertexAttribArray(index);
	CRM(CheckGLError(), "glEnableVertexAttribArray failed");

Error:
	return r;
}

RESULT OpenGLImp::glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	CRM(CheckGLError(), "glVertexAttribPointer failed");

Error:
	return r;
}

// Textures
RESULT OpenGLImp::GenerateTextures(GLsizei n, GLuint *textures) {
	RESULT r = R_PASS;

	//m_OpenGLExtensions.glGenTextures(n, textures);
	glGenTextures(n, textures);
	CRM(CheckGLError(), "glGenTextures failed");

Error:
	return r;
}

RESULT OpenGLImp::glActiveTexture(GLenum texture) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glActiveTexture(texture);
	CRM(CheckGLError(), "glActiveTexture failed");

Error:
	return r;
}

RESULT OpenGLImp::glBindTextures(GLuint first, GLsizei count, const GLuint *textures) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindTextures(first, count, textures);
	CRM(CheckGLError(), "glBindTextures failed");

Error:
	return r;
}

RESULT OpenGLImp::BindTexture(GLenum target, GLuint texture) {
	RESULT r = R_PASS;

	glBindTexture(target, texture);
	CRM(CheckGLError(), "glBindTexture failed");

Error:
	return r;
}

RESULT OpenGLImp::glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glTextStorage2D(target, levels, internalformat, width, height);
	CRM(CheckGLError(), "glTexStorage2D failed");

Error:
	return r;
}

RESULT OpenGLImp::TexParameteri(GLenum target, GLenum pname, GLint param) {
	RESULT r = R_PASS;

	//m_OpenGLExtensions.glTexParamteri(target, pname, param);
	glTexParameteri(target, pname, param);
	CRM(CheckGLError(), "glTexParameteri failed");

Error:
	return r;
}

RESULT OpenGLImp::TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) {
	RESULT r = R_PASS;

	//m_OpenGLExtensions.glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	CRM(CheckGLError(), "glTexImage2D failed");

Error:
	return r;
}

RESULT OpenGLImp::TextureSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels) {
	RESULT r = R_PASS;

	glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
	CRM(CheckGLError(), "glTexSubImage2D failed");

Error:
	return r;
}

RESULT OpenGLImp::glGenerateMipmap(GLenum target) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenerateMipmap(target);
	CRM(CheckGLError(), "glGenerateMipmap failed");

Error:
	return r;
}

RESULT OpenGLImp::wglSwapIntervalEXT(int interval) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.wglSwapIntervalEXT(interval);
	CRM(CheckGLError(), "wglSwapIntervalEXT failed");

Error:
	return r;
}
