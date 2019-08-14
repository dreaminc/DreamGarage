#ifndef OGLPROGRAM_REFERENCE_GEOMETRY_H_
#define OGLPROGRAM_REFERENCE_GEOMETRY_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLProgramReferenceGeometry.h

// The Reference Geometry program is identical to the Minimal program,
// but it renders the reference geometry instead of the ObjectStore

#include "OGLProgramMinimal.h"

class ObjectStore;
class stereocamera;

class OGLProgramReferenceGeometry : public OGLProgramMinimal {
public:
	OGLProgramReferenceGeometry(OGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	virtual RESULT ProcessNode(long frameID) override;

	virtual RESULT OGLInitialize() override;
	virtual RESULT SetupConnections() override;
};

#endif // ! OGLPROGRAM_REFERENCE_GEOMETRY_H_