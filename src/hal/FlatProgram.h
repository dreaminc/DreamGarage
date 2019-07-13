#ifndef FLAT_PROGRAM_H_
#define FLAT_PROGRAM_H_

// Dream OS
// DreamOS/HAL/FlatProgram.h
// Flat Program is an interface for render to texture
// this can be done with the render network, but for purposes of performance it might
// be smart to set these inputs directly. 

#include "./RESULT/EHM.h"

class framebuffer;
class stereocamera;
class FlatContext;

#include "HAL/Pipeline/ProgramNode.h"

#include <string>

class FlatProgram {
public:
	virtual RESULT SetFlatFramebuffer(framebuffer *pFramebuffer) = 0;
	virtual RESULT SetCamera(stereocamera *pCamera) = 0;
	virtual RESULT SetFlatContext(FlatContext *pFlatContext) = 0;
	virtual RESULT RenderFlatContext(FlatContext *pFlatContext) = 0;
};

#endif	// ! FLAT_PROGRAM_NODE_H_