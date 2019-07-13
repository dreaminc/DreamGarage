#ifndef OGLPROGRAM_DEBUG_OVERLAY_H_
#define OGLPROGRAM_DEBUG_OVERLAY_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLProgramDebugOverlay.h

// The DebugOverlay program is a thin wrapper of the 
// minimal shader that allows for pass through rendering
// and disables depth checking - this is for rendering debug geometry

#include "OGLProgramMinimal.h"

class ObjectStore;
class stereocamera;

class OGLProgramDebugOverlay : public OGLProgramMinimal {
public:
	OGLProgramDebugOverlay(OGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	virtual RESULT OGLInitialize() override;

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;
};

#endif // ! OGLPROGRAM_MINIMAL_H_