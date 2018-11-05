#ifndef OGLPROGRAM_DEBUG_OVERLAY_H_
#define OGLPROGRAM_DEBUG_OVERLAY_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramDebugOverlay.h
// The DebugOverlay program is a thin wrapper of the 
// minimal shader that allows for pass through rendering
// and disables depth checking - this is for rendering debug geometry

#include "./RESULT/EHM.h"
#include "OGLProgramMinimal.h"


class ObjectStore;
class stereocamera;

class OGLProgramDebugOverlay : public OGLProgramMinimal {
public:
	OGLProgramDebugOverlay(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	virtual RESULT OGLInitialize() override;

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;
};

#endif // ! OGLPROGRAM_MINIMAL_H_