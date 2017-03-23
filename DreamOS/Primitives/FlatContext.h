#ifndef FLAT_CONTEXT_H_
#define FLAT_CONTEXT_H_

#include "Primitives/composite.h"
#include "Primitives/text.h"
#include "Primitives/framebuffer.h"

class FlatContext : public composite {
public:

	FlatContext(HALImp *pHALImp);

	std::shared_ptr<quad> MakeQuad(double width, double height, point origin);
	std::shared_ptr<quad> AddQuad(double width, double height, point origin);

	std::shared_ptr<text> MakeText(std::shared_ptr<Font> pFont, const std::string& content, double size, bool fDistanceMap = false);
	std::shared_ptr<text> AddText(std::shared_ptr<Font> pFont, const std::string& content, double size, bool fDistanceMap);

public:
	framebuffer* GetFramebuffer() { return m_pFramebuffer; }
	void SetFramebuffer(framebuffer* pFramebuffer) { m_pFramebuffer = pFramebuffer; }

private:
	framebuffer* m_pFramebuffer;
};

#endif	// ! FLAT_CONTEXT_H_
