#include "matrix.h"

// TODO: Implement determinant (not critical atm)
// Only applicable for square matrices
// TODO: Not using Leibniz / Laplace 
template <typename TMat4x4, int N, int M>
TMat4x4 matrix<TMat4x4, N, M>::determinant() {
	TMat4x4 result = -1;

	if (!IsSquare()) {
		DEBUG_LINEOUT("Cannot calculate determinant for %d x %d matrix", N, M);
		return NULL;
	}

	// TODO: This is not working generally

	return result;
}

// Formulas from or derived based on: https://en.wikipedia.org/wiki/Determinant
template <typename TMat2x2>
TMat2x2 matrix<TMat2x2, 2, 2>::determinant() {
	return (m_data[0] * m_data[3]) - (m_data[1] * m_data[2]);
}

template <typename TMat3x3>
TMat3x3 matrix<TMat3x3, 3, 3>::determinant() {
	TMat3x3 result = -1;

	result =  (m_data[0] * m_data[4] * m_data[8]);
	result += (m_data[1] * m_data[5] * m_data[6]);
	result += (m_data[2] * m_data[3] * m_data[7]);

	result -= (m_data[2] * m_data[4] * m_data[6]);
	result -= (m_data[1] * m_data[3] * m_data[8]);
	result -= (m_data[0] * m_data[5] * m_data[7]);

	return result;
}

template <typename TMat4x4>
TMat4x4 matrix<TMat4x4, 4, 4>::determinant() {
	TMat3x3 result = -1;

	result = (m_data[0] * m_data[4] * m_data[8]);
	result += (m_data[1] * m_data[5] * m_data[6]);
	result += (m_data[2] * m_data[3] * m_data[7]);

	result -= (m_data[2] * m_data[4] * m_data[6]);
	result -= (m_data[1] * m_data[3] * m_data[8]);
	result -= (m_data[0] * m_data[5] * m_data[7]);

	return result;
}

template <typename TMatrix, int N, int M>
matrix<TMatrix, (N - 1), (M - 1)> matrix<TMatrix, N, M>::minor(unsigned i, unsigned j) {
	// TODO: create minor
}

template <typename TMat4x4, int N, int M>
matrix<TMat4x4, N, M> matrix<TMat4x4, N, M>::inverse() {
	matrix<TMat4x4, N, M> retMatrix = matrix<TMat4x4, N, M>::MakeIdentity();

	if (!IsSquare()) {
		DEBUG_LINEOUT("Cannot calculate determinant for %d x %d matrix", N, M);
		return NULL;
	}
	
	// TODO: This is not working generally

	return retMatrix;
}

// inverse 
/*
template <typename TMat4x4>
matrix<TMat4x4, 2, 2> inverse(matrix<TMat4x4, 2, 2>& arg) {
	matrix<TMat4x4, 2, 2> result;

	TMat4x4 det = arg.determinant();

	return result;
}
*/