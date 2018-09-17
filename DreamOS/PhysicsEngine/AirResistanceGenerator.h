#ifndef AIR_RESISTANCE_GENERATOR_H_
#define AIR_RESISTANCE_GENERATOR_H_

#include "RESULT/EHM.h"

// Dream Force Generator
// The virtual class defining a force generator

#include "ForceGenerator.h"

#define DEFAULT_AIR_DENSITY 100.225f			// approximately 1.225 kg/m³ at sea level
#define DEFAULT_AIR_DRAG_COEFFICIENT 0.47f	// sphere
#define DEFAULT_AIR_AREA 1.0f				// because I don't wanna deal with this
#define DEFAULT_AIR_MIN_DRAG_TERM 0.001f	// so that stopping works

class AirResistanceGenerator : public ForceGenerator {
	friend class ForceGeneratorFactory;

public:
	AirResistanceGenerator() {
		// empty
	}

	~AirResistanceGenerator() {
		// empty
	}

protected:
	virtual RESULT Update() override;

public:
	virtual vector GenerateForce(ObjectState *pObjectState, double startTime, double deltaTime) override;

private:
	double m_kConstant = (DEFAULT_AIR_DENSITY * DEFAULT_AIR_DRAG_COEFFICIENT * DEFAULT_AIR_AREA) / 2;
	double m_minConstant = DEFAULT_AIR_MIN_DRAG_TERM;
	/*
	double m_airDensity = DEFAULT_AIR_DENSITY;
	double m_airDrag = DEFAULT_AIR_DRAG_COEFFICIENT;
	double m_airArea = DEFAULT_AIR_AREA;
	*/
};

#endif // ! AIR_RESISTANCE_GENERATOR_H_
