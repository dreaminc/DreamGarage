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

class ObjectState {
	friend class VirtualObj;

public:
	ObjectState();
	ObjectState(point ptOrigin);

public:
	point GetOrigin();
	vector GetVelocity();
	vector GetAcceleration();
	quaternion GetRotation();
	quaternion GetAngularMoment();

protected:
	point m_ptOrigin;					// Origin			
	vector m_vVelocity;					// Velocity			
	vector m_vAcceleration;				// Acceleration		
	quaternion m_qRotation;				// Rotation
	quaternion m_qAngularMomentum;		// Angular Momentum
};

#endif	// OBJECT_STATE_H_

