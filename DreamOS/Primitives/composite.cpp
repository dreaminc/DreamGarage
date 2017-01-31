#include "composite.h"

#include "HAL/HALImp.h"

#include "Primitives/hand.h"

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

inline unsigned int composite::NumberVertices() {
	return 0;
}

inline unsigned int composite::NumberIndices() {
	return 0;
}

RESULT composite::AddObject(std::shared_ptr<DimObj> pDimObj) {
	RESULT r = R_PASS;

	CR(AddChild(pDimObj));

	if (m_pBoundingVolume != nullptr) {
		UpdateBoundingVolume();
	}

Error:
	return r;
}

RESULT composite::ClearObjects() {
	return ClearChildren();
}

RESULT composite::UpdateBoundingVolume() {
	RESULT r = R_PASS;

	point ptMax, ptMin, ptMid, ptMinTemp, ptMaxTemp;

	CN(m_pBoundingVolume);

	if (HasChildren()) {
		for (auto &childObj : GetChildren()) {
			std::shared_ptr<DimObj> pDimObj = std::dynamic_pointer_cast<DimObj>(childObj);
			if (pDimObj != nullptr) {
				ptMaxTemp = pDimObj->GetBoundingVolume()->GetMaxPoint();
				ptMinTemp = pDimObj->GetBoundingVolume()->GetMinPoint();

				// X
				if (ptMaxTemp.x() > ptMax.x())
					ptMax.x() = ptMaxTemp.x();
				else if (ptMinTemp.x() < ptMin.x())
					ptMin.x() = ptMinTemp.x();

				// Y
				if (ptMaxTemp.y() > ptMax.y())
					ptMax.y() = ptMaxTemp.y();
				else if (ptMinTemp.y() < ptMin.y())
					ptMin.y() = ptMinTemp.y();

				// Z
				if (ptMaxTemp.z() > ptMax.z())
					ptMax.z() = ptMaxTemp.z();
				else if (ptMinTemp.z() < ptMin.z())
					ptMin.z() = ptMinTemp.z();
			}
		}

		CR(m_pBoundingVolume->UpdateBoundingVolume(ptMid, ptMax));
	}

Error:
	return r;
}


std::shared_ptr<texture> composite::MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) {

	RESULT r = R_PASS;

	std::shared_ptr<texture> pTexture(m_pHALImp->MakeTexture(pszFilename, type));

	//Success:
	return pTexture;

	//Error:
	return nullptr;
}

std::shared_ptr<hand> composite::MakeHand() {
	RESULT r = R_PASS;

	std::shared_ptr<hand> pHand(m_pHALImp->MakeHand());

	//Success:
	return pHand;

	//Error:
	return nullptr;
}

std::shared_ptr<hand> composite::AddHand() {
	RESULT r = R_PASS;

	std::shared_ptr<hand> pHand = MakeHand();
	CR(AddObject(pHand));

	//Success:
	return pHand;

Error:
	return nullptr;
}

std::shared_ptr<composite> composite::MakeModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, vector vEulerRotation) {
	RESULT r = R_PASS;

	std::shared_ptr<composite> pModel(m_pHALImp->MakeModel(wstrOBJFilename, pTexture, ptPosition, scale, vEulerRotation));

	//Success:
	return pModel;

//Error:
	return nullptr;
}

std::shared_ptr<composite> composite::AddModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, vector vEulerRotation) {
	RESULT r = R_PASS;

	std::shared_ptr<composite> pModel = MakeModel(wstrOBJFilename, pTexture, ptPosition, scale, vEulerRotation);
	CR(AddObject(pModel));

	//Success:
	return pModel;

Error:
	return nullptr;
}

std::shared_ptr<composite> composite::MakeComposite() {
	RESULT r = R_PASS;

	std::shared_ptr<composite> pComposite(m_pHALImp->MakeComposite());

	//Success:
	return pComposite;

	//Error:
	return nullptr;
}

std::shared_ptr<composite> composite::AddComposite() {
	RESULT r = R_PASS;

	std::shared_ptr<composite> pComposite = MakeComposite();
	CR(AddObject(pComposite));

	//Success:
	return pComposite;

Error:
	return nullptr;
}

std::shared_ptr<sphere> composite::MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3) {
	RESULT r = R_PASS;

	std::shared_ptr<sphere> pSphere(m_pHALImp->MakeSphere(radius, numAngularDivisions, numVerticalDivisions));
	
//Success:
	return pSphere;

//Error:
	return nullptr;
}

std::shared_ptr<sphere> composite::AddSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3) {
	RESULT r = R_PASS;

	std::shared_ptr<sphere> pSphere = MakeSphere(radius, numAngularDivisions, numVerticalDivisions);
	CR(AddObject(pSphere));

//Success:
	return pSphere;

Error:
	return nullptr;
}

std::shared_ptr<volume> composite::MakeVolume(double width, double length, double height) {
	RESULT r = R_PASS;

	std::shared_ptr<volume> pVolume(m_pHALImp->MakeVolume(width, length, height));

//Success:
	return pVolume;

//Error:
	return nullptr;
}

std::shared_ptr<volume> composite::MakeVolume(double side) {
	return MakeVolume(side, side, side);
}

std::shared_ptr<volume> composite::AddVolume(double width, double length, double height) {
	RESULT r = R_PASS;

	std::shared_ptr<volume> pVolume = MakeVolume(width, length, height);
	CR(AddObject(pVolume));

//Success:
	return pVolume;

Error:
	return nullptr;
}

std::shared_ptr<volume> composite::AddVolume(double side) {
	return AddVolume(side, side, side);
}

std::shared_ptr<quad> composite::MakeQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture * pTextureHeight) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad(m_pHALImp->MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight));
	CR(AddObject(pQuad));

//Success:
	return pQuad;

Error:
	return nullptr;
}

std::shared_ptr<quad> composite::AddQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture * pTextureHeight)
{
	return MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight);
}

std::shared_ptr<DimRay> composite::MakeRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	RESULT r = R_PASS;

	std::shared_ptr<DimRay> pRay(m_pHALImp->MakeRay(ptOrigin, vDirection, step, fDirectional));

	return pRay;
}

std::shared_ptr<DimRay> composite::AddRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	RESULT r = R_PASS;

	std::shared_ptr<DimRay> pRay(m_pHALImp->MakeRay(ptOrigin, vDirection, step, fDirectional));
	CR(AddObject(pRay));

	return pRay;
Error:
	return nullptr;
}

/*
//RESULT Traverse(RESULT (*f)(std::shared_ptr<DimObj>), std::vector<std::shared_ptr<VirtualObj>> objects) {
RESULT Traverse(std::function<RESULT(std::shared_ptr<DimObj> pObject)> f, std::vector<std::shared_ptr<VirtualObj>> objects) {
	RESULT r = R_PASS;
	for (auto& v : objects) {
		auto d = std::dynamic_pointer_cast<DimObj>(v);
		if (d && d->HasChildren()) {
			CR((f)(d));
			CR(Traverse(f, d->GetChildren()));
		}
	}
Error:
	return r;
}
*/
