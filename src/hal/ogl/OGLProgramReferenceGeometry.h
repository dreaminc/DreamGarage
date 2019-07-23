#ifndef OGLPROGRAM_REFERENCE_GEOMETRY_H_
#define OGLPROGRAM_REFERENCE_GEOMETRY_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramReferenceGeometry.h
// The Reference Geometry program is identical to the Minimal program,
// but it renders the reference geometry instead of the ObjectStore

#include "./RESULT/EHM.h"
#include "OGLProgramMinimal.h"

class ObjectStore;
class stereocamera;

class OGLProgramReferenceGeometry : public OGLProgramMinimal {
public:
	OGLProgramReferenceGeometry(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	virtual RESULT ProcessNode(long frameID) override;

	virtual RESULT OGLInitialize() override;
	virtual RESULT SetupConnections() override;
};

#endif // ! OGLPROGRAM_REFERENCE_GEOMETRY_H_