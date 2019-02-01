#ifndef OGLPROGRAM_BILLBOARD_H_
#define OGLPROGRAM_BILLBOARD_H_

#include "./RESULT/EHM.h"
#include "OGLProgramMinimal.h"

class ObjectStore;
class stereocamera;

class OGLProgramBillboard : public OGLProgramMinimal {
public:
	OGLProgramBillboard(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	virtual RESULT ProcessNode(long frameID) override;

	virtual RESULT OGLInitialize() override;
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetObjectUniforms(DimObj *pDimObj) override;

protected:
	OGLUniformMatrix4 *m_pUniformProjectionMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformViewMatrix = nullptr;

	OGLUniform *m_pUniformViewWidth;
	OGLUniform *m_pUniformViewHeight;
};


#endif // ! OGLPROGRAM_BILLBOARD_H_