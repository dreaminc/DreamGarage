#ifndef FILE_LOADER_H_
#define FILE_LOADER_H_

#include <string>
#include <vector>
#include "Primitives/Vertex.h"

// DREAM OS
// DreamOS/Sandbox/FileLoader.h

class FileLoaderHelper {
public:
	static bool LoadOBJFile(const std::wstring& obj_file_name,
		std::vector<vertex> &out_vertices);
};

#endif // ! FILE_LOADER_H_
