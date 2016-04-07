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

	// This assumes the other vertices have a valid position and uv mapping
	// This will set the tangents/bi-tangents for all three vertices
	// Source: http://learnopengl.com/#!Advanced-Lighting/Normal-Mapping
	// TODO: Use matrix to simplify logic
	//RESULT SetTangentBitangent(vertex v1, vertex v2, vertex v3) {
	RESULT SetTriangleTangentBitangent(dimindex i1, dimindex i2, dimindex i3) {
		RESULT r = R_PASS;
		vector tangent, bitangent;
		vertex *pV1 = nullptr, *pV2 = nullptr, *pV3 = nullptr;
		vector deltaPos1, deltaPos2;
		uvcoord deltaUV1, deltaUV2;
		point_precision factor = 0.0f;

		// TODO: More eloquent way than this
		CB((i1 < NumberIndices()));
		pV1 = &(m_pVertices[i1]);
		CN(pV1);

		CB((i2 < NumberIndices()));
		pV2 = &(m_pVertices[i2]);
		CN(pV2);

		CB((i3 < NumberIndices()));
		pV3 = &(m_pVertices[i3]);
		CN(pV3);

		deltaPos1 = pV2->GetPoint() - pV1->GetPoint();
		deltaPos2 = pV3->GetPoint() - pV1->GetPoint();

		deltaUV1 = pV2->GetUV() - pV1->GetUV();
		deltaUV2 = pV3->GetUV() - pV1->GetUV();

		factor = 1.0f / ( (deltaUV1.u() * deltaUV2.v()) - (deltaUV1.v() * deltaUV2.u()) );

		/*
		tangent = vector();
		tangent.x() = factor * ( (deltaUV2.v() * edge1.x()) - (deltaUV1.v() * edge2.x()) );
		tangent.y() = factor * ( (deltaUV2.v() * edge1.y()) - (deltaUV1.v() * edge2.y()) );
		tangent.z() = factor * ( (deltaUV2.v() * edge1.z()) - (deltaUV1.v() * edge2.z()) );
		//*/
		///*
		tangent = factor * ((deltaPos1 * deltaUV2.v()) - (deltaPos2 * deltaUV1.v()));
		//*/
		tangent.Normalize();

		/*
		bitangent = vector();
		bitangent.x() = factor * ((-deltaUV2.u() * edge1.x()) + (deltaUV1.u() * edge2.x()));
		bitangent.y() = factor * ((-deltaUV2.u() * edge1.y()) + (deltaUV1.u() * edge2.y()));
		bitangent.z() = factor * ((-deltaUV2.u() * edge1.z()) + (deltaUV1.u() * edge2.z()));
		//*/

		///*
		bitangent = factor * ((deltaPos2 * deltaUV1.u()) - (deltaPos1 * deltaUV2.u()));
		//*/
		bitangent.Normalize();

		pV1->SetTangentBitangent(tangent, bitangent);
		pV2->SetTangentBitangent(tangent, bitangent);
		pV3->SetTangentBitangent(tangent, bitangent);

	Error:
		return r;
	}

	RESULT SetQuadTangentBitangent(dimindex TL, dimindex TR, dimindex BL, dimindex BR) {
		RESULT r = R_PASS;
		vertex *pVTR = nullptr, *pVBL = nullptr;

		// TODO: More eloquent way than this
		CB((TR < NumberIndices()));
		pVTR = &(m_pVertices[TR]);
		CN(pVTR);

		CB((BL < NumberIndices()));
		pVBL = &(m_pVertices[BL]);
		CN(pVBL);

		CR(SetTriangleTangentBitangent(TL, BR, BL));

		CR(pVTR->SetTangentBitangent(pVBL->GetTangent(), pVBL->GetBitangent()));

	Error:
		return r;
	}

	RESULT SetQuadTangentBitangent(dimindex TL, dimindex TR, dimindex BL, dimindex BR, vector tangent, vector bitangent) {
		RESULT r = R_PASS;
		vertex *pVTL = nullptr, *pVTR = nullptr, *pVBL = nullptr, *pVBR = nullptr;

		// TODO: More eloquent way than this
		CB((TL < NumberIndices()));
		pVTL = &(m_pVertices[TL]);
		CN(pVTL);
		CR(pVTL->SetTangentBitangent(tangent, bitangent));

		CB((TR < NumberIndices()));
		pVTR = &(m_pVertices[TR]);
		CN(pVTR);
		CR(pVTR->SetTangentBitangent(tangent, bitangent));

		CB((BL < NumberIndices()));
		pVBL = &(m_pVertices[BL]);
		CN(pVBL);
		CR(pVBL->SetTangentBitangent(tangent, bitangent));

		CB((BR < NumberIndices()));
		pVBR = &(m_pVertices[BR]);
		CN(pVBR);
		CR(pVBR->SetTangentBitangent(tangent, bitangent));

	Error:
		return r;
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
