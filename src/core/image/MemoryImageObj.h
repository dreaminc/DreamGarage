#ifndef MEMORY_IMAGE_OBJ_H_
#define MEMORY_IMAGE_OBJ_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/image/MemoryImageObj.h
// A memory image obj

#include "image.h"

#include <string>

class MemoryImageObj : public image {
public:
	MemoryImageObj(int width, int height, int channels);

	// TODO: Do we want it?
	virtual RESULT LoadFromPath() { return R_NOT_IMPLEMENTED; };
	virtual RESULT LoadFromMemory() override;

private:
	// empty
};

#endif // ! MEMORY_IMAGE_OBJ_H_