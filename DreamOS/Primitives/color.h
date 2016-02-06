#ifndef COLOR_H_
#define COLOR_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/color.h
// Color Primitive Object derived from matrix

#include "matrix.h"

#ifdef FLOAT_PRECISION
	typedef float color_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double color_precision;
#endif

class color : public matrix <color_precision, 4, 1> {
public:
	color() {
		clear();
	}

	color(color_precision r, color_precision g, color_precision b, color_precision a) {
		this->clear();
		this->element(0, 0) = r;
		this->element(1, 0) = g;
		this->element(2, 0) = b;
		this->element(3, 0) = a;
	}

	// TODO: Understand performance implications of this although both element and this are inline
	inline color_precision &r() { return this->element(0, 0); }
	inline color_precision &g() { return this->element(1, 0); }
	inline color_precision &b() { return this->element(2, 0); }
	inline color_precision &a() { return this->element(3, 0); }

	inline color_precision &r(vector_precision val) { return this->element(0, 0) = val; }
	inline color_precision &g(vector_precision val) { return this->element(1, 0) = val; }
	inline color_precision &b(vector_precision val) { return this->element(2, 0) = val; }
	inline color_precision &a(vector_precision val) { return this->element(3, 0) = val; }
};

#endif // ! COLOR_H_
