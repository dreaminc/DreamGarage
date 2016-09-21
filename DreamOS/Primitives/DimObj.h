#ifndef DIM_OBJ_H_
#define DIM_OBJ_H_

// DREAM OS
// DreamOS/Dimension/Primitives/DimObj.h
// Dimension Base Object
// All objects in Dimension should derive from this base class

#include "RESULT/EHM.h"

#include "valid.h"
#include "dirty.h"
#include "Primitives/Types/UID.h"

#include "VirtualObj.h"
#include "point.h"
#include "TriangleIndexGroup.h"
#include "Vertex.h"

#include "TimeManager/TimeManager.h"
#include "material.h"
#include "texture.h"

#include <vector>
#include <memory>

class DimObj : public VirtualObj, public Subscriber<TimeEvent>, public dirty {
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

	// textures need to go into material once we figure out how we put a sampler2D into a uniform block
	texture *m_pTextureAmbient = nullptr;
	texture *m_pTextureDiffuse = nullptr;
	texture *m_pTextureSpecular = nullptr;

private:
	bool m_fVisible;

public:
	enum class MaterialTexture { Ambient, Diffuse, Specular };

	DimObj();
	~DimObj();

	virtual OBJECT_TYPE GetType();

	virtual RESULT Allocate() = 0;

	virtual RESULT Destroy();

	virtual inline unsigned int NumberVertices() = 0;

	virtual inline unsigned int NumberIndices() = 0;

	inline vertex* VertexData() {
		return m_pVertices;
	}

	inline int VertexDataSize() {
		return NumberVertices() * sizeof(vertex);
	}

	inline dimindex* IndexData() {
		return m_pIndices;
	}

	inline int IndexDataSize() {
		return NumberIndices() * sizeof(dimindex);
	}

	RESULT AllocateVertices(uint32_t numVerts);

	RESULT AllocateIndices(uint32_t numIndices);

	RESULT AllocateTriangleIndexGroups(uint32_t numTriangles);

	virtual RESULT UpdateBuffers();

	bool IsVisible();

	RESULT SetVisible(bool fVisible = true);

	RESULT SetColor(color c);

	RESULT SetColorTexture(texture *pTexture);

	RESULT SetMaterialTexture(MaterialTexture type, texture *pTexture);

	RESULT ClearColorTexture();

	RESULT SetBumpTexture(texture *pBumpTexture);

	RESULT ClearBumpTexture();

	texture* GetColorTexture();

	texture* GetBumpTexture();

	texture* GetTextureAmbient();

	texture* GetTextureDiffuse();

	texture* GetTextureSpecular();

	RESULT SetRandomColor();

	// Children (composite objects)
	RESULT AddChild(std::shared_ptr<DimObj> pDimObj);

	RESULT ClearChildren();

	bool HasChildren();

	std::vector<std::shared_ptr<VirtualObj>> GetChildren();

protected:
	RESULT SetParent(DimObj* pParent);

private:
	DimObj* m_pParent;
	std::unique_ptr<std::vector<std::shared_ptr<VirtualObj>>> m_pObjects;

public:
	// This assumes the other vertices have a valid position and uv mapping
	// This will set the tangents/bi-tangents for all three vertices
	// Source: http://learnopengl.com/#!Advanced-Lighting/Normal-Mapping
	// TODO: Use matrix to simplify logic
	//RESULT SetTangentBitangent(vertex v1, vertex v2, vertex v3) {
	RESULT SetTriangleTangentBitangent(dimindex i1, dimindex i2, dimindex i3);

	// This will not take into consideration surfaces that are continuous 
	// TODO: Create surface based normal calculation function (this works at the vertex level rather the triangle one)
	RESULT SetTriangleNormal(dimindex i1, dimindex i2, dimindex i3);

	RESULT SetQuadTangentBitangent(dimindex TL, dimindex TR, dimindex BL, dimindex BR);

	RESULT SetQuadTangentBitangent(dimindex TL, dimindex TR, dimindex BL, dimindex BR, vector tangent, vector bitangent);

	RESULT CopyVertices(vertex pVerts[], int pVerts_n);

	// TODO: Should this moved up into vertex?
	RESULT RotateVerticesByEulerVector(vector vEuler);
	
	// TODO: This shoudln't be baked in here ultimately
	RESULT Notify(TimeEvent *event);

	material* GetMaterial();

	matrix<virtual_precision, 4, 4> GetModelMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));
};

#endif // !DIM_OBJ_H_
