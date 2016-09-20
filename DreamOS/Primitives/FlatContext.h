#ifndef FLAT_CONTEXT_H_
#define FLAT_CONTEXT_H_

#include "Primitives/composite.h"
#include "Primitives/text.h"

class FlatContext : public composite {
public:

	FlatContext(HALImp *pHALImp);

	std::shared_ptr<quad> MakeQuad(double width, double height, point origin);
	std::shared_ptr<quad> AddQuad(double width, double height, point origin);

	std::shared_ptr<text> MakeText(Font::TYPE type, const std::string& content, double size);
	std::shared_ptr<text> AddText(Font::TYPE type, const std::string& content, double size);
};

#endif	// ! FLAT_CONTEXT_H_
