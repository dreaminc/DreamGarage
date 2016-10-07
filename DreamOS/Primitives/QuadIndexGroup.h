#ifndef QUAD_INDEX_GROUP_H_
#define QUAD_INDEX_GROUP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/QuadIndexGroup.h
// This is a quad index group

#include "matrix.h"

typedef uint32_t dimindex;

class QuadIndexGroup : public matrix <dimindex, 4, 1> {
public:
	QuadIndexGroup() {
		clear();
	}

	QuadIndexGroup(dimindex a, dimindex b, dimindex c, dimindex d) {
		this->clear();

		this->element(0, 0) = a;
		this->element(1, 0) = b;
		this->element(2, 0) = c;
		this->element(3, 0) = d;
	}

	// TODO: Understand performance implications of this although both element and this are inline
	inline dimindex &a() { return this->element(0, 0); }
	inline dimindex &b() { return this->element(1, 0); }
	inline dimindex &c() { return this->element(2, 0); }
	inline dimindex &d() { return this->element(3, 0); }

	inline dimindex &a(dimindex val) { return this->element(0, 0) = val; }
	inline dimindex &b(dimindex val) { return this->element(1, 0) = val; }
	inline dimindex &c(dimindex val) { return this->element(2, 0) = val; }
	inline dimindex &d(dimindex val) { return this->element(3, 0) = val; }

	// Explicitly specializing the assignment operator
	QuadIndexGroup& operator=(const matrix<dimindex, 4, 1> &arg) {
		if (this == &arg)      // Same object?
			return *this;        // Yes, so skip assignment, and just return *this.

		memcpy(this->m_data, arg.m_data, sizeof(dimindex) * 4 * 1);

		return *this;
	}
};

#endif // ! QUAD_INDEX_GROUP_H_