#include "Logger/Logger.h"
#include "FileLoader.h"
#include "PathManager.h"

#include <fstream>
#include <sstream>
#include <array>

bool FileLoaderHelper::LoadOBJFile(const std::wstring& strOBJFilename, multi_mesh_indices_t &out) {

	std::vector<point> all_positions;
	std::vector<point> all_uvs;
	std::vector<vector> all_normals;

	std::vector<int> positionIndices;
	std::vector<int> uvIndices;
	std::vector<int> normalIndices;

	multi_mesh_indices_t unordered_map;

	std::ifstream obj_file(strOBJFilename, std::ios::binary);

	std::vector<dimindex> indices;
	dimindex	index = 0;
	std::map<std::array<int, 3>, dimindex> vertexCache;

	if (!obj_file.is_open()) {
		return false;
	}

	std::string line;

	std::vector<vertex> vectorMeshVerticies;
	material_t materialCurrent;
	materialCurrent.name = "default";
	std::string strCurrentMaterialName = "unknown";

	std::map<std::string, material_t> material_map;

	int newPositionIndices[4]{ 0 }, newUvIndices[4]{ 0 }, newNormalIndices[4]{ 0 };

	DEBUG_LINEOUT("Loading %S from file", strOBJFilename.c_str());

	// converts a Wavefront obj index into an index in buffer (std::vector, cyclic and starting in 0)
	auto ConvertIndex = [](int& index, size_t bufferSize) {
		if (index == 0)
			index = static_cast<int>(bufferSize) - 1;
		else if (index > 0)
			index = (index - 1) % bufferSize;
		else // index < 0
			index = static_cast<int>(bufferSize - 1 - ((-index) % bufferSize));
	};

	// add new index to the mesh using cached vertices
	auto AddVertex = [&](int i) {
		std::array<int, 3> indicesKey { newPositionIndices[i], newNormalIndices[i], newUvIndices[i] };

		if (vertexCache.find(indicesKey) == vertexCache.end())
		{
			vectorMeshVerticies.emplace_back(all_positions[newPositionIndices[i]],
				all_normals[newNormalIndices[i]],
				uvcoord(all_uvs[newUvIndices[i]].x(), all_uvs[newUvIndices[i]].y()));

			indices.push_back(index);
			vertexCache[indicesKey] = index;
			++index;
		}
		else
		{
			indices.push_back(vertexCache[indicesKey]);
		}
	};

	while (std::getline(obj_file, line)) {

		auto space = line.find(' ');
		if (space == line.size()) {
			continue;
		}

		std::string type = line.substr(0, space);
		std::string value = line.substr(space + 1);

		if (type.compare("v") == 0) {
			point_precision x, y, z;
			std::sscanf(value.c_str(), "%f %f %f\n", &x, &y, &z);
			all_positions.emplace_back(x, y, z);
		}
		else if (type.compare("vt") == 0) {
			point_precision u, v;
			std::sscanf(value.c_str(), "%f %f\n", &u, &v);
			all_uvs.emplace_back(u, v, 0.0f); 
		}
		else if (type.compare("vn") == 0) {
			vector_precision x, y, z;
			std::sscanf(value.c_str(), "%f %f %f\n", &x, &y, &z);
			all_normals.emplace_back(x, y, z);
		}
		else if (type.compare("f") == 0) {
			// TODO: support >4 poligons
			enum class FaceType {
				PTN4,
				PTN3,
				PN3,
				PT3
			};

			FaceType face;
			// Read as vertex/uv/normal quad format
			face = FaceType::PTN4;
			int num_matches = std::sscanf(value.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
						&newPositionIndices[3], &newUvIndices[3], &newNormalIndices[3],
						&newPositionIndices[2], &newUvIndices[2], &newNormalIndices[2],
						&newPositionIndices[1], &newUvIndices[1], &newNormalIndices[1],
						&newPositionIndices[0], &newUvIndices[0], &newNormalIndices[0]);

			if (num_matches != 12) {
				// Read as vertex/uv/normal tri format
				face = FaceType::PTN3;
				num_matches = std::sscanf(value.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&newPositionIndices[2], &newUvIndices[2], &newNormalIndices[2],
					&newPositionIndices[1], &newUvIndices[1], &newNormalIndices[1],
					&newPositionIndices[0], &newUvIndices[0], &newNormalIndices[0]);

				if (num_matches != 9) {

					// Read as vertex//normal format
					face = FaceType::PN3;
					num_matches = std::sscanf(value.c_str(), "%d//%d %d//%d %d//%d\n",
						&newPositionIndices[2], &newNormalIndices[2],
						&newPositionIndices[1], &newNormalIndices[1],
						&newPositionIndices[0], &newNormalIndices[0]);

					if (num_matches != 6) {
						// Read as vertex//normal format
						face = FaceType::PT3;
						num_matches = std::sscanf(value.c_str(), "%d/%d %d/%d %d/%d\n",
							&newPositionIndices[2], &newUvIndices[2],
							&newPositionIndices[1], &newUvIndices[1],
							&newPositionIndices[0], &newUvIndices[0]);

						// Cannot read face format.
						//return false;
//						continue;
					}
				}
			}

			#define CYCLIC_BUFFER(index, buffer) \
						if (index == 0)\
							index = static_cast<int>(buffer.size()) - 1;\
						else if (index > 0)\
							index = (index - 1) % buffer.size();\
						else if (index < 0)\
							index = buffer.size() - 1 - ((-index) % buffer.size())
			
			for (int i = 0; i < 4; ++i)
			{
				ConvertIndex(newPositionIndices[i], all_positions.size());
				ConvertIndex(newUvIndices[i], all_uvs.size());
				ConvertIndex(newNormalIndices[i], all_normals.size());
			}

			#define ADD_VERTEX(i) \
			std::array<int, 3> ind {newPositionIndices[i], newNormalIndices[i], newUvIndices[i]};\
			if(vertexCache.find(ind) == vertexCache.end())\
			{	\
				vectorMeshVerticies.emplace_back(all_positions[newPositionIndices[i]],	\
					all_normals[newNormalIndices[i]],	\
					uvcoord(all_uvs[newUvIndices[i]].x(), all_uvs[newUvIndices[i]].y()));	\
				indices.push_back(index);	\
				vertexCache[ind] = index;	\
				++index;	\
			}	\
			else    \
			{	\
				indices.push_back(vertexCache[ind]);	\
			}

			switch (face)
			{
			case FaceType::PTN3:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : 3 - j;
					AddVertex(i);
				}
				break;
			case FaceType::PTN4:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : 3 - j;
					AddVertex(i);
				}
				for (int j = 0; j < 3; ++j) {
					// triangle order 0, 3, 2
					int i = (j == 1) ? j + 2 : j;
					AddVertex(i);
				}
				break;
			case FaceType::PN3:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : j + 1;
					AddVertex(i);
				}
				break;
			case FaceType::PT3:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : 3 - j;
					AddVertex(i);
				}
				break;
			}
		}
		else if (type.compare("usemtl") == 0) {
			unordered_map.push_back(std::make_pair(material_map[strCurrentMaterialName], mesh_t{ std::move(vectorMeshVerticies), std::move(indices) }));

			index = 0;
			vertexCache.clear();

			char	name[1024];
			std::sscanf(value.c_str(), "%s\n",
				name);

			strCurrentMaterialName = std::string(name);
		}
		else if (type.compare("mtllib") == 0) {
			char	file[1024];
			std::sscanf(value.c_str(), "%s\n",
				file);

			std::wstring path = strOBJFilename.substr(0, strOBJFilename.rfind('\\')) + L"\\";

			std::string mtl_file_name(file);
			std::wstring mtl_file_name_w(mtl_file_name.begin(), mtl_file_name.end());

			std::ifstream mtl_file(path + mtl_file_name_w, std::ios::binary);

			if (!mtl_file.is_open())
			{
				return false;
			}

			std::string mtl_line;

			material_t current_material;
			current_material.name = "unknown";

			while (std::getline(mtl_file, mtl_line)) {
				while (mtl_line[0] == ' ' || mtl_line[0] == '\t')
				{
					mtl_line = mtl_line.substr(1);
				}
				
				auto space = mtl_line.find(' ');

				if (space == mtl_line.size() || mtl_line.length() == 0) {
					continue;
				}

				std::string type = mtl_line.substr(0, space);
				std::string value = mtl_line.substr(space + 1);
				if (value[value.length() - 1] == '\r')
				{
					value = value.substr(0, value.length() - 1);
				}

				if (type.compare("newmtl") == 0) {
					material_map[current_material.name] = std::move(current_material);
					current_material.name = value;
				}
				else if (type.compare("map_Ka") == 0) {
					current_material.map_Ka = value;
				}
				else if (type.compare("map_Kd") == 0) {
					current_material.map_Kd = value;
				}
				else if (type.compare("map_Ks") == 0) {
					current_material.map_Ks = value;
				}
				else if (type.compare("Ka") == 0) {
					int num_matches = std::sscanf(value.c_str(), "%f %f %f",
						&current_material.Ka.r(),
						&current_material.Ka.g(),
						&current_material.Ka.b());

					current_material.Ka.a() = 1.0f;
				}
				else if (type.compare("Kd") == 0) {
					int num_matches = std::sscanf(value.c_str(), "%f %f %f",
						&current_material.Kd.r(),
						&current_material.Kd.g(),
						&current_material.Kd.b());

					current_material.Kd.a() = 1.0f;
				}
				else if (type.compare("Ks") == 0) {
					int num_matches = std::sscanf(value.c_str(), "%f %f %f",
						&current_material.Ks.r(),
						&current_material.Ks.g(),
						&current_material.Ks.b());

					current_material.Ks.a() = 1.0f;
				}
			}

			material_map[current_material.name] = std::move(current_material);
		}
	}
	
	for (auto i : material_map)
	{
		LOG(INFO) << "Loaded material for model " << strOBJFilename.substr(strOBJFilename.find_last_of(L"/\\")+1) << ":" << i.first
			<< ":Ka=" << LOG_rgba(i.second.Ka)
			<< ":Kd=" << LOG_rgba(i.second.Kd)
			<< ":Ks=" << LOG_rgba(i.second.Ks)
			<< ":map_Ka=" << i.second.map_Ka
			<< ":map_Kd=" << i.second.map_Kd
			<< ":map_Ks=" << i.second.map_Ks;
	}
	
	unordered_map.push_back(std::make_pair(material_map[strCurrentMaterialName], mesh_t{ std::move(vectorMeshVerticies), std::move(indices) }));

	// combine materials

	//typedef std::vector < std::pair<material_t, mesh_t>> multi_mesh_indices_t;

	while (!unordered_map.empty())
	{
		std::string materialName = unordered_map.front().first.name;

		auto iter = unordered_map.begin();
		while (iter != unordered_map.end())
		{
			if (iter->first.name == materialName)
			{
				out.push_back(std::move(*iter));
				iter = unordered_map.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	////

	obj_file.close();

	return true;
}

bool FileLoaderHelper::LoadOBJFile(const std::wstring& obj_file_name, multi_mesh_t &out) {
	return false;

	// obsolete
#if 0
	std::vector<point> all_positions;
	std::vector<point> all_uvs;
	std::vector<vector> all_normals;

	std::vector<int> positionIndices;
	std::vector<int> uvIndices;
	std::vector<int> normalIndices;

	std::ifstream obj_file(obj_file_name, std::ios::binary);

	if (!obj_file.is_open())
	{
		return false;
	}

	std::string line;

	std::vector<vertex> current_mesh;
	material_t current_material;
	current_material.name = "default";
	std::string current_material_name = "unknown";

	std::map<std::string, material_t> material_map;

	while (std::getline(obj_file, line)) {

		auto space = line.find(' ');

		if (space == line.size()) {
			continue;
		}
		std::string type = line.substr(0, space);
		std::string value = line.substr(space + 1);

		if (type.compare("v") == 0) {
			point_precision x, y, z;
			std::sscanf(value.c_str(), "%f %f %f\n", &x, &y, &z);
			all_positions.emplace_back(x, y, z);
		}
		else if (type.compare("vt") == 0) {
			point_precision u, v;
			std::sscanf(value.c_str(), "%f %f\n", &u, &v);
			all_uvs.emplace_back(u, v, 0.0f);
		}
		else if (type.compare("vn") == 0) {
			vector_precision x, y, z;
			std::sscanf(value.c_str(), "%f %f %f\n", &x, &y, &z);
			all_normals.emplace_back(x, y, z);
		}
		else if (type.compare("f") == 0) {
			int newPositionIndices[4]{ 0 }, newUvIndices[4]{ 0 }, newNormalIndices[4]{ 0 };

			enum class FaceType {
				PTN4,
				PTN3,
				PN3,
				PT3
			};

			FaceType face;
			// Read as vertex/uv/normal quad format
			face = FaceType::PTN4;
			int num_matches = std::sscanf(value.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
				&newPositionIndices[3], &newUvIndices[3], &newNormalIndices[3],
				&newPositionIndices[2], &newUvIndices[2], &newNormalIndices[2],
				&newPositionIndices[1], &newUvIndices[1], &newNormalIndices[1],
				&newPositionIndices[0], &newUvIndices[0], &newNormalIndices[0]);

			if (num_matches != 12) {
				// Read as vertex/uv/normal tri format
				face = FaceType::PTN3;
				num_matches = std::sscanf(value.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&newPositionIndices[2], &newUvIndices[2], &newNormalIndices[2],
					&newPositionIndices[1], &newUvIndices[1], &newNormalIndices[1],
					&newPositionIndices[0], &newUvIndices[0], &newNormalIndices[0]);

				if (num_matches != 9) {

					// Read as vertex//normal format
					face = FaceType::PN3;
					num_matches = std::sscanf(value.c_str(), "%d//%d %d//%d %d//%d\n",
						&newPositionIndices[2], &newNormalIndices[2],
						&newPositionIndices[1], &newNormalIndices[1],
						&newPositionIndices[0], &newNormalIndices[0]);

					if (num_matches != 6) {
						// Read as vertex//normal format
						face = FaceType::PT3;
						num_matches = std::sscanf(value.c_str(), "%d/%d %d/%d %d/%d\n",
							&newPositionIndices[2], &newUvIndices[2],
							&newPositionIndices[1], &newUvIndices[1],
							&newPositionIndices[0], &newUvIndices[0]);

						// Cannot read face format.
						//return false;
						//						continue;
					}
				}
			}

			for (int i = 0; i < 4; ++i)
			{
				/*
				if (newPositionIndices[i] <= 0)
				newPositionIndices[i] = all_positions.size() - 1 + newPositionIndices[i];
				else
				newPositionIndices[i]--;
				*/
				CYCLIC_BUFFER(newPositionIndices[i], all_positions);
				while (newPositionIndices[i] < 0)
				{
					CYCLIC_BUFFER(newPositionIndices[i], all_positions);
				}
				CYCLIC_BUFFER(newUvIndices[i], all_uvs);
				CYCLIC_BUFFER(newNormalIndices[i], all_normals);
			}
			switch (face)
			{
			case FaceType::PTN3:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : 3 - j;
					current_mesh.emplace_back(all_positions[newPositionIndices[i]],
						all_normals[newNormalIndices[i]],
						uvcoord(all_uvs[newUvIndices[i]].x(), all_uvs[newUvIndices[i]].y()));
				}
				break;
			case FaceType::PTN4:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : 3 - j;
					current_mesh.emplace_back(all_positions[newPositionIndices[i]],
						all_normals[newNormalIndices[i]],
						uvcoord(all_uvs[newUvIndices[i]].x(), all_uvs[newUvIndices[i]].y()));
				}
				for (int j = 0; j < 3; ++j) {
					// triangle order 0, 3, 2
					int i = (j == 1) ? j + 2 : j;
					current_mesh.emplace_back(all_positions[newPositionIndices[i]],
						all_normals[newNormalIndices[i]],
						uvcoord(all_uvs[newUvIndices[i]].x(), all_uvs[newUvIndices[i]].y()));
				}
				break;
			case FaceType::PN3:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : j + 1;
					current_mesh.emplace_back(all_positions[newPositionIndices[i]],
						all_normals[newNormalIndices[i]]);
				}
				break;
			case FaceType::PT3:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : 3 - j;
					current_mesh.emplace_back(all_positions[newPositionIndices[i]],
						all_uvs[newUvIndices[i]]);
				}
				break;
			}

			for (int i = 0; i < 3; ++i) {
				positionIndices.push_back(newPositionIndices[i]);
				uvIndices.push_back(newUvIndices[i]);
				normalIndices.push_back(newNormalIndices[i]);
			}

			if (num_matches == 12) {
				for (int j = 0; j < 3; ++j) {
					int i = j == 0 ? j : j + 1;
					positionIndices.push_back(newPositionIndices[i]);
					uvIndices.push_back(newUvIndices[i]);
					normalIndices.push_back(newNormalIndices[i]);

				}
			}

		}
		else if (type.compare("usemtl") == 0) {
			//			out.push_back(std::make_pair(std::move(current_material), std::move(current_mesh)));
			out.push_back(std::make_pair(material_map[current_material_name], std::move(current_mesh)));

			char	name[1024];
			std::sscanf(value.c_str(), "%s\n",
				name);

			current_material_name = std::string(name);
		}
		else if (type.compare("mtllib") == 0) {
			char	file[1024];
			std::sscanf(value.c_str(), "%s\n",
				file);

			std::wstring path = obj_file_name.substr(0, obj_file_name.rfind('\\')) + L"\\";

			std::string mtl_file_name(file);
			std::wstring mtl_file_name_w(mtl_file_name.begin(), mtl_file_name.end());

			std::ifstream mtl_file(path + mtl_file_name_w, std::ios::binary);

			if (!mtl_file.is_open())
			{
				return false;
			}

			std::string mtl_line;

			material_t current_material;
			current_material.name = "unknown";

			while (std::getline(mtl_file, mtl_line)) {
				while (mtl_line[0] == ' ')
				{
					mtl_line = mtl_line.substr(1);
				}

				auto space = mtl_line.find(' ');

				if (space == mtl_line.size() || mtl_line.length() == 0) {
					continue;
				}

				std::string type = mtl_line.substr(0, space);
				std::string value = mtl_line.substr(space + 1);
				if (value[value.length() - 1] == '\r')
				{
					value = value.substr(0, value.length() - 1);
				}

				if (type.compare("newmtl") == 0) {
					material_map[current_material.name] = std::move(current_material);
					current_material.name = value;
				}
				else if (type.compare("\tmap_Kd") == 0) {
					current_material.map_Kd = value;
				}
				else if (type.compare("map_Kd") == 0) {
					current_material.map_Kd = value;
				}
			}

			material_map[current_material.name] = std::move(current_material);
		}
	}

	for (int index = 0; index < positionIndices.size(); ++index) {
		// TBD: add texture coordinates

		// Add a vertex with position and normal
		//out_vertices.emplace_back(all_positions[positionIndices[index] - 1], all_normals[normalIndices[index] - 1]);
	}

	//	out.push_back(std::make_pair(std::move(current_material), std::move(current_mesh)));
	out.push_back(std::make_pair(material_map[current_material_name], std::move(current_mesh)));

	obj_file.close();

	return true;
#endif
}
bool FileLoaderHelper::LoadOBJFile(const std::wstring& obj_file_name,
	std::vector<vertex> &out_vertices) {

	std::vector<point> all_positions;
	std::vector<point> all_uvs;
	std::vector<vector> all_normals;

	std::vector<size_t> positionIndices;
	std::vector<size_t> uvIndices;
	std::vector<size_t> normalIndices;

	std::ifstream obj_file(obj_file_name, std::ios::binary);

	if (!obj_file.is_open())
	{
		return false;
	}

	std::string line;

	while (std::getline(obj_file, line)) {

		auto space = line.find(' ');

		if (space == line.size()) {
			continue;
		}

		std::string type = line.substr(0, space);
		std::string value = line.substr(space + 1);

		if (type.compare("v") == 0) {
			float x, y, z;
			std::sscanf(value.c_str(), "%f %f %f\n", &x, &y, &z);
			all_positions.emplace_back(x, y, z);
		}
		else if (type.compare("vt") == 0) {
			float u, v;
			std::sscanf(value.c_str(), "%f %f\n", &u, &v);
			all_uvs.emplace_back(u, v, 0.0f);
		}
		else if (type.compare("vn") == 0) {
			float x, y, z;
			std::sscanf(value.c_str(), "%f %f %f\n", &x, &y, &z);
			all_normals.emplace_back(x, y, z);
		}
		else if (type.compare("f") == 0) {
			size_t newPositionIndices[4], newUvIndices[4], newNormalIndices[4];

			// Read as vertex/uv/normal quad format
			// %Iu is for windows 64, %zu is for linux 
			int num_matches = std::sscanf(value.c_str(), "%Iu/%Iu/%Iu %Iu/%Iu/%Iu %Iu/%Iu/%Iu %Iu/%Iu/%Iu\n",
				&newPositionIndices[3], &newUvIndices[3], &newNormalIndices[3],
				&newPositionIndices[2], &newUvIndices[2], &newNormalIndices[2],
				&newPositionIndices[1], &newUvIndices[1], &newNormalIndices[1],
				&newPositionIndices[0], &newUvIndices[0], &newNormalIndices[0]);

			if (num_matches != 12) {
				// Read as vertex/uv/normal tri format
				num_matches = std::sscanf(value.c_str(), "%Iu/%Iu/%Iu %Iu/%Iu/%Iu %Iu/%Iu/%Iu\n",
					&newPositionIndices[2], &newUvIndices[2], &newNormalIndices[2],
					&newPositionIndices[1], &newUvIndices[1], &newNormalIndices[1],
					&newPositionIndices[0], &newUvIndices[0], &newNormalIndices[0]);

				if (num_matches != 9) {

					// Read as vertex//normal format
					num_matches = std::sscanf(value.c_str(), "%Iu//%Iu %Iu//%Iu %Iu//%Iu\n",
						&newPositionIndices[2], &newNormalIndices[2],
						&newPositionIndices[1], &newNormalIndices[1],
						&newPositionIndices[0], &newNormalIndices[0]);

					if (num_matches != 6) {
						// Cannot read face format.
						return false;
					}
				}
			}
			for (int i = 0; i < 3; ++i) {
				positionIndices.push_back(newPositionIndices[i]);
				uvIndices.push_back(newUvIndices[i]);
				normalIndices.push_back(newNormalIndices[i]);
			}

			if (num_matches == 12) {
				for (int j = 0; j < 3; ++j) {
					int i = j == 0 ? j : j + 1;
					positionIndices.push_back(newPositionIndices[i]);
					uvIndices.push_back(newUvIndices[i]);
					normalIndices.push_back(newNormalIndices[i]);

				}
			}

		}
	}

	for (int index = 0; index < positionIndices.size(); ++index) {
		// TBD: add texture coordinates

		// Add a vertex with position and normal
		out_vertices.emplace_back(all_positions[positionIndices[index] - 1], all_normals[normalIndices[index] - 1]);
	}

	obj_file.close();

	return true;
}
