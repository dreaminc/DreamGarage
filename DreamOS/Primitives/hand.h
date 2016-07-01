#ifndef HAND_H_
#define HAND_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/composite.h
// Composite Primitive
// The composite object is a collection of other objects when these are constructed within the object itself 
// this is really a convenience object that should be used to derive others rather than off of DimObj directly

#include "composite.h"

typedef enum HandType {
	HAND_LEFT,
	HAND_RIGHT,
	HAND_INVALID
} HAND_TYPE;

/*
class thumb : public composite {
public:
	thumb(std::shared_ptr<HALImp> pHALImp) :
		composite(pHALImp)
	{
		// empty
	}

	RESULT Initialize() {
		RESULT r = R_PASS;

		

	Error:
		return r;
	}

private:
	volume *m_pProximalPhalanx;
	volume *m_pIntermediatePhalanx;
	volume *m_pDistalPhalanx;
};

class finger : public composite {
public:
	finger(std::shared_ptr<HALImp> pHALImp) :
		composite(pHALImp)
	{
		// empty
	}

	RESULT Initialize() {
		RESULT r = R_PASS;

		

	Error:
		return r;
	}

private:
	volume *m_pMetacarpal;
	volume *m_pProximalPhalanx;
	volume *m_pIntermediatePhalanx;
	volume *m_pDistalPhalanx;
};
*/

class hand : public composite {
public:
	hand(std::shared_ptr<HALImp> pHALImp) :
		composite(pHALImp)
	{
		// empty
	}

	RESULT Initialize() {
		RESULT r = R_PASS;

		m_pPalm = AddVolume(1.0f);

	Error:
		return r;
	}

private:
	volume *m_pPalm;
};

#endif	// ! HAND_H_