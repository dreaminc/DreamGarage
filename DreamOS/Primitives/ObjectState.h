#ifndef OBJECT_STATE_H_
#define OBJECT_STATE_H_

// DREAM OS
// DreamOS/Dimension/Primitives/ObjectState.h
// Object State Object
// The state object contains all parameters of the state of a given object 
// this includes position, orientation and the respective velocities and accelerations of either
// this object may change in nature and be extended over time

#include "point.h"
#include "vector.h"
#include "quaternion.h"

class ObjectDerivative;

class ObjectState {
	friend class VirtualObj;
	friend class ObjectDerivative;

public:
	enum class IntegrationType {
		EUCLID,
		RK4,
		INVALID
	};

public:
	ObjectState();
	ObjectState(point ptOrigin);

public:
	RESULT Clear();

	const point GetOrigin();
	const vector GetVelocity();
	const vector GetAcceleration();
	const quaternion GetRotation();
	const quaternion GetAngularMoment();

	ObjectDerivative Evaluate(float timeStart, float timeDelta, const ObjectDerivative &objectDerivative);
	
	template <ObjectState::IntegrationType IT>
	RESULT Integrate(float timeStart, float timeDelta);
	

protected:
	point m_ptOrigin;					// Origin			
	vector m_vVelocity;					// Velocity			
	vector m_vAcceleration;				// Acceleration		
	quaternion m_qRotation;				// Rotation
	quaternion m_qAngularMomentum;		// Angular Momentum
};

#endif	// OBJECT_STATE_H_

