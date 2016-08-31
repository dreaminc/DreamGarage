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
#include "quad.h"

class hand;

class composite : public DimObj {
public:
	composite(HALImp *pHALImp);

	// DimObj Interface
	RESULT Allocate();
	inline unsigned int NumberVertices();
	inline unsigned int NumberIndices();

	RESULT AddObject(std::shared_ptr<DimObj> pDimObj);
	RESULT ClearObjects();

	std::shared_ptr<sphere> MakeSphere(float radius, int numAngularDivisions, int numVerticalDivisions);
	std::shared_ptr<sphere> AddSphere(float radius, int numAngularDivisions, int numVerticalDivisions);

	std::shared_ptr<volume> MakeVolume(double width, double length, double height);
	std::shared_ptr<volume> AddVolume(double width, double length, double height);
	std::shared_ptr<volume> MakeVolume(double side);
	std::shared_ptr<volume> AddVolume(double side);

	std::shared_ptr<composite> MakeModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, point_precision rotateY);
	std::shared_ptr<composite> AddModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, point_precision rotateY);

	std::shared_ptr<hand> MakeHand();
	std::shared_ptr<hand> AddHand();

	std::shared_ptr<quad> MakeQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr);
	std::shared_ptr<quad> AddQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr);

	std::shared_ptr<texture> MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type);

protected:
	HALImp *m_pHALImp;
};

#endif	// ! COMPOSITE_H_