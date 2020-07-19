#include "OGLImp.h"

#include <vector>
#include <algorithm>

#include "logger/DreamLogger.h"

#include "easylogging++.h"

#include "OGLObj.h"
#include "OGLFramebuffer.h"

#include "core/Utilities.h"

#include "core/matrix/ProjectionMatrix.h"
#include "core/matrix/TranslationMatrix.h"
#include "core/matrix/RotationMatrix.h"

#include "core/text/font.h"

#include "OGLVolume.h"

#include "OGLMesh.h"
#include "OGLText.h"
#include "OGLTriangle.h"
#include "OGLQuad.h"

#include "OGLSphere.h"
#include "OGLCylinder.h"
#include "OGLComposite.h"
#include "OGLFlatContext.h"
#include "OGLModel.h"
#include "core/primitives/light.h"
#include "OGLTexture.h"
#include "OGLCubemap.h"
#include "OGLSkybox.h"
#include "OGLUser.h"
#include "OGLHand.h"
#include "OGLRay.h"
#include "OGLPlane.h"
#include "OGLAttachment.h"
#include "OGLBillboard.h"

#include "OGLViewportDisplay.h"
#include "OGLCameraViewportDisplay.h"

OGLImp::OGLImp(OGLRenderingContext *pOpenGLRenderingContext) :
	m_versionOGL(0),
	m_versionGLSL(0),
	m_pOpenGLRenderingContext(pOpenGLRenderingContext)
{
	RESULT r = R_PASS;

//Success:
	Validate();
	return;

//Error:
	Invalidate();
	return;
}

OGLImp::~OGLImp() {
	// empty
}

RESULT OGLImp::InitializeOpenGLVersion() {
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

	//m_versionOGL.SetVersion(vMajor, vMinor);
	m_versionOGL.SetVersion(4, 4);

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
RESULT OGLImp::CreateGLProgram() {
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

RESULT OGLImp::InitializeGLContext() {
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

RESULT OGLImp::CheckGLError() {
	RESULT r = R_PASS;

	GLenum glerr = glGetError();
	switch (glerr) {
		case GL_NO_ERROR: {
			return R_PASS;
		} break;

		case GL_INVALID_ENUM: {
			CBRM(false, R_FAIL, "CheckGLError: GL_INVALID_ENUM");
		} break;

		case GL_INVALID_VALUE: {
			CBRM(false, R_FAIL, "CheckGLError: GL_INVALID_VALUE");
		} break;

		case GL_INVALID_OPERATION: {
			CBRM(false, R_FAIL, "CheckGLError: GL_INVALID_OPERATION");
		} break;

		case GL_STACK_OVERFLOW: {
			CBRM(false, R_FAIL, "CheckGLError: GL_STACK_OVERFLOW");
		} break;

		case GL_STACK_UNDERFLOW: {
			CBRM(false, R_FAIL, "CheckGLError: GL_STACK_UNDERFLOW");
		} break;

		case GL_OUT_OF_MEMORY: {
			CBRM(false, R_FAIL, "CheckGLError: GL_OUT_OF_MEMORY");
		} break;
	}

Error:
	return r;
}

RESULT OGLImp::BindBufferBase(GLenum target, GLuint bindingPointIndex, GLuint bufferIndex) {
	RESULT r = R_PASS;
//	GLenum glerr;
	DWORD werr;

	CR(glBindBufferBase(target, bindingPointIndex, bufferIndex));

	werr = GetLastError();
	DEBUG_LINEOUT("Bound uniform block binding point %d to base buffer %d err:0x%x", bindingPointIndex, bufferIndex, werr);

Error:
	return r;
}

RESULT OGLImp::MakeCurrentContext() {
	if (m_fCurrentContext == false) {
		m_fCurrentContext = true;
		return m_pOpenGLRenderingContext->MakeCurrentContext();
	}
	else {
		return R_SKIPPED;
	}
}

RESULT OGLImp::ReleaseCurrentContext() {
	if (m_fCurrentContext == true) {
		m_fCurrentContext = false;
		return m_pOpenGLRenderingContext->ReleaseCurrentContext();
	}
	else {
		return R_SKIPPED;
	}
}

RESULT OGLImp::InitializeHAL() {
	RESULT r = R_PASS;

	CRM(InitializeGLContext(), "Failed to Initialize OpenGL Context");

	CR(MakeCurrentContext());

	// Clear Background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Depth testing
	glEnable(GL_DEPTH_TEST);	// Enable depth test
	glDepthFunc(GL_LEQUAL);		// Accept fragment if it closer to the camera than the former one

								// Face culling
#define _CULL_BACK_FACES
#ifdef _CULL_BACK_FACES
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
#else
	glDisable(GL_CULL_FACE);
#endif

	// Dithering 
	glEnable(GL_DITHER);

	// Blending
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CR(ReleaseCurrentContext());

Error:
	return r;
}

RESULT OGLImp::Resize(viewport newViewport) {
	RESULT r = R_PASS;

	CR(m_pOpenGLRenderingContext->MakeCurrentContext());

	SetViewport(newViewport);
	glViewport(0, 0, (GLsizei)newViewport.Width(), (GLsizei)newViewport.Height());


Error:
	//CR(m_pOpenGLRenderingContext->ReleaseCurrentContext());

	return r;
}

// Assumes Context Current
// TODO: This should move to a sink node
RESULT OGLImp::SetViewTarget(EYE_TYPE eye, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	// Render to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	SetViewport(pxWidth, pxHeight);

	switch (eye) {
		case EYE_LEFT: {
			glViewport(0, 0, (GLsizei)pxWidth / 2, (GLsizei)pxHeight);
		} break;

		case EYE_RIGHT: {
			glViewport((GLsizei)pxWidth / 2, 0, (GLsizei)pxWidth / 2, (GLsizei)pxHeight);
		} break;

		case EYE_MONO: {
			glViewport(0, 0, (GLsizei)pxWidth, (GLsizei)pxHeight);
		} break;
	}

	// TODO: Do this in the sandbox when choosing eye - do we need the above then?
	/*
	(eye != EYE_MONO) ? m_pCamera->ResizeCamera(pxWidth/2.0f, pxHeight) :
						m_pCamera->ResizeCamera(pxWidth, pxHeight);
						*/

	return r;
}

/*
RESULT OGLImp::Notify(SenseMouseEvent *mEvent) {
	RESULT r = R_PASS;

	//SenseMouse::PrintEvent(mEvent);

	switch (mEvent->EventType) {
		case SENSE_MOUSE_LEFT_DRAG_MOVE: {
			CR(m_pCamera->RotateCameraByDiffXY(static_cast<camera_precision>(mEvent->dx),  static_cast<camera_precision>(mEvent->dy)));
		} break;

		case SENSE_MOUSE_RIGHT_DRAG_MOVE: {
			if (m_pCamera->IsAllowedMoveByKeys()) {
				const float mouseMoveFactor = 0.002f;
				m_pCamera->MoveStrafe(mEvent->dx * mouseMoveFactor);
				m_pCamera->MoveUp(-mEvent->dy * mouseMoveFactor);
			}
		} break;
	}

Error:
	return r;
}
*/

// This allows for separate HAL initialization of objects
// Note, this will clobber the dim object on failure
RESULT OGLImp::InitializeObject(DimObj *pDimObj) {
	RESULT r = R_PASS;

	OGLObj *pOGLObj = dynamic_cast<OGLObj*>(pDimObj);
	CN(pOGLObj);

	CR(pOGLObj->OGLInitialize());

Success:
	return r;

Error:
	if (pDimObj != nullptr) {
		delete pDimObj;
		pDimObj = nullptr;
	}

	return r;
}

RESULT OGLImp::InitializeTexture(texture *pTexture) {
	RESULT r = R_PASS;

	OGLTexture *pOGLTexture = dynamic_cast<OGLTexture*>(pTexture);
	CN(pOGLTexture);

	CR(pOGLTexture->OGLInitialize());

Success:
	return r;

Error:
	if (pOGLTexture != nullptr) {
		delete pOGLTexture;
		pOGLTexture = nullptr;
	}

	return r;
}

DimObj* OGLImp::MakeObject(PrimParams *pPrimParams, bool fInitialize) {
	RESULT r = R_PASS;
	OGLObj *pOGLObj = nullptr;

	switch (pPrimParams->GetPrimitiveType()) {
		case PRIMITIVE_TYPE::SPHERE: {
			pOGLObj = MakeSphere(pPrimParams, fInitialize);
			CN(pOGLObj);
		} break;

		case PRIMITIVE_TYPE::VOLUME: {
			pOGLObj = MakeVolume(pPrimParams, fInitialize);
			CN(pOGLObj);
		} break;

		case PRIMITIVE_TYPE::QUAD: {
			pOGLObj = MakeQuad(pPrimParams, fInitialize);
			CN(pOGLObj);
		} break;

		//case PRIMITIVE_TYPE::MODEL: {
		//	pOGLObj = MakeModel(pPrimParams, fInitialize);
		//	CN(pOGLObj);
		//} break;

		case PRIMITIVE_TYPE::MESH: {
			pOGLObj = MakeMesh(pPrimParams, fInitialize);
			CN(pOGLObj);
		} break;
	}

Success:
	return pOGLObj;

Error:
	if (pOGLObj != nullptr) {
		delete pOGLObj;
		pOGLObj = nullptr;
	}

	return nullptr;
}

texture* OGLImp::MakeTexture(PrimParams *pPrimParams, bool fInitialize) {
	RESULT r = R_PASS;

	OGLTexture *pOGLTexture = nullptr;
	texture::params *pTextureParams = nullptr;

	CBM((pPrimParams->GetPrimitiveType() == PRIMITIVE_TYPE::TEXTURE), "Invalid Texture Params");

	pTextureParams = dynamic_cast<texture::params*>(pPrimParams);
	CN(pTextureParams);

	pOGLTexture = new OGLTexture(this, pTextureParams);
	CN(pOGLTexture);

	if (pTextureParams->pszFilename != nullptr) {
		CR(pOGLTexture->LoadTextureFromFile(pTextureParams->pszFilename));
	}
	else {
		CBM((false), "Currently MakeTexture PrimParam path only supports path based textures")
	}

	if (fInitialize) {
		CR(pOGLTexture->OGLInitialize());
	}

Success:
	return pOGLTexture;

Error:
	if (pOGLTexture != nullptr) {
		delete pOGLTexture;
		pOGLTexture = nullptr;
	}

	return nullptr;

}

OGLMesh* OGLImp::MakeMesh(PrimParams *pPrimParams, bool fInitialize) {
	RESULT r = R_PASS;

	OGLMesh *pOGLMesh = nullptr;

	mesh::params *pMeshParams = dynamic_cast<mesh::params*>(pPrimParams);
	CN(pMeshParams);

	pOGLMesh = new OGLMesh(this, pMeshParams);
	CN(pOGLMesh);

	if (fInitialize) {
		CR(pOGLMesh->OGLInitialize());
	}

Success:
	return pOGLMesh;

Error:
	if (pOGLMesh != nullptr) {
		delete pOGLMesh;
		pOGLMesh = nullptr;
	}

	return nullptr;
}

mesh *OGLImp::MakeMesh(const std::vector<vertex>& vertices) {
	RESULT r = R_PASS;

	OGLMesh *pOGLMesh = new OGLMesh(this, vertices);
	CN(pOGLMesh);

	CR(pOGLMesh->OGLInitialize());

Success:
	return pOGLMesh;

Error:
	if (pOGLMesh != nullptr) {
		delete pOGLMesh;
		pOGLMesh = nullptr;
	}
	return nullptr;
}

mesh *OGLImp::MakeMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
	RESULT r = R_PASS;

	// Not implemented yet, until size_t <-> dimindex conflict is resolved.
	OGLMesh *pOGLMesh = new OGLMesh(this, vertices, indices);
	CN(pOGLMesh);

	CR(pOGLMesh->OGLInitialize());

Success:
	return pOGLMesh;

Error:
	if (pOGLMesh != nullptr) {
		delete pOGLMesh;
		pOGLMesh = nullptr;
	}
	return nullptr;
}

model* OGLImp::MakeModel() {
	RESULT r = R_PASS;

	model *pModel = new OGLModel(this);
	CN(pModel);

Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}
	return nullptr;
}

composite *OGLImp::MakeComposite() {
	RESULT r = R_PASS;

	composite *pComposite = new OGLComposite(this);
	CN(pComposite);

Success:
	return pComposite;

Error:
	if (pComposite != nullptr) {
		delete pComposite;
		pComposite = nullptr;
	}
	return nullptr;
}

FlatContext *OGLImp::MakeFlatContext(int pxFBWidth, int pxFBHeight, int fbChannels) {
	RESULT r = R_PASS;

	FlatContext *pFlatContext = new OGLFlatContext(this);
	OGLFramebuffer *pOGLFramebuffer = new OGLFramebuffer(this, pxFBWidth, pxFBHeight, fbChannels);
	CN(pOGLFramebuffer);

	pFlatContext->SetFramebuffer(pOGLFramebuffer);
	CR(pOGLFramebuffer->OGLInitialize());
	CR(pOGLFramebuffer->Bind());

	CR(pOGLFramebuffer->MakeColorAttachment());
	CR(pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
	CR(pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));
	CR(CheckFramebufferStatus(GL_FRAMEBUFFER));

	CN(pFlatContext);

//Success:
	return pFlatContext;

Error:
	if (pFlatContext!= nullptr) {
		delete pFlatContext;
		pFlatContext = nullptr;
	}
	return nullptr;
}

dosuser *OGLImp::MakeUser() {
	RESULT r = R_PASS;

	dosuser *pUser = new OGLUser(this);
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

billboard *OGLImp::MakeBillboard(point ptOrigin, float width, float height) {
	RESULT r = R_PASS;

	billboard *pBillboard = new OGLBillboard(this, ptOrigin, width, height);
	CN(pBillboard);

	return pBillboard;
Error:
	if (pBillboard != nullptr) {
		delete pBillboard;
		pBillboard = nullptr;
	}
	return nullptr;
}

// TODO: Other approach 
light* OGLImp::MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
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

// Quad

OGLQuad* OGLImp::MakeQuad(PrimParams *pPrimParams, bool fInitialize) {
	RESULT r = R_PASS;

	OGLQuad *pOGLQuad = nullptr;

	quad::params *pQuadParams = dynamic_cast<quad::params*>(pPrimParams);
	CN(pQuadParams);

	pOGLQuad = new OGLQuad(this, pQuadParams);
	CN(pOGLQuad);

	if (fInitialize) {
		CR(pOGLQuad->OGLInitialize());
	}

Success:
	return pOGLQuad;

Error:
	if (pOGLQuad != nullptr) {
		delete pOGLQuad;
		pOGLQuad = nullptr;
	}

	return nullptr;
}

OGLModel* OGLImp::MakeModel(PrimParams *pPrimParams, bool fInitialize) {
	RESULT r = R_PASS;

	OGLModel *pOGLModel = nullptr;

	model::params *pQuadParams = dynamic_cast<model::params*>(pPrimParams);
	CN(pQuadParams);

	pOGLModel = new OGLModel(this);
	CN(pOGLModel);

	if (fInitialize) {
		CR(pOGLModel->OGLInitialize());
	}

Success:
	return pOGLModel;

Error:
	if (pOGLModel != nullptr) {
		delete pOGLModel;
		pOGLModel = nullptr;
	}

	return nullptr;
}

quad* OGLImp::MakeQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) {
	RESULT r = R_PASS;

	OGLQuad *pOGLQuad = new OGLQuad(this, static_cast<float>(width), static_cast<float>(height), numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal);
	CN(pOGLQuad);

	CR(pOGLQuad->OGLInitialize());

Success:
	return pOGLQuad;

Error:
	if (pOGLQuad != nullptr) {
		delete pOGLQuad;
		pOGLQuad = nullptr;
	}
	return nullptr;
}
 
// TODO: This is a stupid thing should be removed
//quad* OGLImp::MakeQuad(double width, double height, point ptOrigin, vector vNormal) {
//	RESULT r = R_PASS;
//
//	OGLQuad* pOGLQuad = new OGLQuad(this, static_cast<float>(width), static_cast<float>(height), 1, 1, nullptr, vNormal);
//	
//	CR(pOGLQuad->OGLInitialize());
//	
//	//pQuad->RotateXByDeg(90.0f);
//	pOGLQuad->MoveTo(ptOrigin);
//
//	CN(pOGLQuad);
//
//Success:
//	return pOGLQuad;
//
//Error:
//	if (pOGLQuad != nullptr) {
//		delete pOGLQuad;
//		pOGLQuad = nullptr;
//	}
//	return nullptr;
//}

// TODO: Origin should not be baked into these calls (done at client)
quad* OGLImp::MakeQuad(double width, double height, point ptCenter, uvcoord uvTopLeft, uvcoord uvBottomRight, vector vNormal) {
	RESULT r = R_PASS;

	OGLQuad* pOGLQuad = new OGLQuad(this, static_cast<float>(width), static_cast<float>(height), ptCenter, uvTopLeft, uvBottomRight, vNormal);
	CN(pOGLQuad);

	CR(pOGLQuad->OGLInitialize());

Success:
	return pOGLQuad;

Error:
	if (pOGLQuad != nullptr) {
		delete pOGLQuad;
		pOGLQuad = nullptr;
	}

	return nullptr;
}

quad* OGLImp::MakeQuad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType, vector vNormal) {
	RESULT r = R_PASS;

	OGLQuad* pOGLQuad = new OGLQuad(this, static_cast<float>(width), static_cast<float>(height), numHorizontalDivisions, numVerticalDivisions, uvTopLeft, uvBottomRight, curveType, vNormal);
	CN(pOGLQuad);	

	CR(pOGLQuad->OGLInitialize());

Success:
	return pOGLQuad;

Error:
	if (pOGLQuad != nullptr) {
		delete pOGLQuad;
		pOGLQuad = nullptr;
	}

	return nullptr;
}

cylinder* OGLImp::MakeCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) {
	RESULT r = R_PASS;

	OGLCylinder *pOGLCylinder = new OGLCylinder(this, radius, height, numAngularDivisions, numVerticalDivisions);
	CN(pOGLCylinder);

Success:
	return pOGLCylinder;

Error:
	if (pOGLCylinder != nullptr) {
		delete pOGLCylinder;
		pOGLCylinder = nullptr;
	}
	return nullptr;
}

DimRay* OGLImp::MakeRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	RESULT r = R_PASS;

	OGLRay *pOGLRay = new OGLRay(this, ptOrigin, vDirection, step, fDirectional);
	CN(pOGLRay);

Success:
	return pOGLRay;

Error:
	if (pOGLRay != nullptr) {
			delete pOGLRay;
			pOGLRay = nullptr;
		}
	return nullptr;
}

DimPlane* OGLImp::MakePlane(point ptOrigin, vector vNormal) {
	RESULT r = R_PASS;

	DimPlane *pPlane = new OGLPlane(this, ptOrigin, vNormal);
	CN(pPlane);

Success:
	return pPlane;

Error:
	if (pPlane != nullptr) {
		delete pPlane;
		pPlane = nullptr;
	}
	return nullptr;
}

OGLSphere* OGLImp::MakeSphere(PrimParams *pPrimParams, bool fInitialize) {
	RESULT r = R_PASS;

	OGLSphere *pOGLSphere = nullptr;

	sphere::params *pSphereParams = dynamic_cast<sphere::params*>(pPrimParams);
	CN(pSphereParams);

	pOGLSphere = new OGLSphere(this, pSphereParams);
	CN(pOGLSphere);

	if (fInitialize) {
		CR(pOGLSphere->OGLInitialize());
	}

Success:
	return pOGLSphere;

Error:
	if (pOGLSphere != nullptr) {
		delete pOGLSphere;
		pOGLSphere = nullptr;
	}

	return nullptr;
}

sphere* OGLImp::MakeSphere(float radius = 1.0f, int numAngularDivisions = 10, int numVerticalDivisions = 10, color c = color(COLOR_WHITE)) {
	RESULT r = R_PASS;

	OGLSphere *pOGLSphere = new OGLSphere(this, radius, numAngularDivisions, numVerticalDivisions, c);
	CN(pOGLSphere);

	CR(pOGLSphere->OGLInitialize());

Success:
	return pOGLSphere;

Error:
	if (pOGLSphere != nullptr) {
		delete pOGLSphere;
		pOGLSphere = nullptr;
	}

	return nullptr;
}

/*
composite* OGLImp::MakeModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, vector vEulerRotation) {
	RESULT r = R_PASS;

	composite *pModel = LoadModel(nullptr, wstrOBJFilename, pTexture, ptPosition, scale, vEulerRotation);
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
*/

hand* OGLImp::MakeHand(HAND_TYPE type) {
	RESULT r = R_PASS;

	hand *pHand = new OGLHand(this, type);
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

hand* OGLImp::MakeHand(HAND_TYPE type, long avatarID) {
	RESULT r = R_PASS;

	hand *pHand = new OGLHand(this, type, avatarID);
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

volume* OGLImp::MakeVolume(double width, double length, double height, bool fTriangleBased) {
	RESULT r = R_PASS;

	OGLVolume *pOGLVolume = new OGLVolume(this, width, length, height, fTriangleBased);
	CN(pOGLVolume);

	CR(pOGLVolume->OGLInitialize());

Success:
	return pOGLVolume;

Error:
	if (pOGLVolume != nullptr) {
		delete pOGLVolume;
		pOGLVolume = nullptr;
	}
	return nullptr;
}

OGLVolume* OGLImp::MakeVolume(PrimParams *pPrimParams, bool fInitialize) {
	RESULT r = R_PASS;

	OGLVolume *pOGLVolume = nullptr;

	volume::params *pVolumeParams = dynamic_cast<volume::params*>(pPrimParams);
	CN(pVolumeParams);

	pOGLVolume = new OGLVolume(this, pVolumeParams);
	CN(pOGLVolume);

	if (fInitialize) {
		CR(pOGLVolume->OGLInitialize());
	}

Success:
	return pOGLVolume;

Error:
	if (pOGLVolume != nullptr) {
		delete pOGLVolume;
		pOGLVolume = nullptr;
	}

	return nullptr;
}

volume* OGLImp::MakeVolume(double side, bool fTriangleBased) {
	return MakeVolume(side, side, side, fTriangleBased);
}

//TODO: the texture could be stored in the font already, but having this pathway
// avoids conflicts with parts of the code that use fonts without setting the texture
text* OGLImp::MakeText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& strContent, double width, double height, bool fDistanceMap, bool fBillboard) {
	RESULT r = R_PASS;

	text *pText = new OGLText(this, pFont, pFontTexture, strContent, width, height, fBillboard);
	CN(pText);

	CR(pText->SetText(strContent));

	int fbWidth = pText->GetDPM(width);
	int fbHeight = pText->GetDPM(height);

	// TODO: Switch to this with C++17
	//std::clamp(fbWidth, 32, 2048);
	//std::clamp(fbHeight, 32, 2048);

	util::Clamp(fbWidth, 32, 2048);
	util::Clamp(fbHeight, 32, 2048);

	OGLFramebuffer *pOGLFramebuffer = new OGLFramebuffer(this, fbWidth, fbHeight, 4);
	CN(pOGLFramebuffer);

	pText->SetFramebuffer(pOGLFramebuffer);

	CR(pOGLFramebuffer->OGLInitialize());
	CR(pOGLFramebuffer->Bind());

	CR(pOGLFramebuffer->MakeColorAttachment());
	CR(pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
	CR(pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	CR(CheckFramebufferStatus(GL_FRAMEBUFFER));

	CR(pText->SetDiffuseTexture(pFont->GetTexture().get()));

//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}
	return nullptr;
}

text* OGLImp::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width, double height, bool fDistanceMap, bool fBillboard) {
	RESULT r = R_PASS;

	text *pText = new OGLText(this, pFont, strContent, width, height, fBillboard);
	CN(pText);

	CR(pText->SetText(strContent));

	int fbWidth = pText->GetDPM(width);
	int fbHeight = pText->GetDPM(height);

	// TODO: Switch to this with C++17
	//std::clamp(fbWidth, 32, 2048);
	//std::clamp(fbHeight, 32, 2048);

	util::Clamp(fbWidth, 32, 2048);
	util::Clamp(fbHeight, 32, 2048);

	OGLFramebuffer *pOGLFramebuffer = new OGLFramebuffer(this, fbWidth, fbHeight, 4);
	CN(pOGLFramebuffer);

	pText->SetFramebuffer(pOGLFramebuffer);
	
	CR(pOGLFramebuffer->OGLInitialize());
	CR(pOGLFramebuffer->Bind());

	CR(pOGLFramebuffer->MakeColorAttachment());
	CR(pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
	CR(pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	CR(CheckFramebufferStatus(GL_FRAMEBUFFER));

	CR(pText->SetDiffuseTexture(pFont->GetTexture().get()));

//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}
	return nullptr;
}

text* OGLImp::MakeText(const std::wstring& strFontFileName, const std::string& strContent, double width, double height, bool fDistanceMap, bool fBillboard) {
	RESULT r = R_PASS;

	text *pText = new OGLText(this, std::make_shared<font>(strFontFileName, fDistanceMap), strContent, width, height, fBillboard);
	CN(pText);

	CR(pText->SetText(strContent));

	int fbWidth = pText->GetDPM(width);
	int fbHeight = pText->GetDPM(height);

	// TODO: Switch to this with C++17
	//std::clamp(fbWidth, 32, 2048);
	//std::clamp(fbHeight, 32, 2048);

	util::Clamp(fbWidth, 32, 2048);
	util::Clamp(fbHeight, 32, 2048);

	OGLFramebuffer *pOGLFramebuffer = new OGLFramebuffer(this, fbWidth, fbHeight, 4);
	CN(pOGLFramebuffer);

	pText->SetFramebuffer(pOGLFramebuffer);

	CR(pOGLFramebuffer->OGLInitialize());
	CR(pOGLFramebuffer->Bind());

	CR(pOGLFramebuffer->MakeColorAttachment());
	CR(pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
	CR(pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	CR(CheckFramebufferStatus(GL_FRAMEBUFFER));

//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

text* OGLImp::MakeText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags) {
	RESULT r = R_PASS;

	text *pText = new OGLText(this, pFont, textFlags);
	CN(pText);

	//CR(pText->SetText(strContent));
	CR(pText->CreateLayout(pLayout, margin));

	int fbWidth = pText->GetDPM(pText->GetWidth());
	int fbHeight = pText->GetDPM(pText->GetHeight());

	// TODO: Switch to this with C++17
	//std::clamp(fbWidth, 32, 2048);
	//std::clamp(fbHeight, 32, 2048);

	util::Clamp(fbWidth, 32, 2048);
	util::Clamp(fbHeight, 32, 2048);

	OGLFramebuffer *pOGLFramebuffer = new OGLFramebuffer(this, fbWidth, fbHeight, 4);
	CN(pOGLFramebuffer);

	pText->SetFramebuffer(pOGLFramebuffer);

	CR(pOGLFramebuffer->OGLInitialize());
	CR(pOGLFramebuffer->Bind());

	CR(pOGLFramebuffer->MakeColorAttachment());
	CR(pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
	CR(pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	CR(CheckFramebufferStatus(GL_FRAMEBUFFER));

	CR(pText->SetDiffuseTexture(pFont->GetTexture().get()));

	//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

text* OGLImp::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM, text::flags textFlags) {
	RESULT r = R_PASS;

	text *pText = new OGLText(this, pFont, strContent, lineHeightM, textFlags);
	CN(pText);

	CR(pText->SetText(strContent));

	int fbWidth = pText->GetDPM(pText->GetWidth());
	int fbHeight = pText->GetDPM(pText->GetHeight());

	// TODO: Switch to this with C++17
	//std::clamp(fbWidth, 32, 2048);
	//std::clamp(fbHeight, 32, 2048);

	util::Clamp(fbWidth, 32, 2048);
	util::Clamp(fbHeight, 32, 2048);

	OGLFramebuffer *pOGLFramebuffer = new OGLFramebuffer(this, fbWidth, fbHeight, 4);
	CN(pOGLFramebuffer);

	pText->SetFramebuffer(pOGLFramebuffer);

	CR(pOGLFramebuffer->OGLInitialize());
	CR(pOGLFramebuffer->Bind());

	CR(pOGLFramebuffer->MakeColorAttachment());
	CR(pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
	CR(pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	CR(CheckFramebufferStatus(GL_FRAMEBUFFER));

	CR(pText->SetDiffuseTexture(pFont->GetTexture().get()));

	//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

text* OGLImp::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width, double height, text::flags textFlags) {
	RESULT r = R_PASS;

	text *pText = new OGLText(this, pFont, strContent, width, height, textFlags);
	CN(pText);

	CR(pText->SetText(strContent));

	int fbWidth = pText->GetDPM(pText->GetWidth());
	int fbHeight = pText->GetDPM(pText->GetHeight());

	// TODO: Switch to this with C++17
	//std::clamp(fbWidth, 32, 2048);
	//std::clamp(fbHeight, 32, 2048);

//*
	util::Clamp(fbWidth, 32, 2048);
	util::Clamp(fbHeight, 32, 2048);

	OGLFramebuffer *pOGLFramebuffer = new OGLFramebuffer(this, fbWidth, fbHeight, 4);
	CN(pOGLFramebuffer);

	pText->SetFramebuffer(pOGLFramebuffer);

	CR(pOGLFramebuffer->OGLInitialize());
	CR(pOGLFramebuffer->Bind());

	CR(pOGLFramebuffer->MakeColorAttachment());
	CR(pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
	CR(pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	CR(CheckFramebufferStatus(GL_FRAMEBUFFER));
	//*/
	
	CR(pText->SetDiffuseTexture(pFont->GetTexture().get()));

	//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

texture* OGLImp::MakeTexture(texture::type type, const wchar_t *pszFilename) {
	RESULT r = R_PASS;

	texture *pTexture = OGLTexture::MakeTextureFromPath(this, type, std::wstring(pszFilename));
	CN(pTexture);

Success:
	return pTexture;

Error:
	if (pTexture != nullptr) {
		delete pTexture;
		pTexture = nullptr;
	}

	return nullptr;
}

texture* OGLImp::MakeTexture(const texture &srcTexture) {
	RESULT r = R_PASS;

	texture *pTexture = OGLTexture::MakeTexture(srcTexture);
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

texture* OGLImp::MakeTexture(texture::type type, int width, int height, PIXEL_FORMAT pixelFormat, int channels, void *pBuffer, int pBuffer_n) {
	RESULT r = R_PASS;

	texture *pTexture = OGLTexture::MakeTextureFromBuffer(this, type, width, height, channels, pixelFormat, pBuffer, pBuffer_n);
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

texture* OGLImp::MakeTextureFromFileBuffer(texture::type type, uint8_t *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	texture *pTexture = OGLTexture::MakeTextureFromFileBuffer(this, type, pBuffer, pBuffer_n);
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

cubemap* OGLImp::MakeCubemap(const std::wstring &wstrCubemapName) {
	RESULT r = R_PASS;

	cubemap *pCubemap = OGLCubemap::MakeCubemapFromName(this, wstrCubemapName);
	CN(pCubemap);

Success:
	return pCubemap;

Error:
	if (pCubemap != nullptr) {
		delete pCubemap;
		pCubemap = nullptr;
	}

	return nullptr;
}

skybox *OGLImp::MakeSkybox() {
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

SinkNode* OGLImp::MakeSinkNode(std::string strNodeName) {
	SinkNode* pSinkNode = nullptr;
		
	if (strNodeName == "display") {
		pSinkNode = DNode::MakeNode<OGLViewportDisplay>(this);
	}
	else if (strNodeName == "displaycamera") {
		pSinkNode = DNode::MakeNode<OGLCameraViewportDisplay>(this);
	}

	return pSinkNode;
}

SourceNode* OGLImp::MakeSourceNode(std::string strNodeName) {
	SourceNode* pSourceNode = nullptr;

	// TODO: ? will there be HAL backed Source Nodes?

	return pSourceNode;
}

ProgramNode* OGLImp::MakeProgramNode(std::string strNodeName, PIPELINE_FLAGS optFlags) {
	ProgramNode* pProgramNode = nullptr;

	pProgramNode = OGLProgramFactory::MakeOGLProgram(OGLProgramFactory::OGLProgramTypeFromstring(strNodeName), this, m_versionGLSL, optFlags);

	return pProgramNode;
}

RESULT OGLImp::ClearHALBuffers() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return R_PASS;
}

RESULT OGLImp::ConfigureHAL() {

	if (m_HALConfiguration.fDrawWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (m_pHMD == nullptr) {
		CheckFramebufferStatus(GL_FRAMEBUFFER);
	}

	return R_PASS;
}

RESULT OGLImp::FlushHALBuffers() {
	glFlush();
	return R_PASS;
}

// This is critical path, so EHM is removed
// Debug manually
/*
RESULT OGLImp::Render(ObjectStore *pSceneGraph, EYE_TYPE eye) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = pSceneGraph->GetSceneGraphStore();
	VirtualObj *pVirtualObj = nullptr;

	static EYE_TYPE lastEye = EYE_INVALID;

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_HALConfiguration.fDrawWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (m_pHMD == nullptr) {
		CheckFramebufferStatus(GL_FRAMEBUFFER);
	}

	// Render Shadows
	/*
	m_pOGLProgramShadowDepth->UseProgram();
	m_pOGLProgramShadowDepth->BindToDepthBuffer();
	m_pOGLProgramShadowDepth->SetCamera(m_pCamera);
	m_pOGLProgramShadowDepth->SetLights(pLights);
	m_pOGLProgramShadowDepth->RenderObjectStore(pSceneGraph);
	m_pOGLProgramShadowDepth->UnbindFramebuffer();
	//

	// 
	m_pOGLRenderProgram->UseProgram();
	m_pOGLRenderProgram->SetLights(pLights);

	// Camera Projection Matrix
	if (m_pHMD != nullptr) {
		m_pCamera->ResizeCamera(m_pHMD->GetEyeWidth(), m_pHMD->GetEyeHeight());
		m_pOGLRenderProgram->UseProgram();
	}

	m_pOGLRenderProgram->SetStereoCamera(m_pCamera, eye);
	
	if (m_pHMD != nullptr) {
		m_pHMD->SetAndClearRenderSurface(eye);
	}
	else if(eye != lastEye) {
		SetViewTarget(eye);
		lastEye = eye;
	}


	// 3D Object / skybox
	//m_pOGLRenderProgram->UseProgram();
	m_pOGLRenderProgram->RenderObjectStore(pSceneGraph);
	RenderReferenceGeometry(pSceneGraph, eye);

	// Skybox
	RenderSkybox(pObjectStore, eye);

	// Profiler
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	RenderProfiler(eye);

	// Commit frame to HMD
	if (m_pHMD) {
		m_pHMD->UnsetRenderSurface(eye);
		m_pHMD->CommitSwapChain(eye);
	}

	glFlush();

//Error:
	// CheckGLError();
	return r;
}
*/

RESULT OGLImp::Shutdown() {
	RESULT r = R_PASS;

	if (m_pOpenGLRenderingContext != NULL) {
		delete m_pOpenGLRenderingContext;
		m_pOpenGLRenderingContext = NULL;
	}

	return r;
}

// Open GL / Wrappers
// TODO: Remove in turn of extensions or something else, right now hitting two context switches and it's non-optimal

// OpenGL Program

RESULT OGLImp::CreateProgram(GLuint *pOGLProgramIndex) {
	RESULT r = R_PASS;

	*pOGLProgramIndex = m_OpenGLExtensions.glCreateProgram();
	CRM(CheckGLError(), "glCreateProgram failed");

Error:
	return r;
}

RESULT OGLImp::DeleteProgram(GLuint OGLProgramIndex) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDeleteProgram(OGLProgramIndex);
	CRM(CheckGLError(), "glDeleteProgram failed");

Error:
	return r;
}

RESULT OGLImp::UseProgram(GLuint OGLProgramIndex) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glUseProgram(OGLProgramIndex);
	CRM(CheckGLError(), "UseProgram failed");

Error:
	return r;
}

RESULT OGLImp::LinkProgram(GLuint OGLProgramIndex) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glLinkProgram(OGLProgramIndex);
	CRM(CheckGLError(), "glLinkProgram failed");

Error:
	return r;
}

RESULT OGLImp::glGetProgramInfoLog(GLuint programID, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramInfoLog(programID, bufSize, length, infoLog);
	CRM(CheckGLError(), "glGetProgramInfoLog failed");

Error:
	return r;
}

RESULT OGLImp::IsProgram(GLuint m_OGLProgramIndex) {
	RESULT r = R_PASS;

	CB((m_OpenGLExtensions.glIsProgram(m_OGLProgramIndex)));
	CRM(CheckGLError(), "glCreateProgram failed");

Error:
	return r;
}

RESULT OGLImp::glGetProgramiv(GLuint programID, GLenum pname, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramiv(programID, pname, params);
	CRM(CheckGLError(), "glGetProgramiv failed");

Error:
	return r;
}

RESULT OGLImp::glGetProgramInterfaceiv(GLuint program, GLenum programInterface, GLenum pname, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramInterfaceiv(program, programInterface, pname, params);  
	CRM(CheckGLError(), "glGetProgramInterfaceiv failed");

Error:
	return r;
}

RESULT OGLImp::glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDrawRangeElements(mode, start, end, count, type, indices);
	CRM(CheckGLError(), "glDrawRangeElements failed");

Error:
	return r;
}

RESULT OGLImp::glGetProgramResourceiv(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramResourceiv(program, programInterface, index, propCount, props, bufSize, length, params);
	CRM(CheckGLError(), "glGetProgramResourceiv failed");

Error:
	return r;
}

RESULT OGLImp::glGetProgramResourceName(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramResourceName(program, programInterface, index, bufSize, length, name);
	CRM(CheckGLError(), "glGetProgramResourceName failed");

Error:
	return r;
}

RESULT OGLImp::glGenVertexArrays(GLsizei n, GLuint *arrays) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenVertexArrays(n, arrays);  //create VAO container and get ID for it
	CRM(CheckGLError(), "glGenVertexArrays failed");

Error:
	return r;
}

// Bind Array to OpenGL context
RESULT OGLImp::glBindVertexArray(GLuint gluiArray) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindVertexArray(gluiArray);
	CRM(CheckGLError(), "glBindVertexArray failed");

Error:
	return r;
}

// FBO
RESULT OGLImp::glGenFramebuffers(GLsizei n, GLuint *framebuffers) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenFramebuffers(n, framebuffers);
	CRM(CheckGLError(), "glGenFramebuffers failed");

Error:
	return r;
}

RESULT OGLImp::glBindFramebuffer(GLenum target, GLuint gluiFramebuffer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindFramebuffer(target, gluiFramebuffer);
	CRM(CheckGLError(), "glBindFramebuffer failed");

Error:
	return r;
}

RESULT OGLImp::glDeleteFramebuffers(GLsizei n, const GLuint *gluiFramebuffer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDeleteFramebuffers(n, gluiFramebuffer);
	CRM(CheckGLError(), "glBindFramebuffer failed");

Error:
	return r;
}

// PBO
void *OGLImp::glMapBuffer(GLenum target, GLenum access) {
	return m_OpenGLExtensions.glMapBuffer(target, access);
}

RESULT OGLImp::glUnmapBuffer(GLenum target) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glUnmapBuffer(target);
	CRM(CheckGLError(), "glUnmapBuffer failed");

Error:
	return r;
}

// Render Buffers
RESULT OGLImp::glGenRenderbuffers(GLsizei n, GLuint *renderbuffers) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenRenderbuffers(n, renderbuffers);
	CRM(CheckGLError(), "glGenRenderbuffers failed");

Error:
	return r;
}

RESULT OGLImp::glDeleteRenderbuffers(GLsizei n, GLuint *renderbuffers) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDeleteRenderbuffers(n, renderbuffers);
	CRM(CheckGLError(), "glDeleteRenderbuffers failed");

Error:
	return r;
}

RESULT OGLImp::glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindRenderbuffer(target, renderbuffer);
	CRM(CheckGLError(), "glBindRenderbuffer failed");

Error:
	return r;
}

RESULT OGLImp::glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glRenderbufferStorage(target, internalformat, width, height);
	CRM(CheckGLError(), "glRenderbufferStorage failed");

Error:
	return r;
}

RESULT OGLImp::glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
	CRM(CheckGLError(), "glRenderbufferStorageMultisample failed");

Error:
	return r;
}

RESULT OGLImp::glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
	CRM(CheckGLError(), "glFramebufferRenderbuffer failed");

Error:
	return r;
}

RESULT OGLImp::CheckFramebufferStatus(GLenum target) {
	RESULT r = R_PASS;

	GLenum glenumCheckFramebufferStatus = m_OpenGLExtensions.glCheckFramebufferStatus(target);
	CBM((glenumCheckFramebufferStatus == GL_FRAMEBUFFER_COMPLETE), "glCheckframebufferStatus failed with 0x%x", glenumCheckFramebufferStatus);
	CRM(CheckGLError(), "glFramebufferRenderbuffer failed");

Error:
	return r;
}

RESULT OGLImp::glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glFramebufferTexture2D(target, attachment, textarget, texture, level);
	CRM(CheckGLError(), "glFramebufferTexture2D failed");

Error:
	return r;
}

RESULT OGLImp::glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
	CRM(CheckGLError(), "glBlitFramebuffer failed");

Error:
	return r;
}

RESULT OGLImp::glDrawBuffers(GLsizei n, const GLenum *bufs) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDrawBuffers(n, bufs);
	CRM(CheckGLError(), "glFramebufferRenderbuffer failed");

Error:
	return r;
}

RESULT OGLImp::glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glFramebufferTexture(target, attachment, texture, level);
	CRM(CheckGLError(), "glFramebufferTexture failed");

Error:
	return r;
}

// VBO
RESULT OGLImp::glGenBuffers(GLsizei n, GLuint *buffers) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenBuffers(n, buffers);
	CRM(CheckGLError(), "glGenBuffers failed");

Error:
	return r;
}

RESULT OGLImp::glBindBuffer(GLenum target, GLuint gluiBuffer) {
	//RESULT r = R_PASS;

	m_OpenGLExtensions.glBindBuffer(target, gluiBuffer);
	//CRM(CheckGLError(), "glBindBuffer failed");

	return R_PASS;

//Error:
//	return r;
}

RESULT OGLImp::glDeleteBuffers(GLsizei n, const GLuint *buffers) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDeleteBuffers(n, buffers);
	CRM(CheckGLError(), "glDeleteBuffers failed");

Error:
	return r;
}

RESULT OGLImp::glDeleteVertexArrays(GLsizei n, const GLuint *arrays) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDeleteVertexArrays(n, arrays);
	CRM(CheckGLError(), "glDeleteVertexArrays failed");

Error:
	return r;
}

RESULT OGLImp::glBindAttribLocation(GLuint program, GLuint index, const GLchar *name) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindAttribLocation(program, index, name);
	CRM(CheckGLError(), "glBindAttribLocation failed");

Error:
	return r;
}

RESULT OGLImp::glGetAttribLocation(GLuint programID, const GLchar *pszName, GLint *pLocation) {
	RESULT r = R_PASS;

	*pLocation = m_OpenGLExtensions.glGetAttribLocation(programID, pszName);
	CRM(CheckGLError(), "glGetAttribLocation failed");

	return r;
Error:
	*pLocation = -1;
	return r;
}

// Blending 

RESULT OGLImp::glBlendEquation(GLenum mode) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBlendEquation(mode);
	CRM(CheckGLError(), "glBlendEquation failed");

Error:
	return r;
}

RESULT OGLImp::glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
	CRM(CheckGLError(), "glBlendFuncSeparate failed");

Error:
	return r;
}

RESULT OGLImp::glGetUniformBlockIndex(GLuint programID, const GLchar *pszName, GLint *pLocation) {
	RESULT r = R_PASS;

	*pLocation = m_OpenGLExtensions.glGetUniformBlockIndex(programID, pszName);
	CRM(CheckGLError(), "glGetUniformLocation failed");

	return r;
Error:
	*pLocation = -1;
	return r;
}

RESULT OGLImp::glUniformBlockBinding(GLuint programID, GLint uniformBlockIndex, GLint uniformBlockBindingPoint) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glUniformBlockBinding(programID, uniformBlockIndex, uniformBlockBindingPoint);
	CRM(CheckGLError(), "glGetUniformLocation failed");

Error:
	return r;
}

RESULT OGLImp::glBindBufferBase(GLenum target, GLuint bindingPointIndex, GLuint bufferIndex) {
	//RESULT r = R_PASS;

	m_OpenGLExtensions.glBindBufferBase(target, bindingPointIndex, bufferIndex);

	return R_PASS;

//	CRM(CheckGLError(), "glBindBufferBase failed");
//
//Error:
//	return r;
}

RESULT OGLImp::glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices);
	CRM(CheckGLError(), "glGetUniformIndices failed");

Error:
	return r;
}

RESULT OGLImp::glGetUniformLocation(GLuint program, const GLchar *name, GLint *pLocation) {
	//RESULT r = R_PASS;

	*pLocation = m_OpenGLExtensions.glGetUniformLocation(program, name);
	//CRM(CheckGLError(), "glGetUniformLocation failed");

	return R_PASS;

//	return r;
//Error:
//	*pLocation = -1;
//	return r;
}

RESULT OGLImp::glUniform1i(GLint location, GLint v0) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glUniform1i(location, v0);

	CRM(CheckGLError(), "glUniform1i failed");

Error:
	return r;
}

RESULT OGLImp::glUniform1fv(GLint location, GLsizei count, const GLfloat *value) {
	//RESULT r = R_PASS;

	m_OpenGLExtensions.glUniform1fv(location, count, value);

	return R_PASS;

//	CRM(CheckGLError(), "glUniform1fv failed");
//
//Error:
//	return r;
}

RESULT OGLImp::glUniform4fv(GLint location, GLsizei count, const GLfloat *value) {
	//RESULT r = R_PASS;

	m_OpenGLExtensions.glUniform4fv(location, count, value);

	return R_PASS;

//	CRM(CheckGLError(), "glUniform4fv failed");
//
//Error:
//	return r;
}

RESULT OGLImp::glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	//RESULT r = R_PASS;

	m_OpenGLExtensions.glUniformMatrix4fv(location, count, transpose, value);

	return R_PASS;

//	CRM(CheckGLError(), "glUniformMatrix4fv failed");
//
//Error:
//	return r;
}

// OpenGL Shaders
RESULT OGLImp::CreateShader(GLenum type, GLuint *shaderID) {
	RESULT r = R_PASS;

	*shaderID = m_OpenGLExtensions.glCreateShader(type);
	CRM(CheckGLError(), "glCreateShader failed");

	return r;
Error:
	*shaderID = NULL;
	return r;
}

RESULT OGLImp::CreateShaderObject(GLenum type, GLuint *shaderID) {
	RESULT r = R_PASS;

	*shaderID = m_OpenGLExtensions.glCreateShaderObject(type);
	CRM(CheckGLError(), "glCreateShaderObject failed");

	return r;
Error:
	*shaderID = NULL;
	return r;
}

RESULT OGLImp::GetShaderiv(GLuint programID, GLenum pname, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetShaderiv(programID, pname, params);
	CRM(CheckGLError(), "glGetShaderiv failed");

Error:
	return r;
}

RESULT OGLImp::GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetShaderInfoLog(shader, bufSize, length, infoLog);
	CRM(CheckGLError(), "glGetShaderInfoLog failed");

Error:
	return r;
}

RESULT OGLImp::glAttachShader(GLuint program, GLuint shader) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glAttachShader(program, shader);
	CRM(CheckGLError(), "glAttachShader failed");

Error:
	return r;
}

RESULT OGLImp::ShaderSource(GLuint shaderID, GLsizei count, const GLchar *const*string, const GLint *length) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glShaderSource(shaderID, count, string, length);
	CRM(CheckGLError(), "glShaderSource failed");

Error:
	return r;
}

RESULT OGLImp::CompileShader(GLuint shaderID) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glCompileShader(shaderID);
	CRM(CheckGLError(), "glCompileShader failed");

Error:
	return r;
}

RESULT OGLImp::glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
	//RESULT r = R_PASS;

	m_OpenGLExtensions.glBufferData(target, size, data, usage);
	//CRM(CheckGLError(), "glBufferData failed");

	return R_PASS;

//Error:
//	return r;
}

RESULT OGLImp::glBufferSubData(GLenum target, GLsizeiptr offset, GLsizeiptr size, const void *data) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBufferSubData(target, offset, size, data);
	CRM(CheckGLError(), "glBufferSubData failed");

Error:
	return r;
}

RESULT OGLImp::glEnableVertexAtrribArray(GLuint index) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glEnableVertexAttribArray(index);
	CRM(CheckGLError(), "glEnableVertexAttribArray failed");

Error:
	return r;
}

RESULT OGLImp::glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	CRM(CheckGLError(), "glVertexAttribPointer failed");

Error:
	return r;
}

// Textures
RESULT OGLImp::GenerateTextures(GLsizei n, GLuint *pTextures) {
	RESULT r = R_PASS;

	glGenTextures(n, pTextures);
	CRM(CheckGLError(), "glGenTextures failed");

Error:
	return r;
}

RESULT OGLImp::DeleteTextures(GLsizei n, GLuint *pTextures) {
	RESULT r = R_PASS;

	glDeleteTextures(n, pTextures);
	CRM(CheckGLError(), "glGenTextures failed");

Error:
	return r;
}

RESULT OGLImp::glActiveTexture(GLenum texture) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glActiveTexture(texture);
	CRM(CheckGLError(), "glActiveTexture failed");

Error:
	return r;
}

RESULT OGLImp::glBindTextures(GLuint first, GLsizei count, const GLuint *textures) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindTextures(first, count, textures);
	CRM(CheckGLError(), "glBindTextures failed");

Error:
	return r;
}

RESULT OGLImp::BindTexture(GLenum target, GLuint texture) {
	RESULT r = R_PASS;

	glBindTexture(target, texture);
	//CRM(CheckGLError(), "glBindTexture failed");

Error:
	return r;
}

RESULT OGLImp::glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glTextStorage2D(target, levels, internalformat, width, height);
	CRM(CheckGLError(), "glTexStorage2D failed");

Error:
	return r;
}

RESULT OGLImp::glTexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glTexImage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations);
	CRM(CheckGLError(), "glTexImage2DMultisample failed");

Error:
	return r;
}

RESULT OGLImp::TexParameteri(GLenum target, GLenum pname, GLint param) {
	RESULT r = R_PASS;

	//m_OpenGLExtensions.glTexParamteri(target, pname, param);
	glTexParameteri(target, pname, param);
	CRM(CheckGLError(), "glTexParameteri failed");

Error:
	return r;
}

RESULT OGLImp::TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) {
	RESULT r = R_PASS;

	// fix alightment for odd value width size
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	//m_OpenGLExtensions.glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	CRM(CheckGLError(), "glTexImage2D failed");

Error:
	return r;
}

RESULT OGLImp::TextureSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels) {
	RESULT r = R_PASS;

	// fix alightment for odd value width size
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
	CRM(CheckGLError(), "glTexSubImage2D failed");

Error:
	return r;
}

RESULT OGLImp::glGenerateMipmap(GLenum target) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenerateMipmap(target);
	CRM(CheckGLError(), "glGenerateMipmap failed");

Error:
	return r;
}

RESULT OGLImp::GetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels) {
	RESULT r = R_PASS;

	glGetTexImage(target, level, format, type, pixels);
	CRM(CheckGLError(), "glGetTexImage failed");

Error:
	return r;
}

RESULT OGLImp::GetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, GLvoid *pixels) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetTextureImage(texture, level, format, type, bufSize, pixels);
	CRM(CheckGLError(), "glGetTextureImage failed");

Error:
	return r;
}

RESULT OGLImp::GetnTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetnTexImage(target, level, format, type, bufSize, pixels);
	CRM(CheckGLError(), "glGetnTexImage failed");

Error:
	return r;
}

// Queries
RESULT OGLImp::glGenQueries(GLsizei n, GLuint *ids) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenQueries(n, ids);
	CRM(CheckGLError(), "glGenQueries failed");

Error:
	return r;
}

RESULT OGLImp::glDeleteQueries(GLsizei n, const GLuint *ids) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDeleteQueries(n, ids);
	CRM(CheckGLError(), "glDeleteQueries failed");

Error:
	return r;
}

bool OGLImp::glIsQuery(GLuint id) {
	return (m_OpenGLExtensions.glIsQuery(id) != 0);
}

RESULT OGLImp::glBeginQuery(GLenum target, GLuint id) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBeginQuery(target, id);
	CRM(CheckGLError(), "glBeginQuery failed");

Error:
	return r;
}

RESULT OGLImp::glEndQuery(GLenum target) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glEndQuery(target);
	CRM(CheckGLError(), "glEndQuery failed");

Error:
	return r;
}

RESULT OGLImp::glGetQueryiv(GLenum target, GLenum pname, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetQueryiv(target, pname, params);
	CRM(CheckGLError(), "glGetQueryiv failed");

Error:
	return r;
}

RESULT OGLImp::glGetQueryObjectiv(GLuint id, GLenum pname, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetQueryObjectiv(id, pname, params);
	CRM(CheckGLError(), "glGetQueryObjectiv failed");

Error:
	return r;
}

RESULT OGLImp::glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetQueryObjectuiv(id, pname, params);
	CRM(CheckGLError(), "glGetQueryObjectuiv failed");

Error:
	return r;
}


RESULT OGLImp::wglSwapIntervalEXT(int interval) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.wglSwapIntervalEXT(interval);
	CRM(CheckGLError(), "wglSwapIntervalEXT failed");

Error:
	return r;
}


