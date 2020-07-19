#ifndef COMPOSITE_H_
#define COMPOSITE_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/primitives/composite.h

// Composite Primitive
// The composite object is a collection of other objects when these are constructed within the object itself 
// this is really a convenience object that should be used to derive others rather than off of DimObj directly

#include <memory>

#include "core/dimension/DimObj.h"

class HALImp;
//#include "HAL/HALImp.h"

class sphere;
class volume;
class DimRay;
class dosuser;
#include "core/primitives/quad.h"

class model;

class hand;
#include "core/hand/HandType.h"

class camera;
class FlatContext;
class UIView;
class stereocamera;

class DreamOS;

#include "core/model/ModelFactory.h"


class composite : public virtual DimObj {
public:
	composite(HALImp *pHALImp);

	// DimObj Interface
	virtual RESULT Allocate() override { return R_NOT_IMPLEMENTED_WARNING; }
	virtual inline unsigned int NumberVertices() override { return 0; }
	virtual inline unsigned int NumberIndices() override { return 0; }

	RESULT AddObject(std::shared_ptr<DimObj> pDimObj);
	RESULT ClearObjects();

	virtual RESULT UpdateBoundingVolume() override;

	// TODO: This is currently not building correctly, so has been removed for now
	/*
	template<typename objType, typename... Targs>
	std::shared_ptr<objType> Add(Targs... Fargs) {
		RESULT r = R_PASS;

		std::shared_ptr<objType> pObj(m_pHALImp->TMakeObject<objType>(Fargs...));
		CN(pObj);

		CR(AddObject(pObj));

		//Success:
		return pObj;

	Error:
		if (pObj != nullptr) {
			pObj = nullptr;
		}

		return nullptr;
	}

	template<typename objType, typename... Targs>
	std::shared_ptr<objType> Make(Targs... Fargs) {
		RESULT r = R_PASS;

		std::shared_ptr<objType> pObj(m_pHALImp->TMakeObject<objType>(Fargs...));
		CN(pObj);

		//Success:
		return pObj;

	Error:
		if (pObj != nullptr) {
			pObj = nullptr;
		}

		return nullptr;
	}

	template<typename objType>
	std::shared_ptr<objType> Add() {
		RESULT r = R_PASS;

		std::shared_ptr<objType> pObj(m_pHALImp->TMakeObject<objType>());
		CN(pObj);

		CR(AddObject(pObj));

		//Success:
		return pObj;

	Error:
		if (pObj != nullptr) {
			pObj = nullptr;
		}

		return nullptr;
	}

	template<typename objType>
	std::shared_ptr<objType> Make() {
		RESULT r = R_PASS;

		std::shared_ptr<objType> pObj(m_pHALImp->TMakeObject<objType>());
		CN(pObj);

		//Success:
		return pObj;

	Error:
		if (pObj != nullptr) {
			pObj = nullptr;
		}

		return nullptr;
	}
	//*/

	std::shared_ptr<sphere> MakeSphere(float radius, int numAngularDivisions, int numVerticalDivisions);
	std::shared_ptr<sphere> AddSphere(float radius, int numAngularDivisions, int numVerticalDivisions);

	std::shared_ptr<volume> MakeVolume(double width, double length, double height);
	std::shared_ptr<volume> AddVolume(double width, double length, double height);
	std::shared_ptr<volume> MakeVolume(double side);
	std::shared_ptr<volume> AddVolume(double side);

	//std::shared_ptr<mesh> MakeMesh(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, vector vEulerRotation);
	//std::shared_ptr<mesh> AddMesh(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, vector vEulerRotation);

	std::shared_ptr<composite> MakeComposite();
	std::shared_ptr<composite> AddComposite();

	std::shared_ptr<model> MakeModel(const std::wstring& wstrModelFilename, texture* pTexture = nullptr);
	std::shared_ptr<model> AddModel(const std::wstring& wstrModelFilename, texture* pTexture = nullptr);
	std::shared_ptr<model> MakeModel(const std::wstring& wstrModelFilename, ModelFactory::flags modelFactoryFlags);
	std::shared_ptr<model> AddModel(const std::wstring& wstrModelFilename, ModelFactory::flags modelFactoryFlags);

	std::shared_ptr<hand> MakeHand(HAND_TYPE type);
	std::shared_ptr<hand> AddHand(HAND_TYPE type);

	std::shared_ptr<hand> MakeHand(HAND_TYPE type, long avatarID);
	std::shared_ptr<hand> AddHand(HAND_TYPE type, long avatarID);

	std::shared_ptr<dosuser> MakeUser();
	std::shared_ptr<dosuser> AddUser();

	std::shared_ptr<quad> MakeQuad(double width, double height, point ptOrigin);
	std::shared_ptr<quad> MakeQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	std::shared_ptr<quad> MakeQuad(double width, double height, point ptOrigin, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal = vector::jVector());
	std::shared_ptr<quad> MakeQuad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType = quad::CurveType::FLAT, vector vNormal = vector::jVector());
	
	std::shared_ptr<quad> AddQuad(double width, double height, point ptOrigin);
	std::shared_ptr<quad> AddQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	std::shared_ptr<quad> AddQuad(double width, double height, point ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal = vector::jVector());
	std::shared_ptr<quad> AddQuad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType = quad::CurveType::FLAT, vector vNormal = vector::jVector());

	std::shared_ptr<DimRay> MakeRay(point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);
	std::shared_ptr<DimRay> AddRay(point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);

	std::shared_ptr<FlatContext> MakeFlatContext(int width = 1024, int height = 1024, int channels = 4);
	std::shared_ptr<FlatContext> AddFlatContext(int width = 1024, int height = 1024, int channels = 4);

	std::shared_ptr<UIView> MakeUIView(DreamOS *pDreamOS);
	std::shared_ptr<UIView> AddUIView(DreamOS *pDreamOS);

	std::shared_ptr<texture> MakeTexture(texture::type type, wchar_t *pszFilename);
	std::shared_ptr<texture> MakeTexture(texture::type type, int width, int height, PIXEL_FORMAT pixelFormat, int channels, void *pBuffer, int pBuffer_n);

	// TODO: This is temporary - should move all textures to 
	// shared pointers or use a central store / special texture object handle
	// that chops the memory when not used 
	texture* MakeTextureRaw(texture::type type, wchar_t *pszFilename);

public:
	RESULT RenderToTexture(std::shared_ptr<FlatContext> context);
	stereocamera* GetCamera();

protected:
	HALImp *m_pHALImp;
};

#endif	// ! COMPOSITE_H_
