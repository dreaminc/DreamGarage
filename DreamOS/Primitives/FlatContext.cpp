#include "FlatContext.h"
#include "HAL/HALImp.h"

FlatContext::FlatContext(HALImp * pHALImp) :
	composite(pHALImp)
{
	// TODO add UI capabilities (alignments, etc)
}

std::shared_ptr<quad> FlatContext::MakeQuad(double width, double height, point origin) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad(m_pHALImp->MakeQuad(width, height, origin));

//Success:
	return pQuad;

//Error:
	return nullptr;
}

std::shared_ptr<quad> FlatContext::AddQuad(double width, double height, point origin) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad = MakeQuad(width, height, origin);
	CR(AddObject(pQuad));

//Success:
	return pQuad;

Error:
	return nullptr;
}

std::shared_ptr<text> FlatContext::MakeText(std::shared_ptr<Font> pFont, const std::string& content, double size, bool fDistanceMap) {
	RESULT r = R_PASS;

	std::shared_ptr<text> pText(m_pHALImp->MakeText(pFont, content, size, fDistanceMap));
	return pText;
}

std::shared_ptr<text> FlatContext::AddText(std::shared_ptr<Font> pFont, const std::string& content, double size, bool fDistanceMap) {
	RESULT r = R_PASS;

	std::shared_ptr<text> pText = MakeText(pFont, content, size, fDistanceMap);
	CR(AddObject(pText));

//Success:
	return pText;

Error:
	return nullptr;
}
