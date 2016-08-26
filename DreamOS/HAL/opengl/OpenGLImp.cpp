#include "OpenGLImp.h"
#include "OGLObj.h"
#include "OGLFramebuffer.h"

#include "Primitives/ProjectionMatrix.h"
#include "Primitives/TranslationMatrix.h"
#include "Primitives/RotationMatrix.h"
#include <vector>

#include "OGLVolume.h"

#include "OGLModel.h"
#include "OGLText.h"
#include "Primitives/font.h"
#include "OGLTriangle.h"
#include "OGLQuad.h"

#include "OGLSphere.h"
#include "OGLComposite.h"
#include "Primitives/light.h"
#include "OGLTexture.h"
#include "OGLSkybox.h"
#include "OGLUser.h"
#include "OGLHand.h"


#include "OGLProfiler.h"

OpenGLImp::OpenGLImp(OpenGLRenderingContext *pOpenGLRenderingContext) :
	m_versionOGL(0),
	m_versionGLSL(0),
	m_pOGLRenderProgram(nullptr),
	m_pOGLSkyboxProgram(nullptr),
	m_pOGLOverlayProgram(nullptr),
	m_pOGLProgramCapture(nullptr),
	m_pOpenGLRenderingContext(pOpenGLRenderingContext)
{
	RESULT r = R_PASS;

	CRM(InitializeGLContext(), "Failed to Initialize OpenGL Context");
	CRM(PrepareScene(), "Failed to prepare GL Scene");

//Success:
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
//	GLenum glerr;
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

	//m_pOGLProgramShadowDepth = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_SHADOW_DEPTH, this, m_versionGLSL);
	m_pOGLProgramShadowDepth = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_SHADOW_DEPTH, this, m_versionGLSL);
	CN(m_pOGLProgramShadowDepth);
	
	// TODO(NTH): Add a program / render pipeline arch
	m_pOGLRenderProgram = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_BLINNPHONG_TEXTURE_BUMP, this, m_versionGLSL);
	//m_pOGLRenderProgram = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_MINIMAL, this, m_versionGLSL);
	//m_pOGLRenderProgram = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_BLINNPHONG, this, m_versionGLSL);
	//m_pOGLRenderProgram = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_MINIMAL_TEXTURE, this, m_versionGLSL);
	//m_pOGLRenderProgram = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_BLINNPHONG_SHADOW, this, m_versionGLSL);
	//m_pOGLRenderProgram = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_BLINNPHONG_TEXTURE_SHADOW, this, m_versionGLSL);
	CN(m_pOGLRenderProgram);
	m_pOGLRenderProgram->SetOGLProgramDepth(m_pOGLProgramShadowDepth);
	
	/*
	m_pOGLProgramCapture = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_BLINNPHONG, this, m_versionGLSL);
	//m_pOGLProgramCapture = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_MINIMAL, this, m_versionGLSL);
	CN(m_pOGLProgramCapture);
	m_pOGLProgramCapture->InitializeRenderToTexture(GL_DEPTH_COMPONENT16, GL_FLOAT, 1024, 1024, 3);
	//m_pOGLProgramCapture->InitializeFrameBuffer(GL_DEPTH_COMPONENT16, GL_FLOAT, 1024, 1024, 3);
	*/

	m_pOGLSkyboxProgram = OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_SKYBOX_SCATTER, this, m_versionGLSL);
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
	m_pCamera = new stereocamera(point(0.0f, 0.0f, -2.0f), 60.0f, m_pxViewWidth, m_pxViewHeight);
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

//Error:
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

RESULT OpenGLImp::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;

	switch (kbEvent->KeyCode) {
		case (SK_SCAN_CODE)('F') : {
			if(kbEvent->KeyState != 0)
				SetRenderProfiler(!GetRenderProfiler());
		}
	}

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

// TODO: This convenience function should be put in a model factory

composite* OpenGLImp::LoadModel(SceneGraph* pSceneGraph, const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, point_precision rotateY) {
	RESULT r = R_PASS;
	
	composite* pComposite = new composite(this);

	// Texture caching
	std::map<std::wstring, texture*> textureMap;

	// Root folder
	PathManager* pPathManager = PathManager::instance();
	wchar_t* pszPath;
	pPathManager->GetCurrentPath((wchar_t*&)pszPath);
	std::wstring strRootFolder(pszPath);

	FileLoaderHelper::multi_mesh_indices_t v;
	FileLoaderHelper::LoadOBJFile(strRootFolder + wstrOBJFilename, v);

	DEBUG_LINEOUT("Loading %S verts and indices", wstrOBJFilename.c_str());

	for (auto& m : v) {
		if (m.second.indices.size() == 0) {
			continue;
		}

		std::shared_ptr<model> pModel(MakeModel(std::move(m.second.vertices), std::move(m.second.indices)));
				
		CN(pModel);
		pComposite->AddChild(pModel);

		if (pTexture != nullptr) {
			pModel->SetColorTexture(pTexture);
		}
		else {
			std::string tex = m.first.map_Kd;
			if (tex.compare("") != 0) {
				std::wstring wstr(tex.begin(), tex.end());
				wstr = L"..\\" + wstrOBJFilename.substr(0, wstrOBJFilename.rfind('\\')) + L"\\" + wstr;

				if (textureMap.find(wstr) == textureMap.end()) {
					//OutputDebugStringW((std::wstring(L"DOS::load tex=") + wstr).c_str());
					texture *pTempTexture = new OGLTexture(this, (wchar_t*)(wstr.c_str()), texture::TEXTURE_TYPE::TEXTURE_COLOR);
					CN(pTempTexture);

					textureMap[wstr] = pTempTexture;

					if (pTempTexture->GetWidth() > 0 && pTempTexture->GetHeight() > 0) {
						//OutputDebugStringW((std::wstring(L"DOS::tex=") + wstr + L"," + std::to_wstring(pTempTexture->GetWidth()) + L"," + std::to_wstring(pTempTexture->GetHeight())).c_str());
						pModel->SetColorTexture(pTempTexture);
					}
				}
				else {
					pModel->SetColorTexture(textureMap[wstr]);
				}
			}
			else {
				pModel->SetColor(color(m.first.Kd.r(), m.first.Kd.g(), m.first.Kd.b(), m.first.Kd.a()));
			}
		}

		pModel->Scale(scale);
		pModel->MoveTo(ptPosition);
		pModel->RotateYBy(rotateY);
		//pModel->UpdateOGLBuffers();
		if (pSceneGraph != nullptr) {
			pSceneGraph->PushObject(pModel.get());
		}
	}

Error:
	return pComposite;
}

model *OpenGLImp::MakeModel(wchar_t *pszModelName) {
	RESULT r = R_PASS;

	model *pModel = new OGLModel(this, pszModelName);
	CN(pModel);

//Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}
	return nullptr;
}


model *OpenGLImp::MakeModel(const std::vector<vertex>& vertices) {
	RESULT r = R_PASS;

	model *pModel = new OGLModel(this, vertices);
	CN(pModel);

	//Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}
	return nullptr;
}

model *OpenGLImp::MakeModel(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
	RESULT r = R_PASS;

	// Not implemented yet, until size_t <-> dimindex conflict is resolved.
	model *pModel = new OGLModel(this, vertices, indices);
	CN(pModel);

	//Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}
	return nullptr;
}

composite *OpenGLImp::MakeComposite() {
	RESULT r = R_PASS;

	composite *pComposite = new OGLComposite(this);
	CN(pComposite);

//Success:
	return pComposite;

Error:
	if (pComposite != nullptr) {
		delete pComposite;
		pComposite = nullptr;
	}
	return nullptr;
}

user *OpenGLImp::MakeUser() {
	RESULT r = R_PASS;

	user *pUser = new OGLUser(this);
	CN(pUser);

	//Success:
	return pUser;

Error:
	if (pUser != nullptr) {
		delete pUser;
		pUser = nullptr;
	}
	return nullptr;
}

// TODO: Other approach 
light* OpenGLImp::MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	RESULT r = R_PASS;

	light *pLight = new light(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
	CN(pLight);

//Success:
	return pLight;

Error:
	if (pLight != nullptr) {
		delete pLight;
		pLight = nullptr;
	}
	return nullptr;
}

quad* OpenGLImp::MakeQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight) {
	RESULT r = R_PASS;

	quad *pQuad = new OGLQuad(this, static_cast<float>(width), static_cast<float>(height), numHorizontalDivisions, numVerticalDivisions, pTextureHeight);
	CN(pQuad);

//Success:
	return pQuad;

Error:
	if (pQuad != nullptr) {
		delete pQuad;
		pQuad = nullptr;
	}
	return nullptr;
}

sphere* OpenGLImp::MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE)) {
	RESULT r = R_PASS;

	sphere *pSphere = new OGLSphere(this, radius, numAngularDivisions, numVerticalDivisions, c);
	CN(pSphere);

//Success:
	return pSphere;

Error:
	if (pSphere != nullptr) {
		delete pSphere;
		pSphere = nullptr;
	}
	return nullptr;
}

composite* OpenGLImp::MakeModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, point_precision rotateY) {
	RESULT r = R_PASS;

	composite *pModel = LoadModel(nullptr, wstrOBJFilename, pTexture, ptPosition, scale, rotateY);
	CN(pModel);

	//Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}
	return nullptr;
}

hand* OpenGLImp::MakeHand() {
	RESULT r = R_PASS;

	hand *pHand = new OGLHand(this);
	CN(pHand);

	//Success:
	return pHand;

Error:
	if (pHand != nullptr) {
		delete pHand;
		pHand = nullptr;
	}
	return nullptr;
}

volume* OpenGLImp::MakeVolume(double width, double length, double height) {
	RESULT r = R_PASS;

	volume *pVolume = new OGLVolume(this, width, length, height);
	CN(pVolume);

//Success:
	return pVolume;

Error:
	if (pVolume != nullptr) {
		delete pVolume;
		pVolume = nullptr;
	}
	return nullptr;
}

volume* OpenGLImp::MakeVolume(double side) {
	return MakeVolume(side, side, side);
}

text* OpenGLImp::MakeText(const std::wstring& fontName, const std::string& content, double size, bool isBillboard)
{
	RESULT r = R_PASS;

	text *pText = new OGLText(this, std::make_shared<Font>(fontName), content, size, isBillboard);
	CN(pText);

//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}
	return nullptr;
}

texture* OpenGLImp::MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) {
	RESULT r = R_PASS;

	texture *pTexture = new OGLTexture(this, pszFilename, type);
	CN(pTexture);

//Success:
	return pTexture;

Error:
	if (pTexture != nullptr) {
		delete pTexture;
		pTexture = nullptr;
	}
	return nullptr;
}

texture* OpenGLImp::MakeTexture(texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n) {
	RESULT r = R_PASS;

	texture *pTexture = new OGLTexture(this, type, width, height, channels, pBuffer, pBuffer_n);
	CN(pTexture);

	//Success:
	return pTexture;

Error:
	if (pTexture != nullptr) {
		delete pTexture;
		pTexture = nullptr;
	}
	return nullptr;
}

skybox *OpenGLImp::MakeSkybox() {
	RESULT r = R_PASS;

	skybox *pSkybox = new OGLSkybox(this);
	CN(pSkybox);

//Success:
	return pSkybox;

Error:
	if (pSkybox != nullptr) {
		delete pSkybox;
		pSkybox = nullptr;
	}
	return nullptr;
}

RESULT OpenGLImp::Render(SceneGraph *pSceneGraph) {
	RESULT r = R_PASS;
	SceneGraphStore *pObjectStore = pSceneGraph->GetSceneGraphStore();
	VirtualObj *pVirtualObj = NULL;

	std::vector<light*> *pLights = NULL;
	CR(pObjectStore->GetLights(pLights));
	CN(pLights);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CheckFramebufferStatus(GL_FRAMEBUFFER);

	// TODO: Temporary go through scene graph again
	/*
	m_pOGLProgramCapture->UseProgram();
	CR(m_pOGLProgramCapture->SetLights(pLights));
	m_pOGLProgramCapture->BindToFramebuffer();
	CR(m_pOGLProgramCapture->SetCamera(m_pCamera));

	pSceneGraph->Reset();
	while ((pVirtualObj = pObjectStore->GetNextObject()) != NULL) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pVirtualObj);

		if (pDimObj == NULL)
			continue;
		else {
			CR(m_pOGLProgramCapture->RenderObject(pDimObj));
		}
	}

	CR(m_pOGLProgramCapture->UnbindFramebuffer());
	//*/

	m_pOGLProgramShadowDepth->UseProgram();
	m_pOGLProgramShadowDepth->BindToDepthBuffer();
	CR(m_pOGLProgramShadowDepth->SetCamera(m_pCamera));
	CR(m_pOGLProgramShadowDepth->SetLights(pLights));
	CR(m_pOGLProgramShadowDepth->RenderSceneGraph(pSceneGraph));
	m_pOGLProgramShadowDepth->UnbindFramebuffer();

	///*
	CRM(m_pOGLRenderProgram->UseProgram(), "Failed to use OGLProgram");
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
	if (GetRenderProfiler()) {
		CRM(m_pOGLProfiler->m_OGLProgram->UseProgram(), "Failed to use OGLProgram");
		CR(m_pOGLProfiler->m_OGLProgram->SetCamera(m_pCamera));
		m_pOGLProfiler->Render();
	}

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

		CRM(m_pOGLRenderProgram->UseProgram(), "Failed to use OGLProgram");

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

		// Render profiler overlay
		if (GetRenderProfiler()) {
			CRM(m_pOGLProfiler->m_OGLProgram->UseProgram(), "Failed to use OGLProgram");
			CR(m_pOGLProfiler->m_OGLProgram->SetStereoCamera(m_pCamera, eye));
			m_pOGLProfiler->Render();
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

RESULT OpenGLImp::RenderStereoFramebuffersFlat(SceneGraph *pFlatSceneGraph) {
	RESULT r = R_PASS;
	SceneGraphStore *pObjectStore = pFlatSceneGraph->GetSceneGraphStore();
	VirtualObj *pVirtualObj = NULL;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_pCamera->ResizeCamera(m_pHMD->GetEyeWidth(), m_pHMD->GetEyeHeight());

	for (int i = 0; i < 2; i++) {
		EYE_TYPE eye = (i == 0) ? EYE_LEFT : EYE_RIGHT;

		CRM(m_pOGLProfiler->m_OGLProgram->UseProgram(), "Failed to use OGLProgram");
		CR(m_pOGLProfiler->m_OGLProgram->SetStereoCamera(m_pCamera, eye));

		pFlatSceneGraph->Reset();
		while ((pVirtualObj = pObjectStore->GetNextObject()) != NULL) {

			DimObj *pDimObj = dynamic_cast<DimObj*>(pVirtualObj);

			if (pDimObj == NULL)
				continue;
			else {
				CR(m_pOGLOverlayProgram->RenderObject(pDimObj));
			}
		}		

		m_pHMD->UnsetRenderSurface(eye);
		m_pHMD->CommitSwapChain(eye);
	}

	glFlush();

Error:
	return r;
}

// TODO: Naming is kind of lame since this hits the HMD
// TODO: Shared code should be consolidated
RESULT OpenGLImp::RenderStereoFramebuffers(SceneGraph *pSceneGraph) {
	RESULT r = R_PASS;
	SceneGraphStore *pObjectStore = pSceneGraph->GetSceneGraphStore();
	VirtualObj *pVirtualObj = NULL;
	
	// Send lights to shader
	std::vector<light*> *pLights = NULL;
	CR(pObjectStore->GetLights(pLights));
	CN(pLights);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO: Temporary go through scene graph again
	/*
	m_pOGLProgramCapture->UseProgram();
	CR(m_pOGLProgramCapture->SetLights(pLights));
	//SetMonoViewTarget();
	//m_pOGLProgramCapture->BindToDepthBuffer();
	m_pOGLProgramCapture->BindToFramebuffer();
	CR(m_pOGLProgramCapture->SetCamera(m_pCamera));

	pSceneGraph->Reset();
	while ((pVirtualObj = pObjectStore->GetNextObject()) != NULL) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pVirtualObj);

		if (pDimObj == NULL)
			continue;
		else {
			CR(m_pOGLProgramCapture->RenderObject(pDimObj));
		}
	}

	CR(m_pOGLProgramCapture->UnbindFramebuffer());
	//*/

	m_pOGLProgramShadowDepth->UseProgram();
	m_pOGLProgramShadowDepth->BindToDepthBuffer();
	CR(m_pOGLProgramShadowDepth->SetCamera(m_pCamera));
	CR(m_pOGLProgramShadowDepth->SetLights(pLights));
	CR(m_pOGLProgramShadowDepth->RenderSceneGraph(pSceneGraph));
	m_pOGLProgramShadowDepth->UnbindFramebuffer();

	CRM(m_pOGLRenderProgram->UseProgram(), "Failed to use OGLProgram");
	CR(m_pOGLRenderProgram->SetLights(pLights));

	m_pCamera->ResizeCamera(m_pHMD->GetEyeWidth(), m_pHMD->GetEyeHeight());

	for (int i = 0; i < 2; i++) {
		EYE_TYPE eye = (i == 0) ? EYE_LEFT : EYE_RIGHT;
		CRM(m_pOGLRenderProgram->UseProgram(), "Failed to use OGLProgram");

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
			CR(m_pOGLSkyboxProgram->SetStereoCamera(m_pCamera, eye));
			CR(m_pOGLSkyboxProgram->RenderObject(pSkybox));
		}
		
		// Render profiler overlay
		if (GetRenderProfiler()) {
			CRM(m_pOGLProfiler->m_OGLProgram->UseProgram(), "Failed to use OGLProgram");
			CR(m_pOGLProfiler->m_OGLProgram->SetStereoCamera(m_pCamera, eye));
			m_pOGLProfiler->Render();
		}

		m_pHMD->UnsetRenderSurface(eye);
		m_pHMD->CommitSwapChain(eye);
	}

	glFlush();

Error:
	return r;
}

RESULT OpenGLImp::Shutdown() {
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

RESULT OpenGLImp::glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
	CRM(CheckGLError(), "glRenderbufferStorageMultisample failed");

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

RESULT OpenGLImp::glTexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glTexImage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations);
	CRM(CheckGLError(), "glTexImage2DMultisample failed");

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
