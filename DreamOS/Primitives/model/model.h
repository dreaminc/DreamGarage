#ifndef MODEL_H_
#define MODEL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/model/model.h

//#include <vector>

//#include "Primitives/DimObj.h"
//#include "Primitives/Vertex.h"
//#include "Primitives/point.h"
//#include "Primitives/color.h"

#include "Primitives/composite.h"

#include "Primitives/PrimParams.h"

class HALImp;
class mesh;

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

	RESULT QueueMesh(std::string strName, const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);
	RESULT HandleOnMeshReady(DimObj* pMesh, void *pContext);

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

private:
	model::params m_params;
	DreamOS *m_pDreamOS = nullptr;
};

#endif // ! MODEL_H_
