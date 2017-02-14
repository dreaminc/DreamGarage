#include "ForceGeneratorFactory.h"

#include "GravityGenerator.h"

ForceGenerator* ForceGeneratorFactory::MakeForceGenerator(FORCE_GENERATOR_TYPE type) {
	ForceGenerator* pForceGenerator = nullptr;

	switch (type) {
		case FORCE_GENERATOR_GRAVITY: {
			pForceGenerator = new GravityGenerator();
		} break;

		default: {
			pForceGenerator = nullptr;
			DEBUG_LINEOUT("Force Generator type %d not supported!", type);
		} break;
	}

	return pForceGenerator;
}