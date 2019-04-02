#include "OGLVolume.h"

OGLVolume::OGLVolume(OpenGLImp *pParentImp, volume::params *pVolumeParams) :
	volume(pVolumeParams),
	OGLObj(pParentImp)
{
	//
}

OGLVolume::OGLVolume(OpenGLImp *pParentImp) :
	volume(1.0f),
	OGLObj(pParentImp)
{
	// 
}

OGLVolume::OGLVolume(OpenGLImp *pParentImp, double width, double length, double height, bool fTriangleBased) :
	volume(width, length, height, fTriangleBased),
	OGLObj(pParentImp)
{
	// 
}

OGLVolume::OGLVolume(OpenGLImp *pParentImp, double side, bool fTriangleBased) :
	volume(side, fTriangleBased),
	OGLObj(pParentImp)
{
	// 
}

OGLVolume::OGLVolume(OpenGLImp *pParentImp, BoundingBox* pBoundingBox, bool fTriangleBased) :
	volume(pBoundingBox, fTriangleBased),
	OGLObj(pParentImp)
{
	// 
}

RESULT OGLVolume::UpdateFromBoundingBox(BoundingBox* pBoundingBox) {
	RESULT r = R_PASS;

	CR(volume::UpdateFromBoundingBox(pBoundingBox));
	CR(UpdateOGLBuffers());

Error:
	return r;
}

RESULT OGLVolume::UpdateFromVertices(DimObj *pDimObj) {
	RESULT r = R_PASS;

	// TODO: Update the bounding box from vertices

	//volume *pVolume = reinterpret_cast<volume*>(GetDimObj());
	//CR(pVolume->UpdateFromVertices(pDimObj));

	CR(UpdateOGLBuffers());

Error:
	return r;
}

// TODO: Need to make this better
RESULT OGLVolume::UpdateBuffers() {
	return UpdateOGLBuffers();
}

// Override this method when necessary by a child object
// Many objects will not need to though. 
// Critical path, EHM removed
// manually debug
RESULT OGLVolume::Render() {
	RESULT r = R_PASS;

	m_pParentImp->glBindVertexArray(m_hVAO);
	m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO);
	m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO);

	// TODO: This should be made more uniform (functions / caps struct etc)
	if (IsWireframe()) {
		GLint previousPolygonMode[2]{ 0 };
		GLboolean previousCullFaceEnabled;
		glGetIntegerv(GL_POLYGON_MODE, previousPolygonMode);
		previousCullFaceEnabled = glIsEnabled(GL_CULL_FACE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (previousCullFaceEnabled) {
			glDisable(GL_CULL_FACE);
		}

		if (IsTriangleBased()) {
			glDrawElements(GL_TRIANGLES, NumberIndices(), GL_UNSIGNED_INT, NULL);
		}
		else {
			for (int i = 0; i < NUM_VOLUME_QUADS; i++) {
				// TODO: Is there a better way to do this without 6 separate draw calls - maybe use a line 
				// strip instead
				glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(sizeof(dimindex)*(4 * i)));
			}
		}

		if (previousPolygonMode[1] != 0) {
			glPolygonMode(GL_FRONT, previousPolygonMode[0]);
			glPolygonMode(GL_BACK, previousPolygonMode[1]);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, previousPolygonMode[0]);
		}

		if (previousCullFaceEnabled) {
			glEnable(GL_CULL_FACE);
		}
	}
	else {
		glDrawElements(GL_TRIANGLES, NumberIndices(), GL_UNSIGNED_INT, NULL);
	}

	//Error:
	return r;
}