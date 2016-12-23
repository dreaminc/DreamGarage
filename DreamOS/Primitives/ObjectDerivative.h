#ifndef OBJECT_DERIVATIVE_H_
#define OBJECT_DERIVATIVE_H_

// DREAM OS
// DreamOS/Dimension/Primitives/ObjectDerivative.h
// Object Derivative Object
// Contains the derivative of a given ObjectState object

#include "point.h"
#include "vector.h"
#include "quaternion.h"

class ObjectDerivative {
	friend class VirtualObj;

public:
	ObjectDerivative();

public:
	vector GetRateOfChangeOrigin();
	vector GetRateOfChangeVelocity();
	vector GetRateOfChangeAcceleration();
	quaternion GetRateOfChangeRotation();
	quaternion GetRateOfChangeAngularMomentun();

protected:
	vector m_vRateOfChangeOrigin;						// Rate of change of origin			
	vector m_vRateOfChangeVelocity;						// Rate of change of Velocity			
	vector m_vRateOfChangeAcceleration;					// Rate of change ofAcceleration		
	quaternion m_qRateOfChangeRotation;					// Rate of change of Rotation
	quaternion m_qRateOfChangeAngularMomentum;			// Rate of change of Angular Momentum
};

#endif	// OBJECT_DERIVATIVE_H_