#ifndef COLOR_H_
#define COLOR_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/color.h
// Color Primitive Object derived from matrix

#include "matrix.h"

class color : public matrix <double, 4, 1> {
public:
	color() {
		clear();
	}

	color(double r, double g, double b, double a) {
		this->clear();
		this->element(0, 0) = r;
		this->element(1, 0) = g;
		this->element(2, 0) = b;
		this->element(3, 0) = a;
	}

	// TODO: Understand performance implications of this although both element and this are inline
	inline double &r() { return this->element(0, 0); }
	inline double &g() { return this->element(1, 0); }
	inline double &b() { return this->element(2, 0); }
	inline double &a() { return this->element(3, 0); }
};

#endif // ! COLOR_H_
