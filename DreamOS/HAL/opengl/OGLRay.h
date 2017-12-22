#ifndef OGL_RAY_H_
#define OGL_RAY_H_

#include "RESULT/EHM.h"

#include "OGLObj.h"
#include "Primitives/DimRay.h"
#include "Primitives/point.h"
#include "Primitives/vector.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLRay : public DimRay, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this; // nullptr
	}

public:
	OGLRay(OpenGLImp *pParentImp, point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);
	virtual RESULT Render() override;
};
#pragma warning(pop)


#endif // ! OGL_RAY_H_