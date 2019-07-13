#ifndef TRIANGLE_INDEX_GROUP_H_
#define TRIANGLE_INDEX_GROUP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/TriangleIndexGroup.h
// This is a triangle index group

#include "matrix/matrix.h"

typedef uint32_t dimindex;

class TriangleIndexGroup : public matrix <dimindex, 3, 1> {
public:
	TriangleIndexGroup() {
		clear();
	}

	TriangleIndexGroup(dimindex a, dimindex b, dimindex c) {
		this->clear();

		this->element(0, 0) = a;
		this->element(1, 0) = b;
		this->element(2, 0) = c;
	}

	// TODO: Understand performance implications of this although both element and this are inline
	inline dimindex &a() { return this->element(0, 0); }
	inline dimindex &b() { return this->element(1, 0); }
	inline dimindex &c() { return this->element(2, 0); }

	inline dimindex &a(dimindex val) { return this->element(0, 0) = val; }
	inline dimindex &b(dimindex val) { return this->element(1, 0) = val; }
	inline dimindex &c(dimindex val) { return this->element(2, 0) = val; }

	// Explicitly specializing the assignment operator
	TriangleIndexGroup& operator=(const matrix<dimindex, 3, 1> &arg) {
		if (this == &arg)      // Same object?
			return *this;        // Yes, so skip assignment, and just return *this.

		memcpy(this->m_data, arg.m_data, sizeof(dimindex) * 3 * 1);

		return *this;
	}
};

#endif // ! TRIANGLE_INDEX_GROUP_H_