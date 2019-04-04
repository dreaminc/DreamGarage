#include "composite.h"

#include "HAL/HALImp.h"

#include "Primitives/hand.h"
#include "Primitives/user.h"

#include "Primitives/FlatContext.h"
#include "Primitives/camera.h"
#include "Primitives/stereocamera.h"

#include "Primitives/sphere.h"
#include "Primitives/volume.h"
#include "Primitives/DimRay.h"
#include "quad.h"

#include "UI/UIView.h"

#include "DreamOS.h"

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

	CNR(m_pBoundingVolume, R_SKIPPED);

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


std::shared_ptr<texture> composite::MakeTexture(texture::type type, wchar_t *pszFilename) {

	RESULT r = R_PASS;

	std::shared_ptr<texture> pTexture(m_pHALImp->MakeTexture(type, pszFilename));
	CN(pTexture);

Success:
	return pTexture;

Error:
	if (pTexture != nullptr) {
		pTexture = nullptr;
	}

	return nullptr;
}

std::shared_ptr<texture> composite::MakeTexture(texture::type type, int width, int height, PIXEL_FORMAT pixelFormat, int channels, void *pBuffer, int pBuffer_n) {
	RESULT r = R_PASS;

	std::shared_ptr<texture> pTexture(m_pHALImp->MakeTexture(type, width, height, pixelFormat, channels, pBuffer, pBuffer_n));
	CN(pTexture);

Success:
	return pTexture;

Error:
	if (pTexture != nullptr) {
		pTexture = nullptr;
	}

	return nullptr;
}

texture* composite::MakeTextureRaw(texture::type type, wchar_t *pszFilename) {
	RESULT r = R_PASS;

	texture* pTexture = m_pHALImp->MakeTexture(type, pszFilename);
	CN(pTexture);

Success:
	return pTexture;

Error:
	if (pTexture != nullptr) {
		delete pTexture;
		pTexture = nullptr;
	}

	return nullptr;
}

std::shared_ptr<hand> composite::MakeHand(HAND_TYPE type) {
	RESULT r = R_PASS;

	std::shared_ptr<hand> pHand(m_pHALImp->MakeHand(type));
	CN(pHand);

Success:
	return pHand;

Error:
	if (pHand != nullptr) {
		pHand = nullptr;
	}
	return nullptr;
}

std::shared_ptr<hand> composite::AddHand(HAND_TYPE type) {
	RESULT r = R_PASS;

	std::shared_ptr<hand> pHand = MakeHand(type);
	CN(pHand);

	CR(AddObject(pHand));

Success:
	return pHand;

Error:
	if (pHand != nullptr) {
		pHand = nullptr;
	}

	return nullptr;
}

std::shared_ptr<hand> composite::MakeHand(HAND_TYPE type, long avatarID) {
	RESULT r = R_PASS;

	std::shared_ptr<hand> pHand(m_pHALImp->MakeHand(type, avatarID));
	CN(pHand);

Success:
	return pHand;

Error:
	if (pHand != nullptr) {
		pHand = nullptr;
	}
	return nullptr;
}

std::shared_ptr<hand> composite::AddHand(HAND_TYPE type, long avatarID) {
	RESULT r = R_PASS;

	std::shared_ptr<hand> pHand = MakeHand(type, avatarID);
	CN(pHand);

	CR(AddObject(pHand));

Success:
	return pHand;

Error:
	if (pHand != nullptr) {
		pHand = nullptr;
	}

	return nullptr;
}

std::shared_ptr<user> composite::MakeUser() {
	RESULT r = R_PASS;

	std::shared_ptr<user> pUser(m_pHALImp->MakeUser());
	CN(pUser);

Success:
	return pUser;

Error:
	if (pUser != nullptr) {
		pUser = nullptr;
	}

	return nullptr;
}

std::shared_ptr<user> composite::AddUser() {
	RESULT r = R_PASS;

	std::shared_ptr<user> pUser = MakeUser();
	CN(pUser);

	CR(AddObject(pUser));

Success:
	return pUser;

Error:
	if (pUser != nullptr) {
		pUser = nullptr;
	}

	return nullptr;
}

/*
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
*/

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
	pComposite = nullptr;
	return nullptr;
}

std::shared_ptr<model> composite::MakeModel(const std::wstring& wstrModelFilename, texture* pTexture) {
	RESULT r = R_PASS;

	// TODO: Other bits (position, scale, rotation)

	std::shared_ptr<model> pModel(ModelFactory::MakeModel(m_pHALImp, wstrModelFilename));
	CN(pModel);

	// Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		pModel = nullptr;
	}

	return nullptr;
}

std::shared_ptr<model> composite::AddModel(const std::wstring& wstrModelFilename, texture* pTexture) {
	RESULT r = R_PASS;

	std::shared_ptr<model> pModel = MakeModel(wstrModelFilename, pTexture);
	CN(pModel);

	CR(AddObject(pModel));

	// Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		pModel = nullptr;
	}

	return nullptr;
}

std::shared_ptr<model> composite::MakeModel(const std::wstring& wstrModelFilename, ModelFactory::flags modelFactoryFlags) {
	RESULT r = R_PASS;

	// TODO: Other bits (position, scale, rotation)

	std::shared_ptr<model> pModel(ModelFactory::MakeModel(m_pHALImp, wstrModelFilename, modelFactoryFlags));
	CN(pModel);

	// Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		pModel = nullptr;
	}

	return nullptr;

}

std::shared_ptr<model> composite::AddModel(const std::wstring& wstrModelFilename, ModelFactory::flags modelFactoryFlags) {
	RESULT r = R_PASS;

	std::shared_ptr<model> pModel = MakeModel(wstrModelFilename, modelFactoryFlags);
	CN(pModel);

	CR(AddObject(pModel));

	// Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		pModel = nullptr;
	}

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

std::shared_ptr<quad> composite::MakeQuad(double width, double height, point ptOrigin) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad(m_pHALImp->MakeQuad(width, height));
	CN(pQuad);

	pQuad->SetPosition(ptOrigin);

Success:
	return pQuad;

Error:
	if (pQuad != nullptr) {
		pQuad = nullptr;
	}

	return nullptr;
}

std::shared_ptr<quad> composite::AddQuad(double width, double height, point ptOrigin) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad = MakeQuad(width, height, ptOrigin);
	CN(pQuad);

	CR(AddObject(pQuad));

Success:
	return pQuad;

Error:
	if (pQuad != nullptr) {
		pQuad = nullptr;
	}

	return nullptr;
}

std::shared_ptr<quad> composite::MakeQuad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType, vector vNormal) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad(m_pHALImp->MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, uvTopLeft, uvBottomRight, curveType, vNormal));
	CN(pQuad);

Success:
	return pQuad;

Error:
	if (pQuad != nullptr) {
		pQuad = nullptr;
	}

	return nullptr;
}



std::shared_ptr<quad> composite::AddQuad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType, vector vNormal) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad = MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, uvTopLeft, uvBottomRight, curveType, vNormal);
	CN(pQuad);

	CR(AddObject(pQuad));

Success:
	return pQuad;

Error:
	if (pQuad != nullptr) {
		pQuad = nullptr;
	}

	return nullptr;
}

std::shared_ptr<quad> composite::MakeQuad(double width, double height, point ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal) {

	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad(m_pHALImp->MakeQuad(width, height, ptCenter, uvTopLeft, uvBottomRight, vNormal));
	CN(pQuad);

	// Not a fan of this but this will resolve a lot of issues it seems
	pQuad->SetPosition(ptCenter);

Success:
	return pQuad;

Error:
	return nullptr;
}

std::shared_ptr<quad> composite::AddQuad(double width, double height, point ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad(m_pHALImp->MakeQuad(width, height, ptCenter, uvTopLeft, uvBottomRight, vNormal));
	CN(pQuad);
	
	CR(AddObject(pQuad));

Success:
	return pQuad;

Error:
	if (pQuad != nullptr) {
		pQuad = nullptr;
	}

	return nullptr;
}

std::shared_ptr<quad> composite::MakeQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture * pTextureHeight, vector vNormal) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad(m_pHALImp->MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal));
	CN(pQuad);

Success:
	return pQuad;

Error:
	if (pQuad != nullptr) {
		pQuad = nullptr;
	}
	return nullptr;
}

std::shared_ptr<quad> composite::AddQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture * pTextureHeight, vector vNormal){
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad = MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal);
	CN(pQuad);

	CR(AddObject(pQuad));

	return pQuad;

Error:
	if (pQuad != nullptr) {
		pQuad = nullptr;
	}

	return nullptr;
}

std::shared_ptr<DimRay> composite::MakeRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	RESULT r = R_PASS;

	std::shared_ptr<DimRay> pRay(m_pHALImp->MakeRay(ptOrigin, vDirection, step, fDirectional));
	CN(pRay);

Success:
	return pRay;

Error:
	if (pRay != nullptr) {
		pRay = nullptr;
	}

	return nullptr;
}

std::shared_ptr<DimRay> composite::AddRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	RESULT r = R_PASS;

	std::shared_ptr<DimRay> pRay(m_pHALImp->MakeRay(ptOrigin, vDirection, step, fDirectional));
	CN(pRay);

	CR(AddObject(pRay));

	return pRay;

Error:
	if (pRay != nullptr) {
		pRay = nullptr;
	}

	return nullptr;
}

std::shared_ptr<FlatContext> composite::MakeFlatContext(int width, int height, int channels) {
	RESULT r = R_PASS;

	std::shared_ptr<FlatContext> pContext(m_pHALImp->MakeFlatContext(width, height, channels));
	CN(pContext);

Success:
	return pContext;

Error:
	if (pContext != nullptr) {
		pContext = nullptr;
	}

	return nullptr;
}

std::shared_ptr<FlatContext> composite::AddFlatContext(int width, int height, int channels) {
	RESULT r = R_PASS;

	std::shared_ptr<FlatContext> pContext(m_pHALImp->MakeFlatContext(width, height, channels));
	CN(pContext);

	CR(AddObject(pContext));

Success:
	return pContext;

Error:
	if (pContext != nullptr) {
		pContext = nullptr;
	}

	return nullptr;
}

std::shared_ptr<UIView> composite::MakeUIView(DreamOS *pDreamOS) {
	RESULT r = R_PASS;

	std::shared_ptr<UIView> pView(new UIView(m_pHALImp, pDreamOS));
	CN(pView);

Success:
	return pView;

Error:
	if (pView != nullptr) {
		pView = nullptr;
	}

	return nullptr;
}

std::shared_ptr<UIView> composite::AddUIView(DreamOS *pDreamOS) {
	RESULT r = R_PASS;

	std::shared_ptr<UIView> pView = MakeUIView(pDreamOS);
	CN(pView);

	CR(AddObject(pView));

Success:
	return pView;

Error:
	if (pView != nullptr) {
		pView = nullptr;
	}

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
