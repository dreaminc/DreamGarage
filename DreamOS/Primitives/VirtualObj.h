#ifndef VIRTUAL_OBJ_H_
#define VIRTUAL_OBJ_H_

// DREAM OS
// DreamOS/Dimension/Primitives/VirtualObj.h
// Virtual Base Object
// Virtual objects are those that do not have any actual dimensional data.  
// For example, lights and cameras may have bounds or otherwise, but no vertices 

#include "valid.h"
#include "point.h"
#include "vector.h"
#include "Primitives/Types/UID.h"
#include "quaternion.h"

#include "matrix.h"
#include "RotationMatrix.h"
#include "TranslationMatrix.h"
#include "ScalingMatrix.h"

#ifdef FLOAT_PRECISION
	typedef float virtual_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double virtual_precision;
#endif

typedef enum ObjectType {
	OBJECT_VIRTUAL,
	OBJECT_DIMENSION,
	OBJECT_INVALID
} OBJECT_TYPE;

class VirtualObj : public valid {
protected:
	point m_ptOrigin;			// Origin
	vector m_vVelocity;			// Velocity

	quaternion m_qRotation;				// Rotation
	quaternion m_qAngularMomentum;		// Angular Momentum

public:
	VirtualObj();
	VirtualObj(point ptOrigin);
	~VirtualObj();

	virtual OBJECT_TYPE GetType() {
		return OBJECT_VIRTUAL;
	}

	// Position
	point GetOrigin();

	VirtualObj translate(matrix <point_precision, 4, 1> v);
	VirtualObj translate(point_precision x, point_precision y, point_precision z);
	VirtualObj translateX(point_precision x);
	VirtualObj translateY(point_precision y);
	VirtualObj translateZ(point_precision z);

	VirtualObj MoveTo(point p);
	VirtualObj MoveTo(point_precision x, point_precision y, point_precision z);

	// Velocity
	VirtualObj AddVelocity(matrix <point_precision, 4, 1> v);
	VirtualObj AddVelocity(point_precision x, point_precision y, point_precision z);
	VirtualObj SetVelocity(matrix <point_precision, 4, 1> v);
	VirtualObj SetVelocity(point_precision x, point_precision y, point_precision z);

	// Rotation
	VirtualObj RotateBy(quaternion q);
	VirtualObj RotateBy(quaternion_precision x, quaternion_precision y, quaternion_precision z);
	VirtualObj RotateXBy(quaternion_precision theta);
	VirtualObj RotateYBy(quaternion_precision theta);
	VirtualObj RotateZBy(quaternion_precision theta);

	VirtualObj SetRotate(quaternion q);
	VirtualObj SetRotate(quaternion_precision x, quaternion_precision y, quaternion_precision z);
	VirtualObj SetRotateX(quaternion_precision theta);
	VirtualObj SetRotateY(quaternion_precision theta);
	VirtualObj SetRotateZ(quaternion_precision theta);

	VirtualObj SetOrientation(quaternion qOrientation);
	quaternion GetOrientation();
	matrix<virtual_precision, 4, 4> GetOrientationMatrix();

	// Angular Momentum
	VirtualObj AddAngularMomentum(quaternion q);
	VirtualObj SetAngularMomentum(quaternion am);

	// Update functions
	VirtualObj Update();
	VirtualObj UpdatePosition();
	VirtualObj UpdateRotation();

	// Matrix Functions
	//matrix<virtual_precision, 4, 4> GetModelMatrix();
	matrix<virtual_precision, 4, 4> GetModelMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));

public:
	UID getID() { return m_uid; }

private:
	UID m_uid;
};

#endif // ! VIRTUAL_OBJ_H_