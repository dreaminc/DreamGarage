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

class DimObj : public VirtualObj, 
			   public Subscriber<TimeEvent>, 
			   public dirty 
{
	friend class OGLObj;

protected:
	vertex *m_pVertices = nullptr;
	dimindex *m_pIndices = nullptr;

	material m_material;

	// Bounding Volume
	std::shared_ptr<BoundingVolume> m_pBoundingVolume;

	// Use this flag to signal the appropriate rendering object (such as OGLObj) that it needs to update the buffer
	// TODO: This should be encapsulated as a dirty pattern
	bool m_fDirty;
	
	// TODO: textures need to go into material once we figure out how we put a sampler2D into a uniform block
	// TODO: Multiple textures (one for now)
	// TODO: This should go into material 
	texture *m_pBumpTexture = nullptr;
	texture *m_pTextureAmbient = nullptr;
	texture *m_pTextureDiffuse = nullptr;
	texture *m_pTextureSpecular = nullptr;
	texture *m_pDisplacementTexture = nullptr;

private:
	bool m_fVisible;
	bool m_fWireframe;

public:

	DimObj();
	virtual ~DimObj();

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

	color GetDiffuseColor();
	color GetSpecularColor();
	color GetAmbientColor();

	RESULT SetVertexColor(color c, bool fSetChildren = false);
	RESULT SetAlpha(color_precision a);

	// TODO: Move this into material 
	RESULT SetDiffuseTexture(texture *pTexture);
	texture* GetTextureDiffuse();

	RESULT SetSpecularTexture(texture *pTexture);
	texture* GetTextureSpecular();

	RESULT SetAmbientTexture(texture *pTexture);
	texture* GetTextureAmbient();

	RESULT SetBumpTexture(texture *pBumpTexture);
	texture *GetBumpTexture();

	RESULT SetDisplacementTexture(texture *pDisplacementTexture);
	texture* GetDisplacementTexture();
	
	// TODO: Above accessors / create texture store
	RESULT SetMaterialColors(color c, bool fSetChildren = false);
	RESULT SetMaterialTexture(MaterialTexture type, texture *pTexture);
	RESULT SetMaterialAmbient(float ambient, bool fSetChildren = false);

	RESULT SetMaterialDiffuseColor(color c, bool fSetChildren = false);
	RESULT SetMaterialSpecularColor(color c, bool fSetChildren = false);
	RESULT SetMaterialAmbientColor(color c, bool fSetChildren = false);
	RESULT SetMaterialShininess(float shine, bool fSetChildren = false);
	RESULT SetMaterialBumpiness(float bumpiness, bool fSetChildren = false);
	RESULT SetMaterialDisplacement(float displacement, bool fSetChildren = false);
	RESULT SetMaterialReflectivity(float reflectivity, bool fSetChildren = false);
	RESULT SetMaterialRefractivity(float refractivity, bool fSetChildren = false);
	RESULT SetMaterialUVTiling(float uTiling, float vTiling, bool fSetChildren = false);
	

	RESULT SetRandomColor();
	
	// linear transformation for uv vector = matA + matB * UV
	// This is a slow operation, use it only for initialization
	// should not be used for transforming UV in real time
	RESULT TransformUV(matrix<uv_precision, 2, 1> matA, matrix<uv_precision, 2, 2> matB);

	// Children (composite objects)
	RESULT AddChild(std::shared_ptr<DimObj> pDimObj, bool fFront = false);
	RESULT RemoveChild(std::shared_ptr<DimObj> pDimObj);
	RESULT RemoveChild(VirtualObj *pObj);
	RESULT RemoveLastChild();
	RESULT ClearChildren();

	std::shared_ptr<DimObj> GetChildObject(int index);

	// Explicit instantiations in source 
	template <class objType> 
	std::shared_ptr<objType> GetFirstChild() {
		for (auto &pChildObject : *m_pObjects) {
			std::shared_ptr<objType> pObj = std::dynamic_pointer_cast<objType>(pChildObject);
			if (pObj != nullptr) {
				return pObj;
			}
		}

		return nullptr;
	}

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
	virtual vector GetScale(bool fAbsolute = false) override;

	// Composites will accumulate mass
	virtual double GetMass() override;
	virtual double GetInverseMass() override;

	// TODO: Intertial momentum 

protected:
	RESULT SetParent(DimObj* pParent);

public:
	DimObj *GetParent();

protected:
	DimObj* m_pParent = nullptr;
	std::unique_ptr<std::vector<std::shared_ptr<VirtualObj>>> m_pObjects;
	//std::vector<std::shared_ptr<VirtualObj>> *m_pObjects = nullptr;

	// Bounding Volume
public:
	virtual RESULT UpdateBoundingVolume();
	virtual RESULT GetMinMaxPoint(point *pPtMax, point *pPtMin);

	RESULT InitializeAABB();
	RESULT InitializeOBB();
	RESULT InitializeBoundingSphere();
	RESULT InitializeBoundingQuad();
	RESULT InitializeBoundingQuad(point ptOrigin, float width, float height, vector vNormal);
	RESULT InitializeBoundingPlane(point ptOrigin, vector vNormal);

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
	RESULT SetMaterial(material mMaterial);

	matrix<virtual_precision, 4, 4> GetRotationMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));
	matrix<virtual_precision, 4, 4> GetTranslationMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));
	matrix<virtual_precision, 4, 4> GetModelMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));
	matrix<virtual_precision, 4, 4> GetFlatModelMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));
	matrix<virtual_precision, 4, 4> GetRelativeModelMatrix();
};

#endif // !DIM_OBJ_H_
