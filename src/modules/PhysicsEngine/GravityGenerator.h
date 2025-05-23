#ifndef GRAVITY_GENERATOR_H_
#define GRAVITY_GENERATOR_H_

#include "core/ehm/EHM.h"

// Dream Physics Engine
// dos/src/modules/PhysicsEngine/GravityGenerator.h

#include "ForceGenerator.h"

#define DEFAULT_GRAVITY_ACCEL -9.8f

class GravityGenerator : public ForceGenerator {
	friend class ForceGeneratorFactory;

public:
	GravityGenerator() {
		// empty
	}

	~GravityGenerator() {
		// empty
	}

	double GetGravityAcceleration();
	RESULT SetGravityAcceleration(double gravityAcceleration);

protected:
	virtual RESULT Update() override;

public:
	virtual vector GenerateForce(ObjectState *pObjectState, double startTime, double deltaTime) override;

private:
	double m_gravityAcceleration = DEFAULT_GRAVITY_ACCEL;
};

#endif // ! GRAVITY_GENERATOR_H_