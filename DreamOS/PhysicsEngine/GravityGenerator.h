#ifndef GRAVITY_GENERATOR_H_
#define GRAVITY_GENERATOR_H_

#include "RESULT/EHM.h"

// Dream Force Generator
// The virtual class defining a force generator

#include "ForceGenerator.h"

#define DEFAULT_GRAVITY_ACCEL -9.8f

class GravityGenerator : public ForceGenerator {
	friend class ForceGeneratorFactory;

protected:
	GravityGenerator();

public:
	~GravityGenerator();

protected:
	virtual RESULT Update() override;
};

#endif // ! GRAVITY_GENERATOR_H_