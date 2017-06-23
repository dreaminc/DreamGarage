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
class CollisionManifold;

class DimObj : public VirtualObj, public Subscriber<TimeEvent>, public dirty {
	friend class OGLObj;

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

	color m_cColor;

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

	virtual bool IsVisible() override;
	RESULT SetVisible(bool fVisible = true, bool fSetChildren = true);

	bool IsWireframe();
	RESULT SetWireframe(bool fWireframe = true);

	color GetColor();
	RESULT SetColor(color c);
	RESULT SetAlpha(color_precision a);

	RESULT SetColorTexture(texture *pTexture);
	RESULT UpdateColorTexture(texture *pTexture);
	RESULT ClearColorTexture();
	texture *GetColorTexture();

	RESULT SetBumpTexture(texture *pBumpTexture);
	RESULT ClearBumpTexture();
	texture *GetBumpTexture();
	
	// TODO: Above accessors / create texture store
	RESULT SetMaterialTexture(MaterialTexture type, texture *pTexture);
	RESULT SetMaterialAmbient(float ambient);
	
	texture* GetTextureAmbient();
	texture* GetTextureDiffuse();
	texture* GetTextureSpecular();

	RESULT SetRandomColor();
	
	// linear transformation for uv vector = matA + matB * UV
	// This is a slow operation, use it only for initialization
	// should not be used for transforming UV in real time
	RESULT TransformUV(matrix<uv_precision, 2, 1> matA, matrix<uv_precision, 2, 2> matB);

	// Children (composite objects)
	RESULT AddChild(std::shared_ptr<DimObj> pDimObj);
	RESULT RemoveChild(std::shared_ptr<DimObj> pDimObj);
	RESULT RemoveChild(VirtualObj *pObj);
	RESULT RemoveLastChild();
	RESULT ClearChildren();
	bool HasChildren();
	bool CompareParent(DimObj* pParent);

	std::vector<std::shared_ptr<VirtualObj>> GetChildren();

	// Intersections and Collision
	bool Intersect(VirtualObj* pObj, int depth = 0);
	CollisionManifold Collide(VirtualObj* pObj, int depth = 0);

	bool Intersect(const ray &rCast, int depth = 0);
	CollisionManifold Collide(const ray &rCast, int depth = 0);

	// Composites will have absolute vs. frame of reference position/orientation
	virtual point GetOrigin(bool fAbsolute = false) override;
	virtual point GetPosition(bool fAbsolute = false) override;
	virtual quaternion GetOrientation(bool fAbsolute = false) override;

	// Composites will accumulate mass
	virtual double GetMass() override;
	virtual double GetInverseMass() override;

	// TODO: Intertial momentum 

protected:
	RESULT SetParent(DimObj* pParent);

public:
	DimObj *GetParent();

protected:
	DimObj* m_pParent;
	std::unique_ptr<std::vector<std::shared_ptr<VirtualObj>>> m_pObjects;

	// Bounding Volume
public:
	virtual RESULT UpdateBoundingVolume();
	RESULT InitializeAABB();
	RESULT InitializeOBB();
	RESULT InitializeBoundingSphere();
	RESULT InitializeBoundingQuad(point ptOrigin, float width, float height, vector vNormal);
	std::shared_ptr<BoundingVolume> GetBoundingVolume();

	// OnManipulation is called by VirtualObj every time a manipulation occurs - this is a chance for
	// DimObj to update the bounding volume
	virtual RESULT OnManipulation() override;

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
