#include "FileLoader.h"
#include "PathManager.h"

#include <fstream>
#include <sstream>

bool FileLoaderHelper::LoadOBJFile(const std::wstring& obj_file_name,
								std::vector<vertex> &out_vertices) {

	std::vector<point> all_vertices;
	std::vector<point> all_uvs;
	std::vector<point> all_normals;

	std::vector<size_t> vertexIndices;
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
			all_vertices.emplace_back(x, y, z);
		}
		else if (type.compare("vt") == 0) {
			float u, v;
			std::sscanf(value.c_str(), "%f %f\n", &u, &v);
			all_uvs.emplace_back(u, v, 0);
		}
		else if (type.compare("vn") == 0) {
			float x, y, z;
			std::sscanf(value.c_str(), "%f %f %f\n", &x, &y, &z);
			all_normals.emplace_back(x, y, z);
		}
		else if (type.compare("f") == 0) {
			size_t newVertexIndices[3], newUvIndices[3], newNormalIndices[3];

			// Read as vertex/uv/normal format
			int num_matches = std::sscanf(value.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d\n",
										&newVertexIndices[0], &newUvIndices[0], &newNormalIndices[0],
										&newVertexIndices[1], &newUvIndices[1], &newNormalIndices[1],
										&newVertexIndices[2], &newUvIndices[2], &newNormalIndices[2]);

			if (num_matches != 9) {

				// Read as vertex//normal format
				num_matches = std::sscanf(value.c_str(), "%d//%d %d//%d %d//%d\n",
										&newVertexIndices[0], &newNormalIndices[0],
										&newVertexIndices[1], &newNormalIndices[1],
										&newVertexIndices[2], &newNormalIndices[2]);

				if (num_matches != 6) {
					// Cannot read face format.
					return false;
				}

			}

			for (int i = 0; i < 3; ++i) {
				vertexIndices.push_back(newVertexIndices[i]);
				uvIndices.push_back(newUvIndices[i]);
				normalIndices.push_back(newNormalIndices[i]);
			}
		}

	}

	for (int i : vertexIndices) {
		point vertex = all_vertices[i - 1];
		out_vertices.push_back(vertex);
	}

	obj_file.close();

	return true;
}
