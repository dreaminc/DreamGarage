#ifndef OGL_VOLUME_H_
#define OGL_VOLUME_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLVolume.h
// OpenGL Volume Object

#include "OGLObj.h"
#include "Primitives/volume.h"

class OGLVolume : public volume, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLVolume(OpenGLImp *pParentImp) :
		volume(1.0f),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLVolume(OpenGLImp *pParentImp, double width, double length, double height, bool fTriangleBased) :
		volume(width, length, height, fTriangleBased),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLVolume(OpenGLImp *pParentImp, double side, bool fTriangleBased) :
		volume(side, fTriangleBased),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLVolume(OpenGLImp *pParentImp, BoundingBox* pBoundingBox, bool fTriangleBased) :
		volume(pBoundingBox, fTriangleBased),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	RESULT UpdateFromBoundingBox(BoundingBox* pBoundingBox) {
		RESULT r = R_PASS;

		volume *pVolume = (volume*)(GetDimObj());
		CR(pVolume->UpdateFromBoundingBox(pBoundingBox));
		CR(UpdateOGLBuffers());

	Error:
		return r;
	}

	RESULT UpdateFromVertices(DimObj *pDimObj) {
		RESULT r = R_PASS;

		// TODO: Update the bounding box from verts

		volume *pVolume = (volume*)(GetDimObj());
		//CR(pVolume->UpdateFromVertices(pDimObj));

		CR(UpdateOGLBuffers());

	Error:
		return r;
	}

	// TODO: Need to make this better
	RESULT UpdateBuffers() override {
		return UpdateOGLBuffers();
	}

	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	// Critical path, EHM removed
	// manually debug
	virtual RESULT Render() override {
		RESULT r = R_PASS;

		// TODO: Rethink this since it's in the critical path
		volume *pVolume = (volume*)(GetDimObj());

		m_pParentImp->glBindVertexArray(m_hVAO);
		m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO);
		m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO);

		// TODO: This should be made more uniform (functions / caps struct etc)
		if (pVolume->IsWireframe()) {
			GLint previousPolygonMode[2]{ 0 };
			GLboolean previousCullFaceEnabled;
			glGetIntegerv(GL_POLYGON_MODE, previousPolygonMode);
			previousCullFaceEnabled = glIsEnabled(GL_CULL_FACE);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (previousCullFaceEnabled) {
				glDisable(GL_CULL_FACE);
			}

			if (pVolume->IsTriangleBased()) {
				glDrawElements(GL_TRIANGLES, pVolume->NumberIndices(), GL_UNSIGNED_INT, NULL);
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
			glDrawElements(GL_TRIANGLES, pVolume->NumberIndices(), GL_UNSIGNED_INT, NULL);
		}

	//Error:
		return r;
	}
};

#endif // ! OGL_QUAD_H_
