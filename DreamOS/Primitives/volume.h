#ifndef VOLUME_H_
#define VOLUME_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Volume.h
// Volume Primitive
// Center is assumed as (0, 0, 0) so points are adjusted positive / negative as needed

#define NUM_VOLUME_FACES 6
#define NUM_VOLUME_QUADS 6
#define NUM_FACE_POINTS 4
#define NUM_VOLUME_POINTS (NUM_FACE_POINTS * NUM_VOLUME_FACES)
#define NUM_VOLUME_TRIS (NUM_VOLUME_FACES * 2)
#define NUM_VOLUME_TRI_INDICES (NUM_VOLUME_TRIS * 3)
#define NUM_VOLUME_QUAD_INDICES (NUM_VOLUME_QUADS * 4)

#include "DimObj.h"

#include "Vertex.h"
#include "point.h"
#include "color.h"
#include "BoundingBox.h"

class volume : public DimObj {
public:
	typedef enum {
		CUBE,
		RECTANGULAR_CUBOID,
		QUADRILATERAL_FRUSTRUM,
		PARALLELEPIPED,
		INVALID
	} VOLUME_TYPE;

public:
	volume(double width, double length, double height, bool fTriangleBased = true);
	volume(double side, bool fTriangleBased = true);
	volume(BoundingBox* pBoundingBox, bool fTriangleBased = true);

	RESULT UpdateFromBoundingBox(BoundingBox* pBoundingBox, bool fTriangleBased = true);
	RESULT SetVolumeVertices(BoundingBox* pBoundingBox, bool fTriangleBased);

	virtual RESULT Allocate() override;
	virtual unsigned int NumberVertices() override;
	virtual unsigned int NumberIndices() override;

	RESULT SetVolumeVertices(double width, double length, double height, bool fTriangleBased = true, point ptOrigin = point(0.0f, 0.0f, 0.0f));
	bool IsTriangleBased();

private:
	VOLUME_TYPE m_volumeType;
	bool m_fTriangleBased;
};

#endif // !VOLUME_H_