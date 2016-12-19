#include "ForceGeneratorFactory.h"

#include "GravityGenerator.h"

std::unique_ptr<ForceGenerator> ForceGeneratorFactory::MakeForceGenerator(FORCE_GENERATOR_TYPE type) {
	std::unique_ptr<ForceGenerator> pForceGenerator = nullptr;

	switch (type) {
		case FORCE_GENERATOR_GRAVITY: {
			pForceGenerator = std::unique_ptr<ForceGenerator>(new GravityGenerator());
		} break;

		default: {
			pForceGenerator = nullptr;
			DEBUG_LINEOUT("Force Generator type %d not supported!", type);
		} break;
	}

	return pForceGenerator;
}