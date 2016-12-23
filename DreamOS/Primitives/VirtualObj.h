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

#include "ObjectState.h"
#include "ObjectDerivative.h"

#include "matrix/matrix.h"
#include "matrix/RotationMatrix.h"
#include "matrix/TranslationMatrix.h"
#include "matrix/ScalingMatrix.h"

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
public:
	friend class PhysicsIntegrator;		// TODO: move physics stuff into state/derivative for RK4

protected:
	ObjectState m_objectState;
	ObjectDerivative m_objectDerivative;

	double m_kgMass;				// Mass (kg)
	vector m_vScale;				// Scale vector

	// The pivot point
	point m_ptPivot;

public:
	VirtualObj();
	VirtualObj(point ptOrigin);
	~VirtualObj();

	virtual OBJECT_TYPE GetType() {
		return OBJECT_VIRTUAL;
	}

	// State
	ObjectState GetState();
	RESULT SetState(ObjectState virtualObjState);

	// Derivative
	ObjectDerivative GetDerivative();
	RESULT SetDerivative(ObjectDerivative virtualObjDerivative);

	// Position
	virtual point GetOrigin();
	virtual point GetPosition();

	VirtualObj* translate(matrix <point_precision, 4, 1> v);
	VirtualObj* translate(point_precision x, point_precision y, point_precision z);
	VirtualObj* translateX(point_precision x);
	VirtualObj* translateY(point_precision y);
	VirtualObj* translateZ(point_precision z);

	VirtualObj* SetOrigin(point p);
	VirtualObj* SetPosition(point p);
	VirtualObj* MoveTo(point p);
	VirtualObj* MoveTo(point_precision x, point_precision y, point_precision z);

	VirtualObj* Scale(point_precision scale);

	// Pivot Point
	RESULT SetPivotPoint(point ptPivot);
	RESULT SetPivotPoint(point_precision x, point_precision y, point_precision z);

	// Rotation
	VirtualObj* RotateBy(quaternion q);
	VirtualObj* RotateBy(vector v, quaternion_precision theta);
	VirtualObj* RotateBy(quaternion_precision thetaX, quaternion_precision thetaY, quaternion_precision thetaZ);
	VirtualObj* RotateXBy(quaternion_precision deg);
	VirtualObj* RotateYBy(quaternion_precision deg);
	VirtualObj* RotateZBy(quaternion_precision deg);

	VirtualObj* RotateByDeg(quaternion_precision x, quaternion_precision y, quaternion_precision z);
	VirtualObj* RotateXByDeg(quaternion_precision theta);
	VirtualObj* RotateYByDeg(quaternion_precision theta);
	VirtualObj* RotateZByDeg(quaternion_precision theta);

	VirtualObj* SetRotate(quaternion q);
	VirtualObj* SetRotate(quaternion_precision x, quaternion_precision y, quaternion_precision z);
	VirtualObj* SetRotateDeg(quaternion_precision degX, quaternion_precision degY, quaternion_precision degZ);
	VirtualObj* ResetRotation();

	/*
	VirtualObj* SetRotateX(quaternion_precision theta);
	VirtualObj* SetRotateY(quaternion_precision theta);
	VirtualObj* SetRotateZ(quaternion_precision theta);

	VirtualObj* SetRotateXDeg(quaternion_precision deg);
	VirtualObj* SetRotateYDeg(quaternion_precision deg);
	VirtualObj* SetRotateZDeg(quaternion_precision deg);
	//*/

	VirtualObj* SetOrientation(quaternion qOrientation);
	quaternion GetOrientation();
	matrix<virtual_precision, 4, 4> GetOrientationMatrix();

	// Velocity
	VirtualObj* AddVelocity(matrix <point_precision, 4, 1> vVelocity);
	VirtualObj* AddVelocity(point_precision x, point_precision y, point_precision z);
	VirtualObj* SetVelocity(matrix <point_precision, 4, 1> vVelocity);
	VirtualObj* SetVelocity(point_precision x, point_precision y, point_precision z);

	// Acceleration
	VirtualObj* AddAcceleration(matrix <point_precision, 4, 1> vAccel);
	VirtualObj* AddAcceleration(point_precision x, point_precision y, point_precision z);
	VirtualObj* SetAcceleration(matrix <point_precision, 4, 1> vAccel);
	VirtualObj* SetAcceleration(point_precision x, point_precision y, point_precision z);

	// Angular Momentum
	VirtualObj* AddAngularMomentum(quaternion q);
	VirtualObj* SetAngularMomentum(quaternion am);

	RESULT SetMass(double kgMass);
	double GetMass();

	// Update functions
	VirtualObj* Update();
	VirtualObj* UpdatePosition();
	VirtualObj* UpdateRotation();

	// OnManipulation is called any time an object is manipulated in some way
	// since some objects may need to update certain things, like reference geometry 
	// or to test for collisions on update etc
	virtual RESULT OnManipulation();

	// Matrix Functions
	//matrix<virtual_precision, 4, 4> GetModelMatrix();
	matrix<virtual_precision, 4, 4> GetModelMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));

public:
	UID getID() { return m_uid; }

private:
	UID m_uid;
};

#endif // ! VIRTUAL_OBJ_H_