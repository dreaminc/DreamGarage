#ifndef OBJECT_DERIVATIVE_H_
#define OBJECT_DERIVATIVE_H_

// DREAM OS
// DreamOS/Dimension/Primitives/ObjectDerivative.h
// Object Derivative Object
// Contains the derivative of a given ObjectState object

#include "point.h"
#include "vector.h"
#include "quaternion.h"

class ObjectState;

class ObjectDerivative {
	friend class VirtualObj;
	friend class ObjectState;

public:
	ObjectDerivative();

public:
	RESULT Clear();

	const vector GetRateOfChangeOrigin();
	//const vector GetRateOfChangeVelocity();
	
	const quaternion GetRateOfChangeRotation();
	const quaternion GetRateOfChangeAngularMomentun();

protected:
	vector m_vRateOfChangeOrigin;						// Rate of change of origin (velocity)
	//vector m_vRateOfChangeVelocity;					// Rate of change of Velocity			
	vector m_vForce;									// Force 
		
	quaternion m_qRateOfChangeRotation;					// Rate of change of Rotation
	quaternion m_qRateOfChangeAngularMomentum;			// Rate of change of Angular Momentum
};

#endif	// OBJECT_DERIVATIVE_H_