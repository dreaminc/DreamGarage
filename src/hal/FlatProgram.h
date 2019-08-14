#ifndef FLAT_PROGRAM_H_
#define FLAT_PROGRAM_H_

#include "core/ehm/EHM.h"

// Dream HAL
// dos/src/hal/FlatProgram.h

// Flat Program is an interface for render to texture
// this can be done with the render network, but for purposes of performance it might
// be smart to set these inputs directly. 

#include <string>

#include "pipeline/ProgramNode.h"

#include "core/types/DObject.h"

class framebuffer;
class stereocamera;
class FlatContext;

class FlatProgram : public DObject {
public:
	virtual RESULT SetFlatFramebuffer(framebuffer *pFramebuffer) = 0;
	virtual RESULT SetCamera(stereocamera *pCamera) = 0;
	virtual RESULT SetFlatContext(FlatContext *pFlatContext) = 0;
	virtual RESULT RenderFlatContext(FlatContext *pFlatContext) = 0;
};

#endif	// ! FLAT_PROGRAM_NODE_H_