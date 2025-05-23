#ifndef VIRTUAL_OBJ_H_
#define VIRTUAL_OBJ_H_

// Dream Core
// dos/src/core/primitives/VirtualObj.h

// Virtual Base Object
// Virtual objects are those that do not have any actual dimensional data.  
// For example, lights and cameras may have bounds or otherwise, but no vertices 

#include "core/types/DObject.h"

#include "point.h"
#include "vector.h"
#include "core/types/UID.h"
#include "quaternion.h"

#include "ObjectState.h"
#include "ObjectDerivative.h"

#include "core/matrix/matrix.h"
#include "core/matrix/RotationMatrix.h"
#include "core/matrix/TranslationMatrix.h"
#include "core/matrix/ScalingMatrix.h"

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

class ray;

class VirtualObj : 
	public DObject 
{
public:
	friend class PhysicsIntegrator;		// TODO: move physics stuff into state/derivative for RK4

protected:
	ObjectState m_objectState;
	ObjectDerivative m_objectDerivative;


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

	template <ObjectState::IntegrationType IT>
	RESULT IntegrateState(float timeStart, float timeDelta, const std::list<ForceGenerator*> &externalForceGenerators);

	// Visibility
	virtual bool IsVisible() {
		return false;
	}

	// Position
	virtual point GetOrigin(bool fAbsolute = false);
	virtual point GetPosition(bool fAbsolute = false);

	VirtualObj* translate(matrix <point_precision, 4, 1> v);
	VirtualObj* translate(point_precision x, point_precision y, point_precision z);
	VirtualObj* translateX(point_precision x);
	VirtualObj* translateY(point_precision y);
	VirtualObj* translateZ(point_precision z);

	VirtualObj* SetOrigin(point p);
	VirtualObj* SetPosition(point p);
	VirtualObj* SetPosition(point_precision x, point_precision y, point_precision z);
	VirtualObj* MoveTo(point p);
	VirtualObj* MoveTo(point_precision x, point_precision y, point_precision z);

	VirtualObj* SetScale(vector vScale);
	virtual vector GetScale(bool fAbsolute = false);
	VirtualObj* Scale(point_precision scale);
	VirtualObj* ScaleX(point_precision scale);
	VirtualObj* ScaleY(point_precision scale);
	VirtualObj* ScaleZ(point_precision scale);

	// Pivot Point
	point GetPivotPoint();
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

	VirtualObj* ResetOrientationOffset();
	VirtualObj* SetOrientationOffset(quaternion q);
	VirtualObj* SetOrientationOffset(vector vEulerRotationVector);
	VirtualObj* SetOrientationOffset(quaternion_precision thetaX, quaternion_precision thetaY, quaternion_precision thetaZ);
	VirtualObj* SetOrientationOffsetDeg(quaternion_precision thetaXdeg, quaternion_precision thetaYdeg, quaternion_precision thetaZdeg);

	/*
	VirtualObj* SetRotateX(quaternion_precision theta);
	VirtualObj* SetRotateY(quaternion_precision theta);
	VirtualObj* SetRotateZ(quaternion_precision theta);

	VirtualObj* SetRotateXDeg(quaternion_precision deg);
	VirtualObj* SetRotateYDeg(quaternion_precision deg);
	VirtualObj* SetRotateZDeg(quaternion_precision deg);
	//*/

	VirtualObj* SetOrientation(quaternion qOrientation);
	virtual quaternion GetOrientation(bool fAbsolute = false);
	matrix<virtual_precision, 4, 4> GetOrientationMatrix();

	// Velocity
	VirtualObj* AddVelocity(matrix <point_precision, 4, 1> vVelocity);
	VirtualObj* AddVelocity(point_precision x, point_precision y, point_precision z);
	VirtualObj* SetVelocity(matrix <point_precision, 4, 1> vVelocity);
	VirtualObj* SetVelocity(point_precision x, point_precision y, point_precision z);

	// Ray - objects can override this if they'd like to
	virtual ray GetRay(bool fAbsolute = false);

	// Acceleration
	/*
	VirtualObj* AddAcceleration(matrix <point_precision, 4, 1> vAccel);
	VirtualObj* AddAcceleration(point_precision x, point_precision y, point_precision z);
	VirtualObj* SetAcceleration(matrix <point_precision, 4, 1> vAccel);
	VirtualObj* SetAcceleration(point_precision x, point_precision y, point_precision z);
	*/

	// Angular Momentum
	VirtualObj* AddAngularMomentum(vector vAngularMomentum);
	VirtualObj* SetAngularMomentum(vector vAngularMomentum);

	VirtualObj* ApplyTorqueImpulse(vector vTorque);
	VirtualObj* ApplyForceAtPoint(vector vForce, point ptRefObj, double msDeltaTime);

	RESULT SetMass(double kgMass);
	virtual double GetMass();
	virtual double GetInverseMass();
	RESULT SetImmovable(bool fImmovable);
	bool IsImmovable();

	VirtualObj* SetRotationalVelocity(vector vRotationalVelocity);
	vector GetRotationalVelocity();

	vector GetVelocity();
	vector GetVelocityOfPoint(point pt);
	vector GetAngularVelocity();
	point GetPointRefCenterOfMass(point pt);
	vector GetMomentum();
	RESULT Impulse(vector vImpulse);
	RESULT AddPendingImpulse(vector vImpulse);
	RESULT CommitPendingImpulses();
	RESULT AddPendingTranslation(vector vTranslation);
	RESULT CommitPendingTranslation();

	// OnManipulation is called any time an object is manipulated in some way
	// since some objects may need to update certain things, like reference geometry 
	// or to test for collisions on update etc
	virtual RESULT OnManipulation();

	// Matrix Functions
	//matrix<virtual_precision, 4, 4> GetModelMatrix();
	matrix<virtual_precision, 4, 4> GetRotationMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));
	matrix<virtual_precision, 4, 4> GetTranslationMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));
	matrix<virtual_precision, 4, 4> GetModelMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));
};

#endif // ! VIRTUAL_OBJ_H_