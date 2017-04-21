#include "OGLProgramReferenceGeometry.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "HAL/opengl/OpenGLImp.h"

OGLProgramReferenceGeometry::OGLProgramReferenceGeometry(OpenGLImp *pParentImp) :
	OGLProgramMinimal(pParentImp)
{
	// empty
}

RESULT OGLProgramReferenceGeometry::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	if (m_pParentImp->IsRenderReferenceGeometry() == false) {
		return R_SKIPPED;
	}

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	UseProgram();
	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	// Reference geometry
	RenderObjectStoreBoundingVolumes(m_pSceneGraph);

//Error:
	return r;
}