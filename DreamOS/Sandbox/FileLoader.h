#ifndef FILE_LOADER_H_
#define FILE_LOADER_H_

#include <string>
#include <vector>
#include "Primitives/Vertex.h"

// DREAM OS
// DreamOS/Sandbox/FileLoader.h

class FileLoaderHelper {
public:
	struct material_t
	{
		std::string	name;
		std::string	map_Kd;
	};

	typedef std::vector < std::pair<material_t, std::vector<vertex>>> multi_mesh_t;

	static bool FileLoaderHelper::LoadOBJFile(const std::wstring& obj_file_name,
		multi_mesh_t &out);

	static bool LoadOBJFile(const std::wstring& obj_file_name,
		std::vector<vertex> &out_vertices);
};

#endif // ! FILE_LOADER_H_
