#ifndef MATRIX_H_
#define MATRIX_H_

// DREAM OS
// DreamOS/Dimension/Primitives/matrix.h
// Matrix Primitive Object

#include "RESULT/EHM.h"
#include <cstring>
#include <math.h>

//#define RANGE_CHECK 

// BIG TODO: Create Matrix testing suite

template <typename TBase>
class MatrixBase {
public:
    //virtual int rows() = 0;
    //virtual int cols() = 0;

	//virtual int getNumRows() = 0;
	//virtual int getNumColumns() = 0;

	/*
    virtual T& operator()(unsigned i, unsigned j) = 0;
    virtual const T& operator()( unsigned i, unsigned j ) const = 0;
    virtual const T& element(unsigned i, unsigned j) const = 0;
    virtual T& element(unsigned i, unsigned j) = 0;
	

protected:
    virtual T* getData() = 0;
	*/
};

template <typename TMatrix, int N = 4, int M = 4>
class matrix : public MatrixBase<TMatrix> {
//class matrix : public MatrixBase {

public:
    TMatrix m_data[N * M];

public:
    static int rows() { return N; }
    static int cols() { return M; }

	//inline int getNumRows() { return N; }
	//inline int getNumColumns() { return M; }
	

	//int rows() { return N; }
	//int cols() { return M; }
	
	// Simply clears data
	RESULT clear() {
		memset(m_data, 0, sizeof(TMatrix) * M * N);
		return R_PASS;
	}

	// Sets up an identity matrix
	RESULT identity() {
		RESULT r = R_PASS;

		// Ensure square matrix
		CBM((N == M), "Cant sent identity matrix on %dx%d dimensions", N, M);

		clear();

		for (int i = 0; i < N; i++)
			this->element(i, i) = 1.0f;

	Error:
		return r;
	}

public:
	// Constructors
	matrix() {
		/* stub */
	}

	matrix(const matrix<TMatrix, N, M> &cp) {
		copyData((TMatrix *)cp.m_data);
	}

	// Destructor
	~matrix() {
		// empty for now
	}

	RESULT Numbers(TMatrix start, TMatrix increment) {
		clear();

		for (int i = 0; i < N * M; i++)
			m_data[i] = start + i * increment;

		return R_PASS;
	}

	RESULT NumbersByElement(TMatrix start, TMatrix increment) {
		for (int i = 0; i < N; i++)
			for (int j = 0; j < M; j++) 
				this->element(i, j) = start + ((i * N) + j) * increment;

		return R_PASS;
	}

	RESULT PrintMatrix() {
		RESULT r = R_PASS;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				DEBUG_OUT("%02f ", m_data[(j * N) + i]);
			}
			DEBUG_LINEOUT("");
		}

	Error:
		return r;
	}

public:
	// Proxy object passed back from look up
	class MatrixProxyObject {
	public:
		MatrixProxyObject(TMatrix* pProxyArray) :
			m_pProxyArray(pProxyArray) 
		{ 
			// Empty - Proxy object
		}

		// TODO: Check bounds
		TMatrix& operator[](const int index) {
			return m_pProxyArray[index];
		}
	private:
		TMatrix *m_pProxyArray;
	};

	// TODO: Check bounds
	MatrixProxyObject& operator[](const int index) {
		int lookUpValue = index * M;
		MatrixProxyObject retProxy(&(m_data[lookUpValue]));
		return retProxy;
	}

public:
	TMatrix* getData() { return m_data; }


#ifdef RANGE_CHECK
    RESULT rangeCheck( unsigned i, unsigned j ) const {
        if( i < rows())
            return R_MATRIX_ROW_OUT_OF_RANGE;
        if( j < cols() )
            return R_MATRIX_COL_OUT_OF_RANGE;

        return R_OK;
    }
#endif

    // For a [N1 x M1] * [N2 x M2] matrix multiplication M1 needs to equal N2
    inline RESULT checkMultDimensions(MatrixBase<TMatrix>& arg) {
        if(cols() != arg.rows())
            return E_MATRIX_MULT_DIMENSION_MISMATCH;

        return R_OK;
    }

    inline void copyData(TMatrix *data) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] = data[i];
    }

    inline void addData(TMatrix *data) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] += data[i];
    }

    inline void subData(TMatrix *data) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] -= data[i];
    }

    inline void multData(const TMatrix& a) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] *= a;
    }

    inline void divData(const TMatrix& a) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] /= a;
    }

    // Look up
    // -------------------------------------------------------------------------
public:
	TMatrix& operator()(unsigned i, unsigned j) {
        #ifdef RANGE_CHECK
            rangeCheck(i,j);
        #endif
		return m_data[(j * N) + i];
     }

     const TMatrix& operator()( unsigned i, unsigned j ) const {
        #ifdef RANGE_CHECK
            rangeCheck(i,j);
        #endif
		return m_data[(j * N) + i];
     }

     inline const TMatrix& element(unsigned i, unsigned j) const {
        #ifdef RANGE_CHECK
            rangeCheck(i,j);
        #endif
		return m_data[(j * N) + i];
     }

	 inline TMatrix& element(unsigned i, unsigned j) {
        #ifdef RANGE_CHECK
            rangeCheck(i, j);
        #endif
        return m_data[(j * N) + i];
     }

    // Assignment Operators
    // -------------------------------------------------------------------------
	
    // Copy
	 /*
    matrix& matrix::operator=(const matrix<TMatrix, N, M>& arg) {
        memcpy(m_data, arg.m_data, sizeof(TMatrix) * N * M);
        return *this;
    }
	*/

	 matrix<TMatrix, N, M>& operator=(const matrix<TMatrix, N, M> &arg) {
		 if (this == &arg)      // Same object?
			 return *this;        // Yes, so skip assignment, and just return *this.

		memcpy(this->m_data, arg.m_data, sizeof(TMatrix) * N * M);

		return *this;
	}

    // Move
	/*
    matrix& matrix::operator=(matrix<TMatrix, N, M>&& arg) {
        //assert(this != &arg);   // TODO: Asserts / EHM
        memcpy(m_data, arg.m_data, sizeof(TMatrix) * N * M);
        return *this;
    }

	/*
    // comparison
    bool operator==( const matrix<TMatrix, N, M>& arg) const {
        for(int i = 0; i < N * M; i++)
            if(m_data[i] != arg[i])
                return false;

        return true;
    }

    bool IsZero() const {
        for(int i = 0; i < N * M; i++)
            if(m_data[i] != 0)
                return false;

        return true;
    }

    bool operator!() const {
        return isZero();
    }
	*/

    // Arithmetic Operators
    // -------------------------------------------------------------------------
	matrix<TMatrix, N, M>& operator+=(const matrix<TMatrix, N, M>& rhs) {
        addData((TMatrix*)rhs.m_data);
        return *this;
    }

	// TODO: Is this the right way?
	friend matrix<TMatrix, N, M> operator+(const matrix<TMatrix, N, M>& lhs, const matrix<TMatrix, N, M>& rhs) {
		//lhs += rhs; // reuse compound assignment
		matrix newMatrix = lhs;
		newMatrix.addData((TMatrix*)rhs.m_data);
		return newMatrix;
	}

	/*
    const matrix<TMatrix, N, M>& operator+( const matrix<TMatrix, N, M>&arg ) const {
        return matrix<TMatrix, N, M>(*this).operator+=(arg);
    }
	//*/

	matrix<TMatrix, N, M>& operator-=(const matrix<TMatrix, N, M>& rhs) {
		subData((TMatrix*)rhs.m_data);
		return *this;
	}

	// TODO: Is this the right way?
	friend matrix<TMatrix, N, M> operator-(const matrix<TMatrix, N, M>& lhs, const matrix<TMatrix, N, M>& rhs) {
		//lhs -= rhs; // reuse compound assignment
		matrix newMatrix = lhs;
		newMatrix.subData((TMatrix*)rhs.m_data);
		return newMatrix;
	}

	/*
	matrix<TMatrix, N, M>& operator-( const matrix<TMatrix, N, M>&arg ) const {
	return matrix<TMatrix, N, M>(*this).operator-=(arg);
	}
	*/

	/*
	matrix<TMatrix, N, M>& operator*=(const matrix<TMatrix, N, M>& arg) {
		DEBUG_LINEOUT("WARN: LinAlgLib doesn't support %d x %d mult by %d x %d matrix", N, M, arg.rows(), arg.cols());
		return (*this);
	}

	const matrix<TMatrix, N, M> operator*(const matrix<TMatrix, N, M>& arg) const {
		return MatrixBase<TMatrix, N, M>(this*) *= arg;
	}
	*/

	// TODO: Is this the right way?
	// TODO: Clean up / consolidate code
	/*
	friend matrix<TMatrix, N, M> operator*(const matrix<TMatrix, N, M>& lhs, const matrix<TMatrix, N, M>& rhs) {
		matrix newMatrix = lhs;
		DEBUG_LINEOUT("WARN: LinAlgLib doesn't support %d x %d mult by %d x %d matrix", lhs.rows(), lhs.cols(), rhs.rows(), rhs.cols());
		return newMatrix;
	}
	*/

	/*

    // Scalar multiplication / Division
    // -------------------------------------------------------------------------
    matrix& operator*=( const TMatrix& a ) {
        multData(a);
        return *this;
    }

    matrix operator*( const TMatrix& a ) const {
        return matrix<TMatrix, N, M>(*this).operator*=(a);
    }

    matrix& operator/=( const TMatrix& a ) {
        divData(a);
        return *this;
    }

    matrix<TMatrix> operator/( const TMatrix& a ) {
        return matrix<TMatrix, N, M>(*this).operator/=(a);
    }

	*/

	// Matrix multiplication
	// This needs to be explicit due to N, M templating 
	// -------------------------------------------------------------------------   

	// 4X4 Matrix
	


	/*
    MatrixBase<TMatrix> operator*( const MatrixBase<TMatrix>& arg) const {
        ARM(checkMultDimensions(arg));

		matrix<TMatrix, N, arg.cols()> result;


		result.clear();

        for(int i = 0; i < N; i++) {
            for(int j = 0; i < arg.cols(); j++) {
				for (int k = 0; k < M; k++)
					result->element(i, j) += this->element(i, k) * arg
            }
        }
    }
	*/

	/*
    MatrixBase<T>& operator*=( const matrix<T, N, M>& arg ) {
        return *this = *this * arg;
    }
	*/


	/*
    // Matrix Division
    // -------------------------------------------------------------------------
    matrix<T> leftDivide( const matrix<T>& arg) const {
        // TODO:
    }
    matrix<T> rightDivide( const matrix<T>& arg) const {
        return transpose().leftDivide(arg.transpose()).transpose();
    }

    matrix<T> operator/( const matrix<T>& D ) const {
        return rightDivide(D);
    }

    matrix<T>& operator/=( const matrix<T>& M ) {
        return *this = *this/M;
    }

    // Determinants
    // -------------------------------------------------------------------------
    matrix<TMatrix> minor( unsigned i, unsigned j ) const {
        // TODO:
    }

	TMatrix Determinant() const {
        // TODO:
    }

	TMatrix MinorDeteterminant( unsigned i, unsigned j ) const {
        // TODO:
    }

    // Only valid for squares
    matrix<TMatrix> inverse() const {
        // TODO:
    }

    matrix<TMatrix> pow(int exp) const {
        // TODO:
    }

    matrix<TMatrix> identity() const {
        // TODO:
    }

    bool isIdentity() const {
        // TODO:
    }

    // Vector operations
    matrix<TMatrix> getRow(unsigned j) const {
        // TODO:
    }

    matrix<TMatrix> getCol(unsigned i) const {
        // TODO:
    }

    matrix<TMatrix>& setCol(unsigned j, const matrix<TMatrix>& arg) {
        // TODO:
    }

    matrix<TMatrix>& setRow(unsigned i, const matrix<TMatrix>& arg) {
        // TODO:
    }

    matrix<TMatrix> deleteRow(unsigned i) const {
        // TODO:
    }

    matrix<TMatrix> deleteCol(unsigned j) const {
        // TODO:
    }

    matrix<TMatrix, N, M> transpose() const {
        // TODO:
    }

    matrix<TMatrix, N, M> operator~() const {
        return transpose();
    }
	*/
};

// Partial Multiplication Specialization for 4x4 Matrix

// Square matrix, should work for all N
template <typename TMat4x4, int N>
matrix<TMat4x4, N, N> operator*(const matrix<TMat4x4, N, N>& lhs, const matrix<TMat4x4, N, N>& rhs) {
	matrix<TMat4x4, N, N> result;
	result.clear();

	// TODO: Find a way to reuse this
	for (int i = 0; i < N; i++) 
		for (int j = 0; j < N; j++) 
			for (int k = 0; k < N; k++)
				result.element(i, j) += lhs.element(i, k) * rhs.element(k, j);

	return result;
}

// N x M multiplied by M x 1, should work for all N, M
template <typename TMat4x4, int N, int M>
matrix<TMat4x4, N, 1> operator*(const matrix<TMat4x4, N, M>& lhs, const matrix<TMat4x4, M, 1>& rhs) {
	matrix<TMat4x4, N, 1> result;
	result.clear();

	// TODO: Find a way to reuse this
	for (int i = 0; i < N; i++) 
		for (int k = 0; k < M; k++)
			result.element(i, j) += lhs.element(i, k) * rhs.element(k, 0);

	return result;
}

// N x M multiplied by M x W, should work for all N, M, W
template <typename TMat4x4, int N, int M, int W>
matrix<TMat4x4, N, W> operator*(const matrix<TMat4x4, N, M>& lhs, const matrix<TMat4x4, M, W>& rhs) {
	matrix<TMat4x4, N, W> result;
	result.clear();

	// TODO: Find a way to reuse this
	for (int i = 0; i < N; i++)
		for (int j = 0; j < W; j++)
			for (int k = 0; k < M; k++)
				result.element(i, j) += lhs.element(i, k) * rhs.element(k, j);

	return result;
}

/*
template <typename TMat4x4>
matrix<TMat4x4, 4, 4>& matrix<TMat4x4, 4, 4>::operator*=(const matrix<TMat4x4, 4, 4>& arg) {
	matrix<TMat4x4, N, M> result;
	result.clear();

	// TODO: Find a way to reuse this
	for (int i = 0; i < N; i++) {
		for (int j = 0; i < arg.cols(); j++) {
			for (int k = 0; k < M; k++)
				result->element(i, j) += this->element(i, k) * arg
		}
	}

	return result;
}

template <typename TMat4x4>
const matrix<TMat4x4, 4, 4> matrix<TMat4x4, 4, 4>::operator*(const matrix<TMat4x4, 4, 4>& arg) const {
	return MatrixBase<TMat4x4, 4, 4>(this*) *= arg;
}

// 4X1 Matrix - returns 4x1
template <typename TMat4x4>
matrix<TMat4x4, 4, 1>& matrix<TMat4x4, 4, 4>::operator*=(const matrix<TMat4x4, 4, 1>& arg) {
	matrix<TMat4x4, N, 1> result;
	result.clear();

	for (int i = 0; i < N; i++) {
		for (int j = 0; i < arg.cols(); j++) {
			for (int k = 0; k < M; k++)
				result->element(i, j) += this->element(i, k) * arg
		}
	}

	return result;
}

template <typename TMat4x4>
const matrix<TMat4x4, 4, 1> matrix<TMat4x4, 4, 4>::operator*(const matrix<TMat4x4, 4, 1>& arg) const {
	return MatrixBase<TMatrix, 4, 1>(this*) *= arg;
}

/*
template <typename TMat4x4>
class matrix<TMat4x4, 4, 4> {
public:
	// Specialized 4x4 handling - returns 4x1
	matrix<TMat4x4, 4, 4>& operator*=(const matrix<TMat4x4, 4, 4>& arg) {
		matrix<TMat4x4, N, M> result;
		result.clear();

		// TODO: Find a way to reuse this
		for (int i = 0; i < N; i++) {
			for (int j = 0; i < arg.cols(); j++) {
				for (int k = 0; k < M; k++)
					result->element(i, j) += this->element(i, k) * arg
			}
		}

		return result;
	}

	const matrix<TMat4x4, 4, 4> operator*(const matrix<TMat4x4, 4, 4>& arg) const {
		return MatrixBase<TMat4x4, 4, 4>(this*) *= arg;
	}

	// 4X1 Matrix - returns 4x1
	matrix<TMat4x4, 4, 1>& operator*=(const matrix<TMat4x4, 4, 1>& arg) {
		matrix<TMat4x4, N, 1> result;
		result.clear();

		for (int i = 0; i < N; i++) {
			for (int j = 0; i < arg.cols(); j++) {
				for (int k = 0; k < M; k++)
					result->element(i, j) += this->element(i, k) * arg
			}
		}

		return result;
	}

	const matrix<TMat4x4, 4, 1> operator*(const matrix<TMat4x4, 4, 1>& arg) const {
		return MatrixBase<TMatrix, 4, 1>(this*) *= arg;
	}
};
*/

#endif MATRIX_H_
