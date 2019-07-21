#ifndef MODEL_H_
#define MODEL_H_

#include "core/ehm/EHM.h"

// DREAM OS
// DreamOS/Primitives/model/model.h

//#include <vector>

//#include "Primitives/DimObj.h"
//#include "Primitives/Vertex.h"
//#include "Primitives/point.h"
//#include "Primitives/color.h"

#include "Primitives/composite.h"

#include "Primitives/PrimParams.h"

#include "mesh.h"

class HALImp;

class model : public composite {
public:
	friend class ModelFactory;

public:
	struct params :
		public PrimParams
	{
		virtual PRIMITIVE_TYPE GetPrimitiveType() override { return PRIMITIVE_TYPE::MODEL; }

		params(std::wstring wstrModelFilePath, ModelFactory::flags modelFactoryFlags = ModelFactory::NONE) :
			wstrModelFilePath(wstrModelFilePath),
			modelFactoryFlags(modelFactoryFlags)
		{ }

		std::wstring wstrModelFilePath = L"";
		std::wstring wstrModelDirectoryPath;
		ModelFactory::flags modelFactoryFlags = ModelFactory::NONE;
	};

public:
	model(HALImp *pParentImp);

	std::shared_ptr<mesh> MakeMesh(const std::vector<vertex>& vertices);
	std::shared_ptr<mesh> AddMesh(const std::vector<vertex>& vertices);

	std::shared_ptr<mesh> MakeMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);
	std::shared_ptr<mesh> AddMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

	std::shared_ptr<mesh> GetChildMesh(int index);

	RESULT QueueMesh(const mesh::params &meshParams);

	// Async Callbacks
	RESULT HandleOnMeshReady(DimObj* pMesh, void *pContext);
	RESULT HandleOnMeshDiffuseTextureReady(texture *pTexture, void *pContext);
	RESULT HandleOnMeshSpecularTextureReady(texture *pTexture, void *pContext);
	RESULT HandleOnMeshNormalTextureReady(texture *pTexture, void *pContext);
	RESULT HandleOnMeshAmbientTextureReady(texture *pTexture, void *pContext);

	bool IsModelLoaded();
	float ModelLoadingProgress();

	/*
public:
	virtual RESULT Allocate() override;

	inline unsigned int NumberVertices() { return m_nVertices; }
	inline unsigned int NumberIndices() { return m_nIndices; }

private:
	unsigned int m_nVertices;
	unsigned int m_nIndices;

public:
	model(wchar_t *pszModelName);
	model(const std::vector<vertex>& vertices);
	model(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

private:
	RESULT SetVertices(const std::vector<vertex>& vertices);
	*/

	RESULT SetModelFilePath(std::wstring wstrFilepath);
	std::wstring GetModelFilePath();
	std::wstring GetModelDirectoryPath();

protected:
	RESULT SetDreamOS(DreamOS *pDOS);
	model::params m_params;

	unsigned int m_totalMeshes = 0;
	unsigned int m_totalTextures = 0;
	std::vector<unsigned long> m_pendingMeshIDs;
	std::vector<std::wstring> m_pendingTextures;

private:
	DreamOS *m_pDreamOS = nullptr;
};

#endif // ! MODEL_H_
