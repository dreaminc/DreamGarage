#include "OGLObj.h"
#include "Primitives/BoundingVolume.h"
#include "Primitives/BoundingSphere.h"
#include "OGLVolume.h"
#include "OGLSphere.h"

OGLObj::OGLObj(OpenGLImp *pParentImp) :
	m_pParentImp(pParentImp),
	m_pOGLBoundingVolume(nullptr),
	m_fnOGLProgramPreCallback(nullptr),
	m_fnOGLProgramPostCallback(nullptr)
{
	/* empty stub */
}

OGLObj::~OGLObj() {
	ReleaseOGLBuffers();
}

RESULT OGLObj::ReleaseOGLBuffers() {
	RESULT r = R_PASS;

	if (m_pParentImp != nullptr) {
		if (m_hVBO != NULL) {
			CR(m_pParentImp->glDeleteBuffers(1, &m_hVBO));
			m_hVBO = NULL;
		}

		if (m_hIBO != NULL) {
			CR(m_pParentImp->glDeleteBuffers(1, &m_hIBO));
			m_hIBO = NULL;
		}

		if (m_hVAO != NULL) {
			CR(m_pParentImp->glDeleteVertexArrays(1, &m_hVAO));
			m_hVAO = NULL;
		}
	}

Error:
	return r;
}

// This should be used in the OGLInitialize function
inline GLushort OGLObj::GetOGLPrecision() {
#ifdef FLOAT_PRECISION
	return GL_FLOAT;
#elif defined(DOUBLE_PRECISION)
	return GL_DOUBLE;
#endif
}

// This needs to be called from the sub-class constructor
// or externally from the object (TODO: factory class needed)
RESULT OGLObj::OGLInitialize() {
	RESULT r = R_PASS;

	DimObj *pDimObj = GetDimObj();
	CR(m_pParentImp->MakeCurrentContext());

	// Set up the Vertex Array Object (VAO)
	
	// TODO: Do we need to do this only one time?
	CR(m_pParentImp->glGenVertexArrays(1, &m_hVAO));
	CR(m_pParentImp->glBindVertexArray(m_hVAO));

	// Create Buffer Objects
	//CR(m_pParentImp->glGenBuffers(NUM_VBO, &m_hVBOs[0]));
	CR(m_pParentImp->glGenBuffers(1, &m_hVBO));
	CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));

	// TODO: Remove convenience vars 
	vertex *pVertex = pDimObj->VertexData();
	GLsizeiptr pVertex_n = pDimObj->VertexDataSize();
	CR(m_pParentImp->glBufferData(GL_ARRAY_BUFFER, pVertex_n, &pVertex[0], GL_STATIC_DRAW));

	// Index Element Buffer
	CR(m_pParentImp->glGenBuffers(1, &m_hIBO));
	CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

	// TODO: Remove convenience vars 
	dimindex *pIndex = pDimObj->IndexData();
	int pIndex_s = pDimObj->IndexDataSize();
	CR(m_pParentImp->glBufferData(GL_ELEMENT_ARRAY_BUFFER, pIndex_s, pIndex, GL_STATIC_DRAW));

	// Enable the vertex attribute arrays
	// TODO: This needs to come out of the Implementation shader compilation, should not be static
	// TODO: Might want to encapsulate this in a VertexDecleration / VertexAttributeDeclaration or simply
	// have it fall out of the actual shader

	// Bind Position
	CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
	//CR(m_pParentImp->EnableVertexPositionAttribute());		// TODO: Investigate performance impact of this
	WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)0));
	CR(m_pParentImp->glVertexAttribPointer((GLuint)0, vertex::GetPointDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetVertexOffset()));

	// Color
	//WCR(m_pParentImp->EnableVertexColorAttribute());		// TEMP: Position
	WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)1));
	WCR(m_pParentImp->glVertexAttribPointer((GLuint)1, vertex::GetColorDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetColorOffset()));

	// Normal
	//WCR(m_pParentImp->EnableVertexNormalAttribute());	
	WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)2));
	WCR(m_pParentImp->glVertexAttribPointer((GLuint)2, vertex::GetNormalDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetNormalOffset()));

	// UV Coordinate
	//WCR(m_pParentImp->EnableVertexUVCoordAttribute());
	WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)3));
	WCR(m_pParentImp->glVertexAttribPointer((GLuint)3, vertex::GetUVCoordDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetUVOffset()));

	// Tangent 
	//WCR(m_pParentImp->EnableVertexTangentAttribute());
	WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)4));
	WCR(m_pParentImp->glVertexAttribPointer((GLuint)4, vertex::GetTangentDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetTangentOffset()));

	// Bi-Tangent 
	//WCR(m_pParentImp->EnableVertexBitangentAttribute());
	WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)5));
	WCR(m_pParentImp->glVertexAttribPointer((GLuint)5, vertex::GetBitangentDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetBitangentOffset()));

	//CR(m_pParentImp->ReleaseCurrentContext());

Error:
	return r;
}

RESULT OGLObj::UpdateOGLBuffers() {
	RESULT r = R_PASS;

	DimObj *pDimObj = GetDimObj();
	CNM(pDimObj, "Failed to acquire Dimension Object");

	//CR(m_pParentImp->MakeCurrentContext());

	CR(m_pParentImp->glBindVertexArray(m_hVAO));
	CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));

	vertex *pVertex = pDimObj->VertexData();
	GLsizeiptr pVertex_n = pDimObj->VertexDataSize();
	CR(m_pParentImp->glBufferData(GL_ARRAY_BUFFER, pVertex_n, &pVertex[0], GL_STATIC_DRAW));

	// Usually we prefer to release the context so that it won't get mistakenly used by next OGL calls.
	// We're constantly getting error 1282 from wglMakeContext(null, null).
	// For now, removing this safety.
	// CR(m_pParentImp->ReleaseCurrentContext());

Error:
	return r;
}

// Override this method when necessary by a child object
// Many objects will not need to though. 
//virtual RESULT Render() {
RESULT OGLObj::Render() {
	RESULT r = R_PASS;

	// TODO: Rethink this since it's in the critical path
	DimObj *pDimObj = GetDimObj();

	CR(m_pParentImp->glBindVertexArray(m_hVAO));
	CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
	CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

	GLint previousPolygonMode;
	GLboolean previousCullFaceEnabled;
	glGetIntegerv(GL_POLYGON_MODE, &previousPolygonMode);
	previousCullFaceEnabled = glIsEnabled(GL_CULL_FACE);

	// TODO: This should be made more uniform (functions / caps struct etc)
	if (pDimObj->IsWireframe()) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (previousCullFaceEnabled) {
			glDisable(GL_CULL_FACE);
		}
	}

	glDrawElements(GL_TRIANGLES, pDimObj->NumberIndices(), GL_UNSIGNED_INT, NULL);

	if (pDimObj->IsWireframe()) {
		glPolygonMode(GL_FRONT_AND_BACK, previousPolygonMode);
		if (previousCullFaceEnabled) {
			glEnable(GL_CULL_FACE);
		}
	}

	//glDrawElements(GL_TRIANGLES, pDimObj->NumberIndices(), GL_UNSIGNED_INT, NULL);
	//glDrawElements(GL_LINES, pDimObj->NumberIndices(), GL_UNSIGNED_INT, NULL);
	//glDrawElements(GL_POINT, pDimObj->NumberVertices(), GL_UNSIGNED_INT, NULL);

Error:
	return r;
}

RESULT OGLObj::UpdateBoundingVolume() {
	RESULT r = R_PASS;

	DimObj *pDimObj = GetDimObj();

	// TODO: Better handling of different bounding volumes (or do it in BoundingVolume)
	BoundingBox *pBoundingBox = nullptr;
	if ((pBoundingBox = dynamic_cast<BoundingBox*>(pDimObj->GetBoundingVolume().get())) != nullptr) {
		OGLVolume *pOGLBoundingBox = dynamic_cast<OGLVolume*>(m_pOGLBoundingVolume);

		if (pBoundingBox->CheckAndCleanDirty() && pOGLBoundingBox != nullptr) {
			CR(pOGLBoundingBox->UpdateFromBoundingBox(pBoundingBox));
		}
	}
	else {
		BoundingSphere *pBoundingSphere = nullptr;
		if ((pBoundingSphere = dynamic_cast<BoundingSphere*>(pDimObj->GetBoundingVolume().get())) != nullptr) {
			OGLSphere *pOGLBoundingSphere = dynamic_cast<OGLSphere*>(m_pOGLBoundingVolume);

			if (pBoundingSphere->CheckAndCleanDirty() && pOGLBoundingSphere != nullptr) {
				CR(pOGLBoundingSphere->UpdateFromBoundingSphere(pBoundingSphere));
			}
		}
	}

Error:
	return r;
}

RESULT OGLObj::RenderBoundingVolume() {
	RESULT r = R_PASS;

	if (m_pOGLBoundingVolume == nullptr) {
		DimObj *pDimObj = GetDimObj();
		BoundingVolume* pBoundingVolume = pDimObj->GetBoundingVolume().get();

		if (pBoundingVolume == nullptr) {
			return R_SKIPPED;
		}

		// Create the bounding volume here
		// TODO: this might be better placed in a factory creation pattern for all objects instead
		switch (pBoundingVolume->GetType()) {
			case BoundingVolume::Type::BOX: {
				BoundingBox *pBoundingBox = dynamic_cast<BoundingBox*>(pBoundingVolume);
				m_pOGLBoundingVolume = new OGLVolume(m_pParentImp, pBoundingBox, false);
			} break;

			case BoundingVolume::Type::SPHERE: {
				BoundingSphere *pBoundingSphere = dynamic_cast<BoundingSphere*>(pBoundingVolume);
				m_pOGLBoundingVolume = new OGLSphere(m_pParentImp, pBoundingSphere, false);
			} break;
		}

		CN(m_pOGLBoundingVolume);
		m_pOGLBoundingVolume->GetDimObj()->SetWireframe(true);
	}
	
	CR(m_pOGLBoundingVolume->Render());

Error:
	return r;
}

OGLTexture* OGLObj::GetColorTexture() {
	DimObj *pDimObj = GetDimObj();
	OGLTexture *pTexture = reinterpret_cast<OGLTexture*>(pDimObj->GetColorTexture());
	return pTexture;
}

OGLTexture* OGLObj::GetBumpTexture() {
	DimObj *pDimObj = GetDimObj();
	OGLTexture *pTexture = reinterpret_cast<OGLTexture*>(pDimObj->GetBumpTexture());
	return pTexture;
}

OGLTexture* OGLObj::GetTextureAmbient() {
	DimObj *pDimObj = GetDimObj();
	OGLTexture *pTexture = reinterpret_cast<OGLTexture*>(pDimObj->GetTextureAmbient());
	return pTexture;
}

OGLTexture* OGLObj::GetTextureDiffuse() {
	DimObj *pDimObj = GetDimObj();
	OGLTexture *pTexture = reinterpret_cast<OGLTexture*>(pDimObj->GetTextureDiffuse());
	return pTexture;
}

OGLTexture* OGLObj::GetTextureSpecular() {
	DimObj *pDimObj = GetDimObj();
	OGLTexture *pTexture = reinterpret_cast<OGLTexture*>(pDimObj->GetTextureSpecular());
	return pTexture;
}

OGLObj *OGLObj::GetOGLBoundingVolume() {
	return m_pOGLBoundingVolume;
}

RESULT OGLObj::SetOGLProgramPreCallback(std::function<RESULT(OGLProgram*, void*)> fnOGLProgramPreCallback) {
	m_fnOGLProgramPreCallback = fnOGLProgramPreCallback;
	return R_PASS;
}

std::function<RESULT(OGLProgram*, void*)> OGLObj::GetOGLProgramPreCallback() {
	return m_fnOGLProgramPreCallback;
}

RESULT OGLObj::SetOGLProgramPostCallback(std::function<RESULT(OGLProgram*, void*)> fnOGLProgramPostCallback) {
	m_fnOGLProgramPostCallback = fnOGLProgramPostCallback;
	return R_PASS;
}

std::function<RESULT(OGLProgram*, void*)> OGLObj::GetOGLProgramPostCallback() {
	return m_fnOGLProgramPostCallback;
}