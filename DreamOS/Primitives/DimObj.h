#ifndef DIM_OBJ_H_
#define DIM_OBJ_H_

#include "RESULT/EHM.h"

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
#include "QuadIndexGroup.h"
#include "Vertex.h"

#include "TimeManager/TimeManager.h"
#include "material.h"
#include "texture.h"

#include <vector>
#include <memory>

class BoundingVolume;

class DimObj : public VirtualObj, public Subscriber<TimeEvent>, public dirty {
protected:
    //point m_ptOrigin;   // origin > now in virtual object
    //AABV m_aabv;        // Axis Aligned Bounding Volume

protected:
	vertex *m_pVertices;
	dimindex *m_pIndices;
	material m_material;

	// Bounding Volume
	std::shared_ptr<BoundingVolume> m_pBoundingVolume;

	// Use this flag to signal the appropriate rendering object (such as OGLObj) that it needs to update the buffer
	// TODO: This should be encapsulated as a dirty pattern
	bool m_fDirty;
	
	// TODO: textures need to go into material once we figure out how we put a sampler2D into a uniform block
	// TODO: Multiple textures (one for now)
	texture *m_pColorTexture;
	texture *m_pBumpTexture;
	texture *m_pTextureAmbient = nullptr;
	texture *m_pTextureDiffuse = nullptr;
	texture *m_pTextureSpecular = nullptr;

private:
	bool m_fVisible;
	bool m_fWireframe;

public:

	DimObj();
	~DimObj();

	// TODO: rename
	enum class MaterialTexture { 
		Ambient, 
		Diffuse, 
		Specular 
	};

	virtual OBJECT_TYPE GetType();

	virtual RESULT Allocate() = 0;

	virtual RESULT Destroy();

	virtual inline unsigned int NumberVertices() = 0;
	inline vertex *VertexData() { return m_pVertices; }
	inline int VertexDataSize() { return NumberVertices() * sizeof(vertex); }

	virtual inline unsigned int NumberIndices() = 0;
	inline dimindex *IndexData() { return m_pIndices; }
	inline int IndexDataSize() { return NumberIndices() * sizeof(dimindex); }

	RESULT AllocateVertices(uint32_t numVerts);
	RESULT AllocateIndices(uint32_t numIndices);
	RESULT AllocateTriangleIndexGroups(uint32_t numTriangles);
	RESULT AllocateQuadIndexGroups(uint32_t numQuads);

	virtual RESULT UpdateBuffers();

	bool IsVisible();
	RESULT SetVisible(bool fVisible = true);

	bool IsWireframe();
	RESULT SetWireframe(bool fWireframe = true);

	RESULT SetColor(color c);

	RESULT SetColorTexture(texture *pTexture);
	RESULT ClearColorTexture();
	texture *GetColorTexture();

	RESULT SetBumpTexture(texture *pBumpTexture);
	RESULT ClearBumpTexture();
	texture *GetBumpTexture();
	
	// TODO: Above accessors / create texture store
	RESULT SetMaterialTexture(MaterialTexture type, texture *pTexture);
	
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

	// Bounding Volume
public:
	RESULT UpdateBoundingVolume();
	RESULT InitializeAABB();
	RESULT InitializeOBB();
	RESULT InitializeBoundingSphere();
	std::shared_ptr<BoundingVolume> GetBoundingVolume();

public:
	RESULT SetTriangleNormal(dimindex i1, dimindex i2, dimindex i3);
	RESULT SetTriangleTangentBitangent(dimindex i1, dimindex i2, dimindex i3);
	RESULT SetQuadTangentBitangent(dimindex TL, dimindex TR, dimindex BL, dimindex BR);
	RESULT SetQuadTangentBitangent(dimindex TL, dimindex TR, dimindex BL, dimindex BR, vector tangent, vector bitangent);

	RESULT CopyVertices(vertex pVerts[], int pVerts_n);
	
	// TODO: Should this moved up into vertex?
	RESULT RotateVerticesByEulerVector(vector vEuler);
	
	// TODO: This shouldn't be baked in here ultimately
	RESULT Notify(TimeEvent *event);

	material* GetMaterial();

	matrix<virtual_precision, 4, 4> GetModelMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));
};

#endif // !DIM_OBJ_H_
