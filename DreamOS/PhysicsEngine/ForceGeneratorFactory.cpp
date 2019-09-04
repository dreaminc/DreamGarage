#include "ForceGeneratorFactory.h"

#include "GravityGenerator.h"
#include "AirResistanceGenerator.h"

ForceGenerator* ForceGeneratorFactory::MakeForceGenerator(FORCE_GENERATOR_TYPE type) {
	ForceGenerator* pForceGenerator = nullptr;

	switch (type) {
		case FORCE_GENERATOR_GRAVITY: {
			pForceGenerator = new GravityGenerator();
		} break;

		case FORCE_GENERATOR_AIR_RESISTANCE: {
			pForceGenerator = new AirResistanceGenerator();
		} break;

		default: {
			pForceGenerator = nullptr;
			DEBUG_LINEOUT("Force Generator type %d not supported!", type);
		} break;
	}

	return pForceGenerator;
}