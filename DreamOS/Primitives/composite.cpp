#include "composite.h"

#include "HAL/HALImp.h"

composite::composite(HALImp *pHALImp) :
	m_pHALImp(pHALImp)
{
	// empty
}

/*
~composite() {
ClearObjects();
}
*/

RESULT composite::Allocate() {
	return R_NOT_IMPLEMENTED;
}

inline int composite::NumberVertices() {
	return 0;
}

inline int composite::NumberIndices() {
	return 0;
}

RESULT composite::AddObject(std::shared_ptr<DimObj> pDimObj) {
	return AddChild(pDimObj);
}

RESULT composite::ClearObjects() {
	return ClearChildren();
}

std::shared_ptr<sphere> composite::MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3) {
	RESULT r = R_PASS;

	std::shared_ptr<sphere> pSphere(m_pHALImp->MakeSphere(radius, numAngularDivisions, numVerticalDivisions));
	
Success:
	return pSphere;

Error:
	return nullptr;
}

std::shared_ptr<sphere> composite::AddSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3) {
	RESULT r = R_PASS;

	std::shared_ptr<sphere> pSphere = MakeSphere(radius, numAngularDivisions, numVerticalDivisions);
	CR(AddObject(pSphere));

Success:
	return pSphere;

Error:
	return nullptr;
}

std::shared_ptr<volume> composite::MakeVolume(double width, double length, double height) {
	RESULT r = R_PASS;

	std::shared_ptr<volume> pVolume(m_pHALImp->MakeVolume(width, length, height));

Success:
	return pVolume;

Error:
	return nullptr;
}

std::shared_ptr<volume> composite::MakeVolume(double side) {
	return MakeVolume(side, side, side);
}

std::shared_ptr<volume> composite::AddVolume(double width, double length, double height) {
	RESULT r = R_PASS;

	std::shared_ptr<volume> pVolume = MakeVolume(width, length, height);
	CR(AddObject(pVolume));

Success:
	return pVolume;

Error:
	return nullptr;
}

std::shared_ptr<volume> composite::AddVolume(double side) {
	return AddVolume(side, side, side);
}