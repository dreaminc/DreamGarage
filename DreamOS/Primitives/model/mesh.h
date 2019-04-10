#ifndef MESH_H_
#define MESH_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/model/mesh.h

#include <vector>

#include "Primitives/DimObj.h"
#include "Primitives/Vertex.h"
#include "Primitives/point.h"
#include "Primitives/color.h"

class mesh : public virtual DimObj {
public:
	friend class model;

public:
	struct params :
		public PrimParams
	{
		virtual PRIMITIVE_TYPE GetPrimitiveType() override { return PRIMITIVE_TYPE::MESH; }

		params(std::string strName, std::vector<vertex> vertices, std::vector<dimindex> indices) :
			strName(strName),
			vertices(vertices),
			indices(indices)
		{ }

		params(std::string strName) :
			strName(strName)
		{ }

		std::string strName;
		std::vector<vertex> vertices;
		std::vector<dimindex> indices;
		
		// material
		material meshMaterial = material();
		std::vector<std::wstring> diffuseTexturePaths;
		std::vector<std::wstring> specularTexturePaths;
		std::vector<std::wstring> normalsTexturePaths;
		std::vector<std::wstring> ambientTexturePaths;
	};

public:
	virtual RESULT Allocate() override;

	inline unsigned int NumberVertices() { return m_nVertices; }
	inline unsigned int NumberIndices() { return m_nIndices; }

private:
	unsigned int m_nVertices;
	unsigned int m_nIndices;

public:
	//mesh(wchar_t *pszModelName);
	mesh(mesh::params *pMeshParams);
	mesh(const std::vector<vertex>& vertices);
	mesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

	RESULT SetName(std::string strName);
	std::string GetName();

private:
	RESULT SetVertices(const std::vector<vertex>& vertices);

protected:
	mesh::params m_params;
};

#endif // ! MESH_H_