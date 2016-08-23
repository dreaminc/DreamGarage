
#include "FileLoader.h"
#include "PathManager.h"

#include <fstream>
#include <sstream>

bool FileLoaderHelper::LoadOBJFile(const std::wstring& obj_file_name, multi_mesh_indices_t &out) {

	std::vector<point> all_positions;
	std::vector<point> all_uvs;
	std::vector<vector> all_normals;

	std::vector<int> positionIndices;
	std::vector<int> uvIndices;
	std::vector<int> normalIndices;

	std::ifstream obj_file(obj_file_name, std::ios::binary);

	std::vector<dimindex> indices;
	dimindex	index = 0;
	std::map<int, dimindex> objIndexMap;

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
						if (index <= 0)\
							index = static_cast<int>(buffer.size()) - 1 + index;\
						else\
							index--

			/*
			#define CYCLIC_BUFFER(index, buffer) \
						if (buffer.size() > 0) \
							index = (index - 1) % buffer.size()
			*/

			for (int i = 0; i < 4; ++i)
			{
				CYCLIC_BUFFER(newPositionIndices[i], all_positions);
				while (newPositionIndices[i] < 0)
				{
					CYCLIC_BUFFER(newPositionIndices[i], all_positions);
				}
				CYCLIC_BUFFER(newUvIndices[i], all_uvs);
				CYCLIC_BUFFER(newNormalIndices[i], all_normals);
			}

			// TODO: now caching vertices is done base only on position.
			// we need to set a different vertex if either position/normal/uv is different, using a multi map for objIndexMap
			#define ADD_VERTEX(i) \
			if(objIndexMap.find(newPositionIndices[i]) == objIndexMap.end())\
			{	\
				current_mesh.emplace_back(all_positions[newPositionIndices[i]],	\
					all_normals[newNormalIndices[i]],	\
					uvcoord(all_uvs[newUvIndices[i]].x(), all_uvs[newUvIndices[i]].y()));	\
				indices.push_back(index);	\
				objIndexMap[newPositionIndices[i]] = index;	\
				++index;	\
			}	\
			else    \
			{	\
				indices.push_back(objIndexMap[newPositionIndices[i]]);	\
			}

			switch (face)
			{
			case FaceType::PTN3:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : 3 - j;
					ADD_VERTEX(i);
				}
				break;
			case FaceType::PTN4:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : 3 - j;
					ADD_VERTEX(i);
				}
				for (int j = 0; j < 3; ++j) {
					// triangle order 0, 3, 2
					int i = (j == 1) ? j + 2 : j;
					ADD_VERTEX(i);
				}
				break;
			case FaceType::PN3:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : j + 1;
					ADD_VERTEX(i);
				}
				break;
			case FaceType::PT3:
				for (int j = 0; j < 3; ++j) {
					int i = (j == 0) ? j : 3 - j;
					ADD_VERTEX(i);
				}
				break;
			}
		}
		else if (type.compare("usemtl") == 0) {
			out.push_back(std::make_pair(material_map[current_material_name], mesh_t{ std::move(current_mesh), std::move(indices) }));

			index = 0;
			objIndexMap.clear();

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
				else if (type.compare("\tmap_Kd") == 0 || type.compare("map_Kd") == 0) {
					current_material.map_Kd = value;
				}
				else if (type.compare("\tKd") == 0 || type.compare("Kd") == 0) {
					int num_matches = std::sscanf(value.c_str(), "%f %f %f",
						&current_material.Kd.r(),
						&current_material.Kd.g(),
						&current_material.Kd.b());

					current_material.Kd.a() = 1.0f;
				}
			}

			material_map[current_material.name] = std::move(current_material);
		}
	}
	/*
	for (auto i : material_map)
	{
		OutputDebugStringA((std::string("DOS::Material ") + i.first + ":" + std::to_string(i.second.Kd.r())
			+ "," + std::to_string(i.second.Kd.g())
			+ "," + std::to_string(i.second.Kd.b())
			+ "," + std::to_string(i.second.Kd.a())
			+ ";" + i.second.map_Kd).c_str());

	}
	*/
	out.push_back(std::make_pair(material_map[current_material_name], mesh_t{ std::move(current_mesh), std::move(indices) }));

	obj_file.close();

	return true;
}

bool FileLoaderHelper::LoadOBJFile(const std::wstring& obj_file_name, multi_mesh_t &out) {

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
