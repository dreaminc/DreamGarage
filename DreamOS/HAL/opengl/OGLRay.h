#ifndef OGL_RAY_H_
#define OGL_RAY_H_

#include "RESULT/EHM.h"

#include "OGLObj.h"
#include "Primitives/DimRay.h"
#include "Primitives/point.h"
#include "Primitives/vector.h"

class OGLVolume;

class OGLRay : public DimRay, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this; // nullptr
	}

public:
	OGLRay(OpenGLImp *pParentImp, point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);
	virtual RESULT Render() override;

};


#endif // ! OGL_RAY_H_