#ifndef DIM_OBJ_H_
#define DIM_OBJ_H_

// DREAM OS
// DreamOS/Dimension/Primitives/DimObj.h
// Dimension Base Object
// All objects in Dimension should derive from this base class

#include "valid.h"
#include "Primitives/Types/UID.h"

#include "VirtualObj.h"
#include "point.h"
#include "TriangleIndexGroup.h"
#include "Vertex.h"
#include "material.h"

#include "texture.h"

class DimObj : public VirtualObj {
protected:
    //point m_ptOrigin;   // origin > now in virtual object
    //AABV m_aabv;        // Axis Aligned Bounding Volume

protected:
	vertex *m_pVertices;
	dimindex *m_pIndices;
	material m_material;

	// TODO: Multiple textures (one for now)
	texture *m_pColorTexture;
	texture *m_pBumpTexture;

public:
    DimObj() :
        VirtualObj(),	// velocity, origin
		m_pVertices(nullptr),
		m_pIndices(nullptr),
		m_material(),
		m_pColorTexture(nullptr),
		m_pBumpTexture(nullptr)
        //m_aabv()
    {
        /* stub */
    }

    ~DimObj() {
		if (m_pIndices != NULL) {
			delete[] m_pIndices;
			m_pIndices = NULL;
		}

		if (m_pVertices != NULL) {
			delete[] m_pVertices;
			m_pVertices = NULL;
		}
    }

	virtual OBJECT_TYPE GetType() {
		return OBJECT_DIMENSION;
	}

	virtual RESULT Allocate() = 0;

	virtual inline int NumberVertices() = 0;
	inline vertex *VertexData() {
		return m_pVertices;
	}

	inline int VertexDataSize() {
		return NumberVertices() * sizeof(vertex);
	}

	virtual inline int NumberIndices() = 0;
	inline dimindex *IndexData() {
		return m_pIndices;
	}

	inline int IndexDataSize() {
		return NumberIndices() * sizeof(dimindex);
	}

	RESULT AllocateVertices(uint32_t numVerts) {
		RESULT r = R_PASS;

		m_pVertices = new vertex[numVerts];
		CN(m_pVertices);

	Error:
		return r;
	}

	RESULT AllocateIndices(uint32_t numIndices) {
		RESULT r = R_PASS;

		m_pIndices = new dimindex[numIndices];
		CN(m_pIndices);

	Error:
		return r;
	}

	RESULT AllocateTriangleIndexGroups(uint32_t numTriangles) {
		RESULT r = R_PASS;

		m_pIndices = (dimindex*)(new TriangleIndexGroup[numTriangles]);
		CN(m_pIndices);

	Error:
		return r;
	}

	RESULT SetColor(color c) {
		for (int i = 0; i < NumberVertices(); i++)
			m_pVertices[i].SetColor(c);

		return R_PASS;
	}

	RESULT SetColorTexture(texture *pTexture) {
		RESULT r = R_PASS;

		CBM((m_pColorTexture == nullptr), "Cannot overwrite color texture");
		m_pColorTexture = pTexture;
		m_pColorTexture->SetTextureType(texture::TEXTURE_TYPE::TEXTURE_COLOR);

	Error:
		return r;
	}

	RESULT ClearColorTexture() {
		RESULT r = R_PASS;

		CB((m_pColorTexture != nullptr));
		m_pColorTexture = nullptr;

	Error:
		return r;
	}

	RESULT SetBumpTexture(texture *pBumpTexture) {
		RESULT r = R_PASS;

		CBM((m_pBumpTexture == nullptr), "Cannot overwrite bump texture");
		m_pBumpTexture = pBumpTexture;
		m_pBumpTexture->SetTextureType(texture::TEXTURE_TYPE::TEXTURE_BUMP);

	Error:
		return r;
	}

	RESULT ClearBumpTexture() {
		RESULT r = R_PASS;

		CB((m_pBumpTexture != nullptr));
		m_pBumpTexture = nullptr;

	Error:
		return r;
	}

	texture *GetColorTexture() {
		return m_pColorTexture;
	}

	texture *GetBumpTexture() {
		return m_pBumpTexture;
	}

	RESULT SetRandomColor() {
		for (int i = 0; i < NumberVertices(); i++)
			m_pVertices[i].SetRandomColor();

		return R_PASS;
	}

	RESULT CopyVertices(vertex pVerts[], int pVerts_n) {
		RESULT r = R_PASS;

		CBM((pVerts_n == NumberVertices()), "Cannot copy %d verts into DimObj with %d verts", pVerts_n, NumberVertices());
		for (int i = 0; i < pVerts_n; i++)
			m_pVertices[i].SetVertex(pVerts[i]);

	Error:
		return r;
	}

	material *GetMaterial() {
		return (&m_material);
	}
};

#endif // !DIM_OBJ_H_
