#ifndef OGL_BILLBOARD_H_
#define OGL_BILLBOARD_H_

#include "RESULT/EHM.h"

#include "OGLObj.h"
#include "Primitives/billboard.h"

#pragma warning(push)
#pragma warning(disable : 4250)
class OGLBillboard : public billboard, public OGLObj {
public:
	OGLBillboard(OpenGLImp *pParentImp, point ptOrigin, float width, float height, texture *pTexture);
	~OGLBillboard();

	// override to specify GL_POIONT
	virtual RESULT Render() override;
};
#pragma warning(pop)

#endif // ! OGL_BILLBOARD_H_