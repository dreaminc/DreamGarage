#ifndef UV_COORD_H_
#define UV_COORD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/uvcoord.h
// UV Coordinate

#include "matrix/matrix.h"


#ifdef FLOAT_PRECISION
	typedef float uv_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double uv_precision;
#endif

class uvcoord : public matrix <uv_precision, 2, 1> {
public:
	uvcoord() {
		clear();
	}

	uvcoord(uv_precision u, uv_precision v) {
		this->clear();
		this->element(0, 0) = u;
		this->element(1, 0) = v;
	}

	uvcoord(const matrix<uv_precision, 2, 1>& arg) :
		matrix<uv_precision, 2, 1>(arg)
	{
		// empty
	}

	inline RESULT Clamp(uv_precision low = 0.0f, uv_precision high = 1.0f) {
		for (int i = 0; i < 2; i++) {
			if (this->element(i, 0) > high)
				this->element(i, 0) = high;
			else if (this->element(i, 0) < low)
				this->element(i, 0) = low;
		}

			return R_PASS;
	}

	// TODO: Understand performance implications of this although both element and this are inline
	inline uv_precision &u() { return this->element(0, 0); }
	inline uv_precision &v() { return this->element(1, 0); }

	const uv_precision &u() const { return this->element(0, 0); }
	const uv_precision &v() const { return this->element(1, 0); }
	
	inline uv_precision &u(uv_precision val) { return this->element(0, 0) = val; }
	inline uv_precision &v(uv_precision val) { return this->element(1, 0) = val; }

	// Explicitly specializing the assignment operator
	uvcoord& operator=(const matrix<uv_precision, 2, 1> &arg) {
		if (this == &arg)      // Same object?
			return *this;        // Yes, so skip assignment, and just return *this.

		memcpy(this->m_data, arg.m_data, sizeof(uv_precision) * 2 * 1);

		return *this;
	}
};

#endif // ! UVCOORD_H_
