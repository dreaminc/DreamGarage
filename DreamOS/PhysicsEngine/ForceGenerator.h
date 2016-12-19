#ifndef FORCE_GENERATOR_H_
#define FORCE_GENERATOR_H_

#include "RESULT/EHM.h"

// Dream Force Generator
// The virtual class defining a force generator

#include "Primitives/Types/UID.h"

class ForceGenerator {
	friend class PhysicsIntegrator;
	friend class ForceGeneratorFactory;

protected:
	virtual RESULT Update() = 0;

private:
	UID m_uid;
};

#endif // ! FORCE_GENERATOR_H_