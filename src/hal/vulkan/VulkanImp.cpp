#include "VulkanImp.h"

#include <vector>
#include <algorithm>

#include "logger/DreamLogger.h"

#include "easylogging++.h"

#include "core/Utilities.h"

#include "core/matrix/ProjectionMatrix.h"
#include "core/matrix/TranslationMatrix.h"
#include "core/matrix/RotationMatrix.h"

#include "core/text/font.h"

VulkanImp::VulkanImp() :
	m_versionVulkan(0)
{
	RESULT r = R_PASS;

Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

VulkanImp::~VulkanImp() {
	// empty
}

RESULT VulkanImp::CheckVulkanError() {
	RESULT r = R_NOT_IMPLEMENTED_ERROR;

	/*
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
	*/

Error:
	return r;
}

RESULT VulkanImp::MakeCurrentContext() {
	RESULT r = R_NOT_IMPLEMENTED_ERROR;

	// TODO: 

Error:
	return r;
}

RESULT VulkanImp::ReleaseCurrentContext() {
	RESULT r = R_NOT_IMPLEMENTED_ERROR;

	// TODO: 

Error:
	return r;
}

RESULT VulkanImp::InitializeHAL() {
	//RESULT r = R_PASS;
	RESULT r = R_NOT_IMPLEMENTED_ERROR;

	// TODO: 

Error:
	return r;
}

RESULT VulkanImp::Resize(viewport newViewport) {
	//RESULT r = R_PASS;
	RESULT r = R_NOT_IMPLEMENTED_ERROR;

	// TODO: 

Error:
	return r;
}

// Assumes Context Current
// TODO: This should move to a sink node
RESULT VulkanImp::SetViewTarget(EYE_TYPE eye, int pxWidth, int pxHeight) {
	//RESULT r = R_PASS;
	RESULT r = R_NOT_IMPLEMENTED_ERROR;



Error:
	return r;
}

/*
// This allows for separate HAL initialization of objects
// Note, this will clobber the dim object on failure
RESULT VulkanImp::InitializeObject(DimObj *pDimObj) {
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

RESULT VulkanImp::InitializeTexture(texture *pTexture) {
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

DimObj* VulkanImp::MakeObject(PrimParams *pPrimParams, bool fInitialize) {
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

texture* VulkanImp::MakeTexture(PrimParams *pPrimParams, bool fInitialize) {
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

OGLMesh* VulkanImp::MakeMesh(PrimParams *pPrimParams, bool fInitialize) {
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

mesh *VulkanImp::MakeMesh(const std::vector<vertex>& vertices) {
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

mesh *VulkanImp::MakeMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
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

model* VulkanImp::MakeModel() {
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

composite *VulkanImp::MakeComposite() {
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

FlatContext *VulkanImp::MakeFlatContext(int pxFBWidth, int pxFBHeight, int fbChannels) {
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

user *VulkanImp::MakeUser() {
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

billboard *VulkanImp::MakeBillboard(point ptOrigin, float width, float height) {
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
light* VulkanImp::MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
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

OGLQuad* VulkanImp::MakeQuad(PrimParams *pPrimParams, bool fInitialize) {
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

OGLModel* VulkanImp::MakeModel(PrimParams *pPrimParams, bool fInitialize) {
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

quad* VulkanImp::MakeQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) {
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
//quad* VulkanImp::MakeQuad(double width, double height, point ptOrigin, vector vNormal) {
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
quad* VulkanImp::MakeQuad(double width, double height, point ptCenter, uvcoord uvTopLeft, uvcoord uvBottomRight, vector vNormal) {
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

quad* VulkanImp::MakeQuad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType, vector vNormal) {
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

cylinder* VulkanImp::MakeCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) {
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

DimRay* VulkanImp::MakeRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
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

DimPlane* VulkanImp::MakePlane(point ptOrigin, vector vNormal) {
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

OGLSphere* VulkanImp::MakeSphere(PrimParams *pPrimParams, bool fInitialize) {
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

sphere* VulkanImp::MakeSphere(float radius = 1.0f, int numAngularDivisions = 10, int numVerticalDivisions = 10, color c = color(COLOR_WHITE)) {
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
composite* VulkanImp::MakeModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, vector vEulerRotation) {
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

hand* VulkanImp::MakeHand(HAND_TYPE type) {
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

hand* VulkanImp::MakeHand(HAND_TYPE type, long avatarID) {
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

volume* VulkanImp::MakeVolume(double width, double length, double height, bool fTriangleBased) {
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

OGLVolume* VulkanImp::MakeVolume(PrimParams *pPrimParams, bool fInitialize) {
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

volume* VulkanImp::MakeVolume(double side, bool fTriangleBased) {
	return MakeVolume(side, side, side, fTriangleBased);
}

//TODO: the texture could be stored in the font already, but having this pathway
// avoids conflicts with parts of the code that use fonts without setting the texture
text* VulkanImp::MakeText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& strContent, double width, double height, bool fDistanceMap, bool fBillboard) {
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

text* VulkanImp::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width, double height, bool fDistanceMap, bool fBillboard) {
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

text* VulkanImp::MakeText(const std::wstring& strFontFileName, const std::string& strContent, double width, double height, bool fDistanceMap, bool fBillboard) {
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

text* VulkanImp::MakeText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags) {
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

text* VulkanImp::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM, text::flags textFlags) {
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
*/

/*
text* VulkanImp::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width, double height, text::flags textFlags) {
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
	//*//*
	
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
*/

/*
texture* VulkanImp::MakeTexture(texture::type type, const wchar_t *pszFilename) {
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

texture* VulkanImp::MakeTexture(const texture &srcTexture) {
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

texture* VulkanImp::MakeTexture(texture::type type, int width, int height, PIXEL_FORMAT pixelFormat, int channels, void *pBuffer, int pBuffer_n) {
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

texture* VulkanImp::MakeTextureFromFileBuffer(texture::type type, uint8_t *pBuffer, size_t pBuffer_n) {
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

cubemap* VulkanImp::MakeCubemap(const std::wstring &wstrCubemapName) {
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

skybox *VulkanImp::MakeSkybox() {
	//RESULT r = R_PASS;
	RESULT r = R_NOT_IMPLEMENTED_ERROR;

	skybox *pSkybox = nullptr;

	//skybox *pSkybox = new OGLSkybox(this);
	//CN(pSkybox);

Success:
	return pSkybox;

Error:
	if (pSkybox != nullptr) {
		delete pSkybox;
		pSkybox = nullptr;
	}
	return nullptr;
}
*/

SinkNode* VulkanImp::MakeSinkNode(std::string strNodeName) {
	SinkNode* pSinkNode = nullptr;
		
	if (strNodeName == "display") {
		// pSinkNode = DNode::MakeNode<OGLViewportDisplay>(this);
	}
	else if (strNodeName == "displaycamera") {
		// pSinkNode = DNode::MakeNode<OGLCameraViewportDisplay>(this);
	}

	ACBM(false, "MakeSinkNode not implemented for Vulkan HAL");

	return pSinkNode;
}

SourceNode* VulkanImp::MakeSourceNode(std::string strNodeName) {
	SourceNode* pSourceNode = nullptr;

	// TODO: ? will there be HAL backed Source Nodes?

	return pSourceNode;
}

ProgramNode* VulkanImp::MakeProgramNode(std::string strNodeName, PIPELINE_FLAGS optFlags) {
	ProgramNode* pProgramNode = nullptr;

	//pProgramNode = OGLProgramFactory::MakeOGLProgram(OGLProgramFactory::OGLProgramTypeFromstring(strNodeName), this, m_versionGLSL, optFlags);

	// TODO: Need to revisit program arch a bit since don't want to
	// create/support a Vulkan Program factory.  Should be possible to 
	// use programs across HALs

	ACBM(false, "MakeProgramNode not implemented for Vulkan HAL");

	return pProgramNode;
}

RESULT VulkanImp::ClearHALBuffers() {
	RESULT r = R_NOT_IMPLEMENTED_ERROR;

	// TODO: 

Error:
	return r;
}

RESULT VulkanImp::ConfigureHAL() {
	RESULT r = R_NOT_IMPLEMENTED_ERROR;

	// TODO: 

Error:
	return r;
}

RESULT VulkanImp::FlushHALBuffers() {
	RESULT r = R_NOT_IMPLEMENTED_ERROR;

	// TODO: 

Error:
	return r;
}

RESULT VulkanImp::Shutdown() {
	RESULT r = R_NOT_IMPLEMENTED_ERROR;

	// TODO:

Error:
	return r;
}


