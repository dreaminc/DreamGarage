#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include "VirtualObj.h"

class point;
class texture;

class billboard : public VirtualObj {
public:
	billboard(point ptOrigin, float width, float height, texture *pTexture);
	~billboard();

	RESULT SetTexture(texture *pTexture);
	texture *GetTexture();

private:
	texture *m_pTexture = nullptr;

	float m_width;
	float m_height;
};

#endif // ! _BILLBOARD_H_