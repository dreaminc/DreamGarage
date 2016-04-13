#ifndef FILE_LOADER_H_
#define FILE_LOADER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/FileLoader.h

#include <string>
#include <fstream>

class FileLoader {
public:
	static RESULT LoadFile(const std::wstring& strFilename, std::ifstream * &n_pFile);
};

#endif // ! FILE_LOADER_H_
