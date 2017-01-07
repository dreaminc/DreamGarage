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

#include <list>
#include <memory>

class ObjectDerivative;
class ForceGenerator;

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
	RESULT Recalculate();

	RESULT AddForceGenerator(ForceGenerator *pForceGenerator);
	RESULT ClearForceGenerators();

	RESULT SetMass(double kgMass);
	const double GetMass();
	const double GetInverseMass();

	RESULT SetVelocity(vector vVelocity);
	const vector GetVelocity();
	RESULT AddMomentumImpulse(vector vImplulse);

	const vector GetMomentum();

	const point GetOrigin();

	const quaternion GetRotation();
	const quaternion GetAngularMoment();

	ObjectDerivative Evaluate(float timeStart, float timeDelta, const ObjectDerivative &objectDerivative, const std::list<ForceGenerator*> &externalForceGenerators);
	
	template <ObjectState::IntegrationType IT>
	RESULT Integrate(float timeStart, float timeDelta, const std::list<ForceGenerator*> &externalForceGenerators);
	
private:
	double m_kgMass;				// Mass (kg)
	double m_inverseMass;			// Inverse mass (1/kg)

	vector m_vVelocity;					// Velocity			

	std::list<ForceGenerator*> m_forceGenerators;

protected:
	point m_ptOrigin;					// Origin			
	vector m_vMomentum;					// Momentum

	quaternion m_qRotation;				// Rotation
	quaternion m_qAngularMomentum;		// Angular Momentum
};

#endif	// OBJECT_STATE_H_

