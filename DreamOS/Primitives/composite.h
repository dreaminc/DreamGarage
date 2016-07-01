#ifndef COMPOSITE_H_
#define COMPOSITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/composite.h
// Composite Primitive
// The composite object is a collection of other objects when these are constructed within the object itself 
// this is really a convenience object that should be used to derive others rather than off of DimObj directly

#include "DimObj.h"
#include <memory>

class HALImp;

#include "Primitives/sphere.h"
#include "Primitives/volume.h"

class composite : public DimObj {
public:
	composite(HALImp *pHALImp);

	// DimObj Interface
	RESULT Allocate();
	inline int NumberVertices();
	inline int NumberIndices();

	RESULT AddObject(std::shared_ptr<DimObj> pDimObj);
	RESULT ClearObjects();

	std::shared_ptr<sphere> MakeSphere(float radius, int numAngularDivisions, int numVerticalDivisions);
	std::shared_ptr<sphere> AddSphere(float radius, int numAngularDivisions, int numVerticalDivisions);

	std::shared_ptr<volume> MakeVolume(double width, double length, double height);
	std::shared_ptr<volume> AddVolume(double width, double length, double height);
	std::shared_ptr<volume> MakeVolume(double side);
	std::shared_ptr<volume> AddVolume(double side);



protected:
	HALImp *m_pHALImp;
};

#endif	// ! COMPOSITE_H_