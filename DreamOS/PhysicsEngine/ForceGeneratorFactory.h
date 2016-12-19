#ifndef FORCE_GENERATOR_FACTORY_H_
#define FORCE_GENERATOR_FACTORY_H_

// DREAM OS
// DreamOS/PhysicsEngine/ForceGeneratorFactory.h
// Dream Force Generator Factory

#include "ForceGenerator.h"
#include <memory>

typedef enum {
	FORCE_GENERATOR_GRAVITY,
	FORCE_GENERATOR_INVALID
} FORCE_GENERATOR_TYPE;

class ForceGeneratorFactory {
public:
	static std::unique_ptr<ForceGenerator> MakeForceGenerator(FORCE_GENERATOR_TYPE type);
};

#endif // ! FORCE_GENERATOR_FACTORY_H_