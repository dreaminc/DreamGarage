#ifndef FLAT_CONTEXT_H_
#define FLAT_CONTEXT_H_

#include "Primitives/composite.h"

class FlatContext : public composite {
public:

	FlatContext(HALImp *pHALImp);

	std::shared_ptr<quad> MakeQuad(double width, double height, point origin);
	std::shared_ptr<quad> AddQuad(double width, double height, point origin);
};

#endif	// ! FLAT_CONTEXT_H_
