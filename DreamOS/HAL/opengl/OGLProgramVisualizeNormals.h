#ifndef OGLPROGRAM_VISUALIZE_NORMALS_H_
#define OGLPROGRAM_VISUALIZE_NORMALS_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramVisualizeNormals.h
// This is a debug program that will visualize normals in a scene
// using a geometry shader

#include "./RESULT/EHM.h"
#include "OGLProgramMinimal.h"

class ObjectStore;
class stereocamera;

class OGLProgramVisualizeNormals : public OGLProgramMinimal {
public:
	OGLProgramVisualizeNormals(OpenGLImp *pParentImp);
	virtual RESULT ProcessNode(long frameID) override;

	virtual RESULT OGLInitialize() override;
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetupConnections() override;
};

#endif // ! OGLPROGRAM_VISUALIZE_NORMALS_H_