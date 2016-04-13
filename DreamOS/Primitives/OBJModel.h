#ifndef OBJ_MODEL_H_
#define OBJ_MODEL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/OBJModel.h
// This is the OBJ model that can use the PathManager and FileLoader to load a
// file from the FS and create a model object

#include <vector>
#include "DimObj.h"
#include "Vertex.h"
#include "Sandbox\PathManager.h"
#include "Sandbox\FileLoader.h"
#include "point.h"
#include "uvcoord.h"

#include "model.h"

#include <fstream>
#include <sstream>

class OBJModel : public model {
public:
	OBJModel() :
		model()
	{
		// empty for now
	}

	RESULT LoadFile(std::ifstream *pFile) {
		RESULT r = R_PASS;
		
		unsigned int vertexCount = 0;

		std::vector<point> points;
		std::vector<uvcoord> uvcoords;
		std::vector<vector> normals;

		std::vector<dimindex> pointIndices;
		std::vector<dimindex> uvIndices;
		std::vector<dimindex> normalIndices;

		std::vector<vertex> vertices;

		std::string line;

		while (std::getline(*pFile, line)) {

			auto space = line.find(' ');

			if (space == line.size()) {
				continue;
			}

			std::string type = line.substr(0, space);
			std::string value = line.substr(space + 1);

			if (type.compare("v") == 0) {
				float x, y, z;
				std::sscanf(value.c_str(), "%f %f %f\n", &x, &y, &z);
				points.emplace_back(x, y, z);
			}
			else if (type.compare("vt") == 0) {
				float u, v;
				std::sscanf(value.c_str(), "%f %f\n", &u, &v);
				uvcoords.emplace_back(u, v);
			}
			else if (type.compare("vn") == 0) {
				float x, y, z;
				std::sscanf(value.c_str(), "%f %f %f\n", &x, &y, &z);
				normals.emplace_back(x, y, z);
			}
			else if (type.compare("f") == 0) {
				size_t newPositionIndices[3] = { 0, 0, 0 };
				size_t newUvIndices[3] = { 0, 0, 0 };
				size_t newNormalIndices[3] = { 0, 0, 0 };

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

					CBM((num_matches == 6), "Wrong OBJ file structure on line %d");

				}

				for (int i = 0; i < 3; ++i) {
					pointIndices.push_back(newPositionIndices[i]);
					uvIndices.push_back(newUvIndices[i]);
					normalIndices.push_back(newNormalIndices[i]);
				}
			}
		}

		for (int index = 0; index < pointIndices.size(); ++index) {
			if (static_cast<long>(normalIndices[index]) - 1 >= 0) {
				if(static_cast<long>(uvIndices[index]) - 1 >= 0) {
					vertices.emplace_back(points[pointIndices[index] - 1], normals[normalIndices[index] - 1], uvcoords[uvIndices[index] - 1]);
				}
				else {
					vertices.emplace_back(points[pointIndices[index] - 1], normals[normalIndices[index] - 1]);
				}
			}				
			else {
				vertices.emplace_back(points[pointIndices[index] - 1]);
			}
		}

		// Allocate the buffers
		SetNumVertices(vertices.size());
		SetNumIndices(vertices.size());
		CR(Allocate());

		// Set Vertices
		for (vertex& v : vertices) {
			m_pVertices[vertexCount++] = vertex(v);
		}

	Error:
		return r;
	}
};

#endif	// ! OBJ_MODEL_H_