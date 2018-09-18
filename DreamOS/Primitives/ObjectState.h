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
#include "dirty.h"

#include <list>
#include <memory>

class ObjectDerivative;
class ForceGenerator;

class ObjectState : public dirty {
	friend class VirtualObj;
	friend class DimObj;
	friend class ObjectDerivative;
	friend class CollisionResolver;

public:
	enum class IntegrationType {
		EUCLID,
		RK4,
		INVALID
	};

	enum class MassDistributionType {
		VOLUME,
		SPHERE,
		CUSTOM,
		QUAD,
		PLANE,
		INVALID
	};

public:
	ObjectState(VirtualObj *pParentObj);
	ObjectState(VirtualObj *pParentObj, point ptOrigin);

public:
	RESULT Clear();
	RESULT RecalculateLinearVelocity();
	RESULT RecalculateAngularVelocity();

	RESULT AddForceGenerator(ForceGenerator *pForceGenerator);
	RESULT ClearForceGenerators();

	RESULT SetMass(double kgMass);
	const double GetMass();
	const double GetInverseMass();

	RESULT SetImmovable(bool fImmovable);
	bool IsImmovable();


	RESULT SetVelocity(vector vVelocity);
	const vector GetVelocity();
	const vector GetAngularVelocity();
	const vector GetVelocityAtPoint(point ptRefObj);
	RESULT AddMomentumImpulse(vector vImplulse);
	
	RESULT AddTorqueImpulse(vector vTorque);
	RESULT ApplyForceAtPoint(vector vForce, point ptRefObj, double msDeltaTime);

	RESULT SetRotationalVelocity(vector vRotationalVelocity);
	vector GetRotationalVelocity();
	
	// These functions are meant to represent a moment in a simulation
	// to avoid changing velocity during a particular point of a simulation
	RESULT AddPendingMomentumImpulse(vector vImplulse);
	RESULT CommitPendingMomentum();

	RESULT AddPendingTorque(vector vTorque);
	RESULT CommitPendingTorque();

	RESULT AddPendingTranslation(vector vTranslation);
	RESULT CommitPendingTranslation();

	RESULT SetMassDistributionType(ObjectState::MassDistributionType type);
	RESULT RecalculateInertialTensor();
	RESULT SetInertiaTensorSphere(point_precision radius);
	RESULT SetInertiaTensorVolume(point_precision width, point_precision height, point_precision depth);
	RESULT SetInertiaTensorQuad(point_precision width, point_precision height);
	RESULT SetInertiaTensor(MassDistributionType type, const matrix<point_precision, 3, 3> &matInertiaTensor);

	const vector GetMomentum();

	const point GetOrigin();
	RESULT Translate(vector vTranslation);
	RESULT translate(vector v);

	const quaternion GetRotation();
	const vector GetAngularMomentum();

	ObjectDerivative Evaluate(float timeStart, float timeDelta, const ObjectDerivative &objectDerivative, const std::list<ForceGenerator*> &externalForceGenerators);
	
	template <ObjectState::IntegrationType IT>
	RESULT Integrate(float timeStart, float timeDelta, const std::list<ForceGenerator*> &externalForceGenerators);
	
private:
	double m_kgMass;				// Mass (kg)
	double m_inverseMass;			// Inverse mass (1/kg)
	bool m_fImmovable = false;		// Immovable means the object will not respond to forces of any kind (effectively infinite mass, and no volume)

	vector m_vVelocity;					// Velocity			

	std::list<ForceGenerator*> m_forceGenerators;

	std::list<vector> m_pendingTorqueVectors;
	std::list<vector> m_pendingMomentumVectors;
	std::list<vector> m_pendingTranslationVectors;

protected:
	point m_ptOrigin;					// Origin			
	vector m_vMomentum;					// Momentum

	point m_ptCenterOfMass;									// Center of Mass
	matrix<point_precision, 4, 4> m_matInertiaTensor;				// Inertia tensor
	matrix<point_precision, 4, 4> m_matInverseInertiaTensor;		// Inverse inertia tensor
	MassDistributionType m_massDistributionType;			// Mass distribution type

	double m_angularDamping = 0.0f;							// Angular Damping
	vector m_vTorque;										// Torque
	vector m_vAngularVelocity;								// Angular Velocity
	vector m_vAngularMomentum;							// Angular Momentum

	quaternion m_qSpin;										// Spin Quantity

	quaternion m_qRotation;									// Rotation
	quaternion m_qOrientationOffset;						// A base rotation applied as an offset

private:
	VirtualObj *m_pParentObj = nullptr;						// This as a reference to parent
};

#endif	// OBJECT_STATE_H_

