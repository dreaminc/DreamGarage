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

	std::shared_ptr<text> MakeText(const std::wstring& fontName, const std::string& content, double size);
	std::shared_ptr<text> AddText(const std::wstring& fontName, const std::string& content, double size);

public:
	framebuffer* GetFramebuffer() { return m_pFramebuffer; }
	void SetFramebuffer(framebuffer* pFramebuffer) { m_pFramebuffer = pFramebuffer; }

private:
	framebuffer* m_pFramebuffer;
};

#endif	// ! FLAT_CONTEXT_H_
