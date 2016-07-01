#ifndef COMPOSITE_H_
#define COMPOSITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/composite.h
// Composite Primitive
// The composite object is a collection of other objects when these are constructed within the object itself 
// this is really a convenience object that should be used to derive others rather than off of DimObj directly

#include "DimObj.h"

#include "HAL/HALImp.h"

#include <memory>

class composite : public DimObj {
public:
	composite(std::shared_ptr<HALImp> pHALImp) :
		m_pHALImp(pHALImp)
	{
		// empty
	}

	~composite() {
		ClearObjects();
	}

	RESULT AddObject(std::shared_ptr<DimObj> pDimObj) {
		return AddChild(pDimObj);
	}

	RESULT ClearObjects() {
		return ClearChildren();
	}

	sphere* AddSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3) {
		RESULT r = R_PASS;

		sphere *pSphere = m_pHALImp->MakeSphere(radius, numAngularDivisions, numVerticalDivisions);
		CR(AddObject(std::make_shared<DimObj>(pSphere)));

	Success:
		return pSphere;

	Error:
		if (pSphere != nullptr) {
			delete pSphere;
			pSphere = nullptr;
		}

		return nullptr;
	}

	volume* AddVolume(double side) {
		RESULT r = R_PASS;

		volume *pVolume = m_pHALImp->MakeVolume(side);
		CR(AddObject(std::make_shared<DimObj>(pVolume)));

	Success:
		return pVolume;

	Error:
		if (pVolume != nullptr) {
			delete pVolume;
			pVolume = nullptr;
		}

		return nullptr;
	}

private:
	std::shared_ptr<HALImp> m_pHALImp;
};

#endif	// ! COMPOSITE_H_