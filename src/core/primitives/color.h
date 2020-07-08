#ifndef COLOR_H_
#define COLOR_H_

#include <stdlib.h>

#include "core/ehm/EHM.h"

// DREAM OS Core
// dos/src/core/primitives/color.h

// Color Primitive Object derived from matrix

#include "core/matrix/matrix.h"

#ifdef FLOAT_PRECISION
	typedef float color_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double color_precision;
#endif

enum COLOR_TYPE {
	COLOR_RANDOM,
	COLOR_WHITE,
	COLOR_BLACK,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_BLUE,
	COLOR_AQUA,
	COLOR_YELLOW,
	COLOR_PINK,
	COLOR_GRAY,
	COLOR_INVALID
};

enum class PIXEL_FORMAT {
	GREYSCALE,
	RGB,
	RGBA,
	BGR,
	BGRA,
	INVALID
};

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

	color(COLOR_TYPE type) {
		this->clear();

		switch (type) {
			case COLOR_RANDOM: {
				color_precision r = static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX);
				color_precision g = static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX);
				color_precision b = static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX);

				SetColor(r, g, b, 1.0f);
			} break;

			case COLOR_BLACK: { SetColor(0.0f, 0.0f, 0.0f, 1.0f); } break;
			case COLOR_WHITE: { SetColor(1.0f, 1.0f, 1.0f, 1.0f); } break;
			case COLOR_RED: { SetColor(1.0f, 0.0f, 0.0f, 1.0f); } break;
			case COLOR_GREEN: { SetColor(0.0f, 1.0f, 0.0f, 1.0f); } break;
			case COLOR_BLUE: { SetColor(0.0f, 0.0f, 1.0f, 1.0f); } break;
			case COLOR_YELLOW: { SetColor(1.0f, 1.0f, 0.0f, 1.0f); } break;
			case COLOR_PINK: { SetColor(1.0f, 20.0f/255.0f, 147.0f/255.0f, 1.0f); } break;
			case COLOR_AQUA: { SetColor(0.0f, 1.0f, 1.0f, 1.0f); } break;
			case COLOR_GRAY: { SetColor(0.2f, 0.2f, 0.2f, 1.0f); } break;

			default: {
				this->clear();
			} break;
		}
	}

	RESULT SetColor(color_precision r, color_precision g, color_precision b, color_precision a) {
		this->element(0, 0) = r;
		this->element(1, 0) = g;
		this->element(2, 0) = b;
		this->element(3, 0) = a;

		return R_PASS;
	}

	RESULT IncrementRGB(float amount) {

		this->r() += amount;

		if (this->r() > 1.0f) {
			float diffR = this->r() - 1.0f;

			this->r() = 1.0f;
			this->g() += diffR;

			if (this->g() > 1.0f) {
				float diffG = this->g() - 1.0f;

				this->g() = 0.0f;
				this->b() += diffG;

				if (this->b() > 1.0f) {
					this->r() = 0.0f; 
					this->g() = 0.0f; 
					this->b() = 0.0f;
				}
			}
		}

		return R_PASS;
	}

	// TODO: Understand performance implications of this although both element and this are inline
	inline color_precision &r() { return this->element(0, 0); }
	inline color_precision &g() { return this->element(1, 0); }
	inline color_precision &b() { return this->element(2, 0); }
	inline color_precision &a() { return this->element(3, 0); }

	inline color_precision &r(color_precision val) { return this->element(0, 0) = val; }
	inline color_precision &g(color_precision val) { return this->element(1, 0) = val; }
	inline color_precision &b(color_precision val) { return this->element(2, 0) = val; }
	inline color_precision &a(color_precision val) { return this->element(3, 0) = val; }
};

#endif // ! COLOR_H_
