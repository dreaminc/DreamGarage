#include "matrix.h"

template <typename TMatrix, int N, int M>
matrix<TMatrix, (N - 1), (M - 1)> matrix<TMatrix, N, M>::minor(unsigned i, unsigned j) {
	// TODO: create minor
}

/*
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
*/

// inverse 
/*
template <typename TMat4x4>
matrix<TMat4x4, 2, 2> inverse(matrix<TMat4x4, 2, 2>& arg) {
	matrix<TMat4x4, 2, 2> result;

	TMat4x4 det = arg.determinant();

	return result;
}
*/