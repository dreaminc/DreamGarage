#include "composite.h"

#include "HAL/HALImp.h"

#include "Primitives/hand.h"

#include "Primitives/FlatContext.h"
#include "Primitives/camera.h"
#include "Primitives/stereocamera.h"

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

	point ptMax; 
	point ptMin; 
	point ptMid; 

	/*
	point ptMinTemp = GetOrigin();
	point ptMaxTemp = GetOrigin();
	*/

	point ptMinTemp = point();
	point ptMaxTemp = point();

	CN(m_pBoundingVolume);

	if (HasChildren()) {
		for (auto &childObj : GetChildren()) {
			std::shared_ptr<DimObj> pDimObj = std::dynamic_pointer_cast<DimObj>(childObj);
			

			if (pDimObj != nullptr) {
				auto pObjBoundingVolume = pDimObj->GetBoundingVolume();

				if (pObjBoundingVolume != nullptr) {
					ptMinTemp = pObjBoundingVolume->GetMinPointOriented();
					ptMaxTemp = pObjBoundingVolume->GetMaxPointOriented();

					// X
					if (ptMaxTemp.x() > ptMax.x())
						ptMax.x() = ptMaxTemp.x();
					else if (ptMaxTemp.x() < ptMin.x())
						ptMin.x() = ptMaxTemp.x();

					if (ptMinTemp.x() > ptMax.x())
						ptMax.x() = ptMinTemp.x();
					else if (ptMinTemp.x() < ptMin.x())
						ptMin.x() = ptMinTemp.x();

					// Y
					if (ptMaxTemp.y() > ptMax.y())
						ptMax.y() = ptMaxTemp.y();
					else if (ptMaxTemp.y() < ptMin.y())
						ptMin.y() = ptMaxTemp.y();

					if (ptMinTemp.y() > ptMax.y())
						ptMax.y() = ptMinTemp.y();
					else if (ptMinTemp.y() < ptMin.y())
						ptMin.y() = ptMinTemp.y();

					// Z
					if (ptMaxTemp.z() > ptMax.z())
						ptMax.z() = ptMaxTemp.z();
					else if (ptMaxTemp.z() < ptMin.z())
						ptMin.z() = ptMaxTemp.z();

					if (ptMinTemp.z() > ptMax.z())
						ptMax.z() = ptMinTemp.z();
					else if (ptMinTemp.z() < ptMin.z())
						ptMin.z() = ptMinTemp.z();
				}	// pBoundingVolume
			}	// pDimObj
		} // FOR

		// TODO: Composite is not calculating pivot (Center of mass) vs origin 
		// there needs to be more work here, especially if we want these to respond physically
		CR(m_pBoundingVolume->UpdateBoundingVolumeMinMax(ptMin, ptMax));
	}  // HasChildren()
	else {

		// TODO: Composite is not calculating pivot (Center of mass) vs origin 
		// there needs to be more work here, especially if we want these to respond physically
		CR(m_pBoundingVolume->UpdateBoundingVolumeMinMax(point(), point()));	
	}

	CR(m_pBoundingVolume->SetDirty());

	// Handle nested composites
	if (m_pParent != nullptr) {
		m_pParent->UpdateBoundingVolume();
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

std::shared_ptr<texture> composite::MakeTexture(texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n) {
	RESULT r = R_PASS;

	std::shared_ptr<texture> pTexture(m_pHALImp->MakeTexture(type, width, height, format, channels, pBuffer, pBuffer_n));

	//Success:
	return pTexture;
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

std::shared_ptr<quad> composite::MakeQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture * pTextureHeight, vector vNormal) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad(m_pHALImp->MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal));
	CR(AddObject(pQuad));

//Success:
	return pQuad;

Error:
	return nullptr;
}

std::shared_ptr<quad> composite::AddQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture * pTextureHeight, vector vNormal)
{
	return MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal);
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

std::shared_ptr<FlatContext> composite::MakeFlatContext(int width, int height, int channels) {
	RESULT r = R_PASS;

	std::shared_ptr<FlatContext> pContext(m_pHALImp->MakeFlatContext(width, height, channels));

	return pContext;
}

std::shared_ptr<FlatContext> composite::AddFlatContext(int width, int height, int channels) {
	RESULT r = R_PASS;

	std::shared_ptr<FlatContext> pContext(m_pHALImp->MakeFlatContext(width, height, channels));
	CR(AddObject(pContext));

	return pContext;
Error:
	return nullptr;
}

RESULT composite::RenderToTexture(std::shared_ptr<FlatContext> pContext) {
	RESULT r = R_PASS;
	
	CR(m_pHALImp->RenderToTexture(pContext.get(), GetCamera()));

Error:
	return r;
}

stereocamera* composite::GetCamera() {
	return m_pHALImp->GetCamera();
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
