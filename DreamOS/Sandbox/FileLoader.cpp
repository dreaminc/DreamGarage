#include "FileLoader.h"
#include "PathManager.h"

RESULT FileLoader::LoadFile(const std::wstring& strFilename, std::ifstream * &n_pFile) {
	RESULT r = R_PASS;
	
	n_pFile = nullptr;

	n_pFile = new std::ifstream(strFilename, std::ios::binary);
	CBM((n_pFile->is_open()), "Failed to open file %S", strFilename.c_str());

	return r;

Error:
	if(n_pFile != nullptr) {
		n_pFile->close();
		delete n_pFile;
		n_pFile = nullptr;
	}
	return r;
}
