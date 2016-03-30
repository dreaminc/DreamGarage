#ifndef DIM_OBJ_H_
#define DIM_OBJ_H_

// DREAM OS
// DreamOS/Dimension/Primitives/DimObj.h
// Dimension Base Object
// All objects in Dimension should derive from this base class

#include "valid.h"
#include "Primitives/Types/UID.h"

#include "VirtualObj.h"
#include "point.h"
#include "TriangleIndexGroup.h"
#include "Vertex.h"
#include "TimeObj.h"

class DimObj : public VirtualObj, public TimeObject, public Subscriber<TimeEvent> {
protected:
    //point m_ptOrigin;   // origin > now in virtual object
    //AABV m_aabv;        // Axis Aligned Bounding Volume

protected:
	vertex *m_pVertices;
	dimindex *m_pIndices;

public:
    DimObj() :
        VirtualObj(),	// velocity, origin
		m_pVertices(NULL),
		m_pIndices(NULL)
        //m_aabv()
    {
        /* stub */
    }

    ~DimObj() {
		if (m_pIndices != NULL) {
			delete[] m_pIndices;
			m_pIndices = NULL;
		}

		if (m_pVertices != NULL) {
			delete[] m_pVertices;
			m_pVertices = NULL;
		}
    }

	
	virtual RESULT Allocate() = 0;

	virtual inline int NumberVertices() = 0;
	inline vertex *VertexData() {
		return m_pVertices;
	}

	inline int VertexDataSize() {
		return NumberVertices() * sizeof(vertex);
	}

	virtual inline int NumberIndices() = 0;
	inline dimindex *IndexData() {
		return m_pIndices;
	}

	inline int IndexDataSize() {
		return NumberIndices() * sizeof(dimindex);
	}

	RESULT AllocateVertices(uint32_t numVerts) {
		RESULT r = R_PASS;

		m_pVertices = new vertex[numVerts];
		CN(m_pVertices);

	Error:
		return r;
	}

	RESULT AllocateIndices(uint32_t numIndices) {
		RESULT r = R_PASS;

		m_pIndices = new uint32_t[numIndices];
		CN(m_pIndices);

	Error:
		return r;
	}

	RESULT AllocateTriangleIndexGroups(uint32_t numTriangles) {
		RESULT r = R_PASS;

		m_pIndices = (dimindex*)(new TriangleIndexGroup[numTriangles]);
		CN(m_pIndices);

	Error:
		return r;
	}

	RESULT SetColor(color c) {
		for (int i = 0; i < NumberVertices(); i++)
			m_pVertices[i].SetColor(c);

		return R_PASS;
	}

	RESULT SetRandomColor() {
		for (int i = 0; i < NumberVertices(); i++)
			m_pVertices[i].SetRandomColor();

		return R_PASS;
	}

	RESULT CopyVertices(vertex pVerts[], int pVerts_n) {
		RESULT r = R_PASS;

		CBM((pVerts_n == NumberVertices()), "Cannot copy %d verts into DimObj with %s verts", pVerts_n, NumberVertices());
		for (int i = 0; i < pVerts_n; i++)
			m_pVertices[i].SetVertex(pVerts[i]);

	Error:
		return r;
	}
	
	RESULT Notify(TimeEvent *event) {
		quaternion_precision factor = 0.05;
		quaternion_precision filter = 0.1;

		static quaternion_precision x = 1.0;
		static quaternion_precision y = 1.0;
		static quaternion_precision z = 1.0;

		//x = ((1.0f - filter) * x) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));
		//y = ((1.0f - filter) * y) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));
		//z = ((1.0f - filter) * z) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));

		RotateBy(x * factor, y * factor, z * factor);

		return R_PASS;
	}

	void onTimeUpdate(double currentTime, double deltaTime) {
		//printf("nir\n");
		quaternion_precision factor = 0.05;
		quaternion_precision filter = 0.1;

		static quaternion_precision x = 1.0;
		static quaternion_precision y = 1.0;
		static quaternion_precision z = 1.0;

		//x = ((1.0f - filter) * x) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));
		//y = ((1.0f - filter) * y) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));
		//z = ((1.0f - filter) * z) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));

		RotateBy(x * factor, y * factor, z * factor);

	}

public:
	UID getID() { return m_uid; }

private:
	UID m_uid;
};

#endif // !DIM_OBJ_H_
