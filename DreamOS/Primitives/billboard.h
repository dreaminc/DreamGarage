#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include "DimObj.h"

class point;
class texture;

class billboard : public DimObj {
public:
	billboard(point ptOrigin, float width, float height, texture *pTexture);
	~billboard();

	RESULT SetTexture(texture *pTexture);
	texture *GetTexture();

	RESULT SetVertex(point ptOrigin);

public:
	virtual unsigned int NumberIndices() override;
	virtual unsigned int NumberVertices() override;
	virtual RESULT Allocate() override;

private:
	texture *m_pTexture = nullptr;

	float m_width;
	float m_height;
};

#endif // ! _BILLBOARD_H_