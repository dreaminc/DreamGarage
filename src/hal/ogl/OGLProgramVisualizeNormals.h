#ifndef OGLPROGRAM_VISUALIZE_NORMALS_H_
#define OGLPROGRAM_VISUALIZE_NORMALS_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLProgramVisualizeNormals.h

// This is a debug program that will visualize normals in a scene
// using a geometry shader

#include "OGLProgramMinimal.h"

class ObjectStore;
class stereocamera;

class OGLProgramVisualizeNormals : public OGLProgramMinimal {
public:
	OGLProgramVisualizeNormals(OGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	virtual RESULT ProcessNode(long frameID) override;

	virtual RESULT OGLInitialize() override;
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetupConnections() override;

	virtual RESULT SetCameraUniforms(camera *pCamera) override;
	virtual RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) override;
	virtual RESULT SetObjectUniforms(DimObj *pDimObj) override;

protected:
	OGLVertexAttributeVector *m_pVertexAttributeNormal = nullptr;

	OGLUniformMatrix4 *m_pUniformProjectionMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformViewMatrix = nullptr;
};

#endif // ! OGLPROGRAM_VISUALIZE_NORMALS_H_