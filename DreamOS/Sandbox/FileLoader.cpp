#include "FileLoader.h"
#include "PathManager.h"

#include <fstream>
#include <sstream>

bool FileLoaderHelper::LoadOBJFile(const std::wstring& obj_file_name,
								std::vector<vertex> &out_vertices) {

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
			all_uvs.emplace_back(u, v, 0);
		}
		else if (type.compare("vn") == 0) {
			float x, y, z;
			std::sscanf(value.c_str(), "%f %f %f\n", &x, &y, &z);
			all_normals.emplace_back(x, y, z);
		}
		else if (type.compare("f") == 0) {
			size_t newPositionIndices[3], newUvIndices[3], newNormalIndices[3];

			// Read as vertex/uv/normal format
			int num_matches = std::sscanf(value.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d\n",
										&newPositionIndices[2], &newUvIndices[2], &newNormalIndices[2],
										&newPositionIndices[1], &newUvIndices[1], &newNormalIndices[1],
										&newPositionIndices[0], &newUvIndices[0], &newNormalIndices[0]);

			if (num_matches != 9) {

				// Read as vertex//normal format
				num_matches = std::sscanf(value.c_str(), "%d//%d %d//%d %d//%d\n",
										&newPositionIndices[2], &newNormalIndices[2],
										&newPositionIndices[1], &newNormalIndices[1],
										&newPositionIndices[0], &newNormalIndices[0]);

				if (num_matches != 6) {
					// Cannot read face format.
					return false;
				}

			}

			for (int i = 0; i < 3; ++i) {
				positionIndices.push_back(newPositionIndices[i]);
				uvIndices.push_back(newUvIndices[i]);
				normalIndices.push_back(newNormalIndices[i]);
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
