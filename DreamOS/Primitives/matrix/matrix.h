#ifndef MATRIX_H_
#define MATRIX_H_

// DREAM OS
// DreamOS/Dimension/Primitives/matrix.h
// Matrix Primitive Object

#include "RESULT/EHM.h"
#include <cstring>
#include <math.h>
#include <stdlib.h>     
#include <time.h>       
#include <array>

//#define RANGE_CHECK 
#define MATRIX_ROW_MAJOR
#ifndef MATRIX_ROW_MAJOR
	#define MATRIX_COLUMN_MAJOR
#endif

// TODO: Not sure how best to set this (external config, or based on data type?)
#define MAX_MATRIX_INVERSE_ERROR 0.0001f

// BIG TODO: Create Matrix testing suite

// TODO: Implement Kahan addition flag for better precision
// https://en.wikipedia.org/wiki/Kahan_summation_algorithm

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

	bool IsZero() {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; i++) {
				if (element(i, j) != 0) {
					return false;
				}
			}
		}

		return true;
	}

	TMatrix max() {
		TMatrix maxVal = m_data[0];
		
		for (int i = 0; i < (N * M); i++) {
			if (m_data[i] > maxVal)
				maxVal = m_data[i];
		}
		
		return maxVal;
	}

	TMatrix min() {
		TMatrix maxVal = m_data[0];

		for (int i = 0; i < (N * M); i++) {
			if (m_data[i] > maxVal)
				maxVal = m_data[i];
		}
		
		return maxVal;
	}

	// Sets up an identity matrix
	RESULT identity(TMatrix val = 1.0f) {
		RESULT r = R_PASS;

		// Ensure square matrix
		CBM((N == M), "Cant sent identity matrix on %dx%d dimensions", N, M);

		clear();

		for (int i = 0; i < N; i++) {
			this->element(i, i) = val;
		}

	Error:
		return r;
	}

	RESULT randomize(TMatrix maxval = 10.0f) {
		srand(time(nullptr));

		for (int i = 0; i < (N * M); i++) {
			float randval = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			m_data[i] = maxval * randval;
		}

		return R_PASS;
	}

	RESULT rangeByElement(TMatrix start = 0.0f) {
		TMatrix counter = start;

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				this->element(i, j) = counter; 
				counter += static_cast<TMatrix>(1.0f);
			}
		}

		return R_PASS;
	}

	RESULT range(TMatrix start = 0.0f) {
		for (int i = 0; i < (N * M); i++) {
			m_data[i] = start + static_cast<TMatrix>(i);
		}

		return R_PASS;
	}

	// TODO: Consolidate range vs 'numbers'
	RESULT Numbers(TMatrix start, TMatrix increment) {
		clear();

		for (int i = 0; i < N * M; i++) {
			m_data[i] = start + i * increment;
		}

		return R_PASS;
	}

	RESULT NumbersByElement(TMatrix start, TMatrix increment) {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				this->element(i, j) = start + ((i * N) + j) * increment;
			}
		}

		return R_PASS;
	}

	static matrix<TMatrix, N, M> MakeIdentity(TMatrix val = 1.0f) {
		matrix<TMatrix, N, M> retMatrix;
		retMatrix.identity(1.0f);
		return retMatrix;
	}

	static matrix<TMatrix, N, M> MakeRandom(TMatrix maxval = 10.0f) {
		matrix<TMatrix, N, M> retMatrix;
		retMatrix.randomize(maxval);
		return retMatrix;
	}

	static matrix<TMatrix, N, M> MakeRange(TMatrix start = 0.0f) {
		matrix<TMatrix, N, M> retMatrix;
		retMatrix.range(start);
		return retMatrix;
	}

	static matrix<TMatrix, N, M> MakeRangeByElement(TMatrix start = 0.0f) {
		matrix<TMatrix, N, M> retMatrix;
		retMatrix.rangeByElement(start);
		return retMatrix;
	}

	matrix<TMatrix, (N - 1), (M - 1)> minormatrix(unsigned i, unsigned j) {
		RESULT r = R_PASS;
		matrix<TMatrix, N - 1, M - 1> retMatrix;
		retMatrix.clear();

		int rowCount = 0;
		int colCount = 0;

		CRM(RangeCheck(i, j), "%d %d minor not possible in %d x %d matrix", i, j, N, M);

		for (int x = 0; x < (N - 1); x++) {
			
			if (rowCount == i) {
				rowCount += 1;
			}

			colCount = 0;
			for (int y = 0; y < (M - 1); y++) {

				if (colCount == j) {
					colCount += 1;
				}

				retMatrix.element(x, y) = element(rowCount, colCount);

				colCount += 1;
			}

			rowCount += 1;
		}

	Error:
		return retMatrix;
	}

	TMatrix minor(unsigned i, unsigned j) {
		matrix<TMatrix, N - 1, M - 1> retMatrix = minormatrix(i, j);
		return determinant(retMatrix);
	}

	matrix<TMatrix, 1, M> row(unsigned r) {
		matrix<TMatrix, 1, M> retMatrix;
		retMatrix.clear();

		for (int i = 0; i < M; i++) {
			retMatrix.element(0, i) = element(r, i);
		}

		return retMatrix;
	}

	matrix<TMatrix, N, 1> column(unsigned c) {
		matrix<TMatrix, N, 1> retMatrix;

		for (int i = 0; i < N; i++) {
			retMatrix.element(i, 0) = element(i, c);
		}

		return retMatrix;
	}

	// This is a zero based matrix, so adding one for the power (but -1^0 == 1 so the math should actually work regardless)
	TMatrix cofactor(unsigned i, unsigned j) {
		TMatrix signVal = pow(-1, ((i + 1) + (j + 1)));		
		return (signVal * minor(i, j));
	}

	RESULT RangeCheck(unsigned i, unsigned j) {
		ACBM((i < N), "i index greater than %d", N);
		ACBM((j < M), "j index greater than %d", M);

		return R_PASS;
	}


public:
	// Constructors
	matrix() {
		/* stub */
	}

	// Warning: This is a pretty vulnerable function
	matrix(TMatrix *pInitArray) {
		copyData((TMatrix *)pInitArray);
	}

	matrix(const std::array<TMatrix, N*M>& initArray) {
		copyData((TMatrix *)initArray.data());
	}

	matrix(const matrix<TMatrix, N, M> &cp) {
		copyData((TMatrix *)cp.m_data);
	}

	matrix(TMatrix diagVal) {
		identity(diagVal);
	}

	// Destructor
	~matrix() {
		// empty for now
	}

	RESULT SetMatrix(const matrix<TMatrix, N, M> &arg) {
		copyData((TMatrix *)arg.m_data);
		return R_PASS;
	}

	RESULT PrintMatrix() {
		DEBUG_LINEOUT("matrix %d x %d | N: %d  M:%d ", rows(), cols(), N, M);

		DEBUG_OUT("| ");

		for (int i = 0; i < (N * M); i++) {
			if((i != 0) && (i % M) == 0) {
				DEBUG_LINEOUT(" |");
				DEBUG_OUT("| ");
			}

			DEBUG_OUT("%02f ", m_data[i]);
		}

		DEBUG_LINEOUT("| ");

		return R_PASS;
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
		int lookUpValue = (index * M);
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
            return R_MATRIX_MULT_DIMENSION_MISMATCH;
		else
			return R_PASS;
    }

	inline bool IsSquare() {
		return (N == M);
	}

    inline void copyData(TMatrix *data) {
		for (int i = 0; i < (N * M); i++) {
			m_data[i] = data[i];
		}
    }

    inline void addData(TMatrix *data) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] += data[i];
    }

    inline void subData(TMatrix *data) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] -= data[i];
    }

    inline void divData(const TMatrix& a) {
		for (int i = 0; i < (N * M); i++) {
			m_data[i] /= a;
		}
    }

    // Look up
    // -------------------------------------------------------------------------
public:

#if defined(MATRIX_ROW_MAJOR)
	#define MATRIX_LOOK_UP(i, j) (m_data[(i * M) + j])
#else 
	#define MATRIX_LOOK_UP(i, j) (m_data[(j * N) + i])
#endif

	TMatrix& operator()(unsigned i) {
		#ifdef RANGE_CHECK
			rangeCheck(i);
		#endif

		return m_data[i];
	}

	TMatrix& operator()(unsigned i, unsigned j) {
        #ifdef RANGE_CHECK
            rangeCheck(i,j);
        #endif
		return MATRIX_LOOK_UP(i, j);
     }

     const TMatrix& operator()( unsigned i, unsigned j ) const {
        #ifdef RANGE_CHECK
            rangeCheck(i,j);
        #endif
		return MATRIX_LOOK_UP(i, j);
     }

     inline const TMatrix& element(unsigned i, unsigned j) const {
        #ifdef RANGE_CHECK
            rangeCheck(i,j);
        #endif
		return MATRIX_LOOK_UP(i, j);
     }

	 inline TMatrix& element(unsigned i, unsigned j) {
        #ifdef RANGE_CHECK
            rangeCheck(i, j);
        #endif
        return MATRIX_LOOK_UP(i, j);
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

	// Scalar multiplication / Division
	// -------------------------------------------------------------------------
	matrix<TMatrix, N, M>& operator*=(const TMatrix& a) {
		for (int i = 0; i < (N * M); i++)
			m_data[i] *= a;

		return *this;
	}

	matrix<TMatrix, N, M> operator*(const TMatrix& a) const {
		return matrix<TMatrix, N, M>(*this).operator*=(a);
	}

	matrix<TMatrix, N, M>& operator/=(const TMatrix& a) {
		divData(a);
		return *this;
	}

	matrix<TMatrix, N, M> operator/(const TMatrix& a) {
		return matrix<TMatrix, N, M>(*this).operator/=(a);
	}

	// This is specialized 
	//TMatrix determinant();
	//matrix<TMatrix, N, M> inverse();

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

// Scalar multiplication
// TODO: Fix const-ness?
template <typename TMat4x4, int N, int M>
matrix<TMat4x4, N, M> operator* (TMat4x4 val, matrix<TMat4x4, N, M>& arg) {
	matrix<TMat4x4, N, M> result(arg);
	return (result *= (val));
}

/*
template <typename TMat4x4, int N, int M>
matrix<TMat4x4, N, M> operator* (const matrix<TMat4x4, N, M>& arg, TMat4x4 val) {
	return arg.operator*=(val);
}
*/

// Matrix Multiplication

// Square matrix, should work for all N
template <typename TMat4x4, int N>
matrix<TMat4x4, N, N> operator*(const matrix<TMat4x4, N, N>& lhs, const matrix<TMat4x4, N, N>& rhs) {
	matrix<TMat4x4, N, N> result;
	result.clear();

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {
				result.element(i, j) += lhs.element(i, k) * rhs.element(k, j);
			}
		}
	}

	return result;
}

///*
// N x M multiplied by M x 1, should work for all N, M
// This is technically an optimized of the below
template <typename TMat4x4, int N, int M>
matrix<TMat4x4, N, 1> operator*(const matrix<TMat4x4, N, M>& lhs, const matrix<TMat4x4, M, 1>& rhs) {
	matrix<TMat4x4, N, 1> result;
	result.clear();

	for (int i = 0; i < N; i++) 
		for (int k = 0; k < M; k++)
			result.element(i, 0) += lhs.element(i, k) * rhs.element(k, 0);

	return result;
}
//*/

// N x M multiplied by M x W, should work for all N, M, W
template <typename TMat4x4, int N, int M, int W>
matrix<TMat4x4, N, W> operator*(const matrix<TMat4x4, N, M>& lhs, const matrix<TMat4x4, M, W>& rhs) {
	matrix<TMat4x4, N, W> result;
	result.clear();

	for (int i = 0; i < N; i++)
		for (int j = 0; j < W; j++)
			for (int k = 0; k < M; k++)
				result.element(i, j) += lhs.element(i, k) * rhs.element(k, j);

	return result;
}

// DOT PRODUCT
// Dot Product defined as when two N x 1 matrices are multiplied and this will return a scalar result
// Square matrix, should work for all N
// Warning: This is not suggested for calculating dot products for vectors, please use the vector dot and cross calls
template <typename TMat4x4, int N>
TMat4x4 operator*(const matrix<TMat4x4, N, 1>& lhs, const matrix<TMat4x4, N, 1>& rhs) {
	TMat4x4 result = 0;	

	for (int i = 0; i < N; i++)
		result += lhs.element(i, 0) * rhs.element(i, 0);

	return result;
}

// EQUALITY
template <typename TMat4x4, int N, int M>
bool operator==(const matrix<TMat4x4, N, M>& lhs, const matrix<TMat4x4, N, M>& rhs) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (lhs.element(i, j) != rhs.element(i, j))
				return false;
		}
	}
	return true;
}

template <typename TMat4x4, int N, int M>
bool operator!=(const matrix<TMat4x4, N, M>& lhs, const matrix<TMat4x4, N, M>& rhs) {
	return !(lhs==rhs);
}

// DETERMINANT

// Formulas from or derived based on: https://en.wikipedia.org/wiki/Determinant
template <typename TMat2x2>
TMat2x2 determinant(matrix<TMat2x2, 2, 2> mat) {
	return (mat.m_data[0] * mat.m_data[3]) - (mat.m_data[1] * mat.m_data[2]);
}

template <typename TMat3x3>
TMat3x3 determinant(matrix<TMat3x3, 3, 3> mat) {
	TMat3x3 result = -1;

	result = (mat.m_data[0] * mat.m_data[4] * mat.m_data[8]);
	result += (mat.m_data[1] * mat.m_data[5] * mat.m_data[6]);
	result += (mat.m_data[2] * mat.m_data[3] * mat.m_data[7]);

	result -= (mat.m_data[2] * mat.m_data[4] * mat.m_data[6]);
	result -= (mat.m_data[1] * mat.m_data[3] * mat.m_data[8]);
	result -= (mat.m_data[0] * mat.m_data[5] * mat.m_data[7]);

	return result;
}

// TODO: This could likely be generalized to a M M method
template <typename TMat4x4>
TMat4x4 determinant(matrix<TMat4x4, 4, 4> mat) {
	TMat4x4 determinantResult = 0.0f;

	for (int j = 0; j < 4; j++) {
		determinantResult += (mat.element(0, j) * mat.cofactor(0, j));
	}

	return determinantResult;
}

// http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
template <typename TMat2x2>
matrix<TMat2x2, 2, 2> inverse(matrix<TMat2x2, 2, 2> mat) {
	RESULT r = R_PASS;

	matrix<TMat2x2, 2, 2> retMatrix;
	retMatrix.clear();

	TMat2x2 matDeterminant = determinant(mat);
	CBM((matDeterminant != 0), "Matrix cannot be inverted, determinant is zero");
	
	// 0
	retMatrix.element(0, 0) = mat(1, 1);
	retMatrix.element(0, 1) = -1 * mat(0, 1);

	// 1
	retMatrix.element(1, 0) = -1 * mat(1, 0);
	retMatrix.element(1, 1) = mat(0, 0);

	retMatrix *= (1.0f / matDeterminant);

Error:
	return retMatrix;
}

// http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
template <typename TMat3x3>
matrix<TMat3x3, 3, 3> inverse(matrix<TMat3x3, 3, 3> mat) {
	RESULT r = R_PASS;

	matrix<TMat3x3, 3, 3> retMatrix;
	retMatrix.clear();

	TMat3x3 matDeterminant = determinant(mat);
	CBM((matDeterminant != 0), "Matrix cannot be inverted, determinant is zero");
	/*
	// TODO: hard coding suuuuuuuuuuuuuuuucks (but it works really well)
	(0, 0) = (1, 1)(2, 2) - (1, 2)(2, 1)
	(0, 1) = (0, 2)(2, 1) - (0, 1)(2, 2)
	(0, 2) = (0, 1)(1, 2) - (0, 2)(1, 1)
	(0, 0) = (1, 2)(2, 0) - (1, 0)(2, 2)
	(1, 1) = (0, 0)(2, 2) - (0, 2)(2, 0)
	(1, 2) = (0, 2)(1, 0) - (0, 0)(1, 2)
	(0, 0) = (1, 0)(2, 1) - (1, 1)(2, 0)
	(1, 1) = (0, 1)(2, 0) - (0, 0)(2, 1)
	(2, 2) = (0, 0)(1, 1) - (0, 1)(1, 0)
	*/
	
	retMatrix.element(0, 0) = mat(1, 1)*mat(2, 2) - mat(1, 2)*mat(2, 1);
	retMatrix.element(0, 1) = mat(0, 2)*mat(2, 1) - mat(0, 1)*mat(2, 2);
	retMatrix.element(0, 2) = mat(0, 1)*mat(1, 2) - mat(0, 2)*mat(1, 1);
	retMatrix.element(1, 0) = mat(1, 2)*mat(2, 0) - mat(1, 0)*mat(2, 2);
	retMatrix.element(1, 1) = mat(0, 0)*mat(2, 2) - mat(0, 2)*mat(2, 0);
	retMatrix.element(1, 2) = mat(0, 2)*mat(1, 0) - mat(0, 0)*mat(1, 2);
	retMatrix.element(2, 0) = mat(1, 0)*mat(2, 1) - mat(1, 1)*mat(2, 0);
	retMatrix.element(2, 1) = mat(0, 1)*mat(2, 0) - mat(0, 0)*mat(2, 1);
	retMatrix.element(2, 2) = mat(0, 0)*mat(1, 1) - mat(0, 1)*mat(1, 0);
	

	retMatrix *= (1.0f / matDeterminant);

Error:
	return retMatrix;
}

// http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
template <typename TMat4x4>
matrix<TMat4x4, 4, 4> inverse(matrix<TMat4x4, 4, 4> mat) {
	RESULT r = R_PASS;

	matrix<TMat4x4, 4, 4> retMatrix;
	retMatrix.clear();

	TMat4x4 matDeterminant = determinant(mat);
	CBM((matDeterminant != 0), "Matrix cannot be inverted, determinant is zero");

	// TODO: hard coding suuuuuuuuuuuuuuuucks
	/*
	// This is here to inspect the ordering / indices - use this as a template to redo the below
	(0,0) = (1,1)(2,2)(3,3) + (1,2)(2,3)(3,1) + (1,3)(2,1)(3,2) - (1,1)(2,3)(3,2) - (1,2)(2,1)(3,3) - (1,3)(2,2)(3,1)
	(0,1) = (0,1)(2,3)(3,2) + (0,2)(2,1)(3,3) + (0,3)(2,2)(3,1) - (0,1)(2,2)(3,3) - (0,2)(2,3)(3,1) - (0,3)(2,1)(3,2)
	(0,2) = (0,1)(1,2)(3,3) + (0,2)(1,3)(3,1) + (0,3)(1,1)(3,2) - (0,1)(1,3)(3,2) - (0,2)(1,1)(3,3) - (0,3)(1,2)(3,1)
	(0,3) = (0,1)(1,3)(2,2) + (0,2)(1,1)(2,3) + (0,3)(1,2)(2,1) - (0,1)(1,2)(2,3) - (0,2)(1,3)(2,1) - (0,3)(1,1)(2,2)
	(1,0) = (1,0)(2,3)(3,2) + (1,2)(2,0)(3,3) + (1,3)(2,2)(3,0) - (1,0)(2,2)(3,3) - (1,2)(2,3)(3,0) - (1,3)(2,0)(3,2)
	(1,1) = (0,0)(2,2)(3,3) + (0,2)(2,3)(3,0) + (0,3)(2,0)(3,2) - (0,0)(2,3)(3,2) - (0,2)(2,0)(3,3) - (0,3)(2,2)(3,0)
	(1,2) = (0,0)(1,3)(3,2) + (0,2)(1,0)(3,3) + (0,3)(1,2)(3,0) - (0,0)(1,2)(3,3) - (0,2)(1,3)(3,0) - (0,3)(1,0)(3,2)
	(1,3) = (0,0)(1,2)(2,3) + (0,2)(1,3)(2,0) + (0,3)(1,0)(2,2) - (0,0)(1,3)(2,2) - (0,2)(1,0)(2,3) - (0,3)(1,2)(2,0)
	(2,0) = (1,0)(2,1)(3,3) + (1,1)(2,3)(3,0) + (1,3)(2,0)(3,1) - (1,0)(2,3)(3,1) - (1,1)(2,0)(3,3) - (1,3)(2,1)(3,0)
	(2,1) = (0,0)(2,3)(3,1) + (0,1)(2,0)(3,3) + (0,3)(2,1)(3,0) - (0,0)(2,1)(3,3) - (0,1)(2,3)(3,0) - (0,3)(2,0)(3,1)
	(2,2) = (0,0)(1,1)(3,3) + (0,1)(1,3)(3,0) + (0,3)(1,0)(3,1) - (0,0)(1,3)(3,1) - (0,1)(1,0)(3,3) - (0,3)(1,1)(3,0)
	(2,3) = (0,0)(1,3)(2,1) + (0,1)(1,0)(2,3) + (0,3)(1,1)(2,0) - (0,0)(1,1)(2,3) - (0,1)(1,3)(2,0) - (0,3)(1,0)(2,1)
	(3,0) = (1,0)(2,2)(3,1) + (1,1)(2,0)(3,2) + (1,2)(2,1)(3,0) - (1,0)(2,1)(3,2) - (1,1)(2,2)(3,0) - (1,2)(2,0)(3,1)
	(3,1) = (0,0)(2,1)(3,2) + (0,1)(2,2)(3,0) + (0,2)(2,0)(3,1) - (0,0)(2,2)(3,1) - (0,1)(2,0)(3,2) - (0,2)(2,1)(3,0)
	(3,2) = (0,0)(1,2)(3,1) + (0,1)(1,0)(3,2) + (0,2)(1,1)(3,0) - (0,0)(1,1)(3,2) - (0,1)(1,2)(3,0) - (0,2)(1,0)(3,1)
	(3,3) = (0,0)(1,1)(2,2) + (0,1)(1,2)(2,0) + (0,2)(1,0)(2,1) - (0,0)(1,2)(2,1) - (0,1)(1,0)(2,2) - (0,2)(1,1)(2,0)
	*/
	
	retMatrix.element(0, 0) = mat(1, 1)*mat(2, 2)*mat(3, 3) + mat(1, 2)*mat(2, 3)*mat(3, 1) + mat(1, 3)*mat(2, 1)*mat(3, 2) - mat(1, 1)*mat(2, 3)*mat(3, 2) - mat(1, 2)*mat(2, 1)*mat(3, 3) - mat(1, 3)*mat(2, 2)*mat(3, 1);
	retMatrix.element(0, 1) = mat(0, 1)*mat(2, 3)*mat(3, 2) + mat(0, 2)*mat(2, 1)*mat(3, 3) + mat(0, 3)*mat(2, 2)*mat(3, 1) - mat(0, 1)*mat(2, 2)*mat(3, 3) - mat(0, 2)*mat(2, 3)*mat(3, 1) - mat(0, 3)*mat(2, 1)*mat(3, 2);
	retMatrix.element(0, 2) = mat(0, 1)*mat(1, 2)*mat(3, 3) + mat(0, 2)*mat(1, 3)*mat(3, 1) + mat(0, 3)*mat(1, 1)*mat(3, 2) - mat(0, 1)*mat(1, 3)*mat(3, 2) - mat(0, 2)*mat(1, 1)*mat(3, 3) - mat(0, 3)*mat(1, 2)*mat(3, 1);
	retMatrix.element(0, 3) = mat(0, 1)*mat(1, 3)*mat(2, 2) + mat(0, 2)*mat(1, 1)*mat(2, 3) + mat(0, 3)*mat(1, 2)*mat(2, 1) - mat(0, 1)*mat(1, 2)*mat(2, 3) - mat(0, 2)*mat(1, 3)*mat(2, 1) - mat(0, 3)*mat(1, 1)*mat(2, 2);
	retMatrix.element(1, 0) = mat(1, 0)*mat(2, 3)*mat(3, 2) + mat(1, 2)*mat(2, 0)*mat(3, 3) + mat(1, 3)*mat(2, 2)*mat(3, 0) - mat(1, 0)*mat(2, 2)*mat(3, 3) - mat(1, 2)*mat(2, 3)*mat(3, 0) - mat(1, 3)*mat(2, 0)*mat(3, 2);
	retMatrix.element(1, 1) = mat(0, 0)*mat(2, 2)*mat(3, 3) + mat(0, 2)*mat(2, 3)*mat(3, 0) + mat(0, 3)*mat(2, 0)*mat(3, 2) - mat(0, 0)*mat(2, 3)*mat(3, 2) - mat(0, 2)*mat(2, 0)*mat(3, 3) - mat(0, 3)*mat(2, 2)*mat(3, 0);
	retMatrix.element(1, 2) = mat(0, 0)*mat(1, 3)*mat(3, 2) + mat(0, 2)*mat(1, 0)*mat(3, 3) + mat(0, 3)*mat(1, 2)*mat(3, 0) - mat(0, 0)*mat(1, 2)*mat(3, 3) - mat(0, 2)*mat(1, 3)*mat(3, 0) - mat(0, 3)*mat(1, 0)*mat(3, 2);
	retMatrix.element(1, 3) = mat(0, 0)*mat(1, 2)*mat(2, 3) + mat(0, 2)*mat(1, 3)*mat(2, 0) + mat(0, 3)*mat(1, 0)*mat(2, 2) - mat(0, 0)*mat(1, 3)*mat(2, 2) - mat(0, 2)*mat(1, 0)*mat(2, 3) - mat(0, 3)*mat(1, 2)*mat(2, 0);
	retMatrix.element(2, 0) = mat(1, 0)*mat(2, 1)*mat(3, 3) + mat(1, 1)*mat(2, 3)*mat(3, 0) + mat(1, 3)*mat(2, 0)*mat(3, 1) - mat(1, 0)*mat(2, 3)*mat(3, 1) - mat(1, 1)*mat(2, 0)*mat(3, 3) - mat(1, 3)*mat(2, 1)*mat(3, 0);
	retMatrix.element(2, 1) = mat(0, 0)*mat(2, 3)*mat(3, 1) + mat(0, 1)*mat(2, 0)*mat(3, 3) + mat(0, 3)*mat(2, 1)*mat(3, 0) - mat(0, 0)*mat(2, 1)*mat(3, 3) - mat(0, 1)*mat(2, 3)*mat(3, 0) - mat(0, 3)*mat(2, 0)*mat(3, 1);
	retMatrix.element(2, 2) = mat(0, 0)*mat(1, 1)*mat(3, 3) + mat(0, 1)*mat(1, 3)*mat(3, 0) + mat(0, 3)*mat(1, 0)*mat(3, 1) - mat(0, 0)*mat(1, 3)*mat(3, 1) - mat(0, 1)*mat(1, 0)*mat(3, 3) - mat(0, 3)*mat(1, 1)*mat(3, 0);
	retMatrix.element(2, 3) = mat(0, 0)*mat(1, 3)*mat(2, 1) + mat(0, 1)*mat(1, 0)*mat(2, 3) + mat(0, 3)*mat(1, 1)*mat(2, 0) - mat(0, 0)*mat(1, 1)*mat(2, 3) - mat(0, 1)*mat(1, 3)*mat(2, 0) - mat(0, 3)*mat(1, 0)*mat(2, 1);
	retMatrix.element(3, 0) = mat(1, 0)*mat(2, 2)*mat(3, 1) + mat(1, 1)*mat(2, 0)*mat(3, 2) + mat(1, 2)*mat(2, 1)*mat(3, 0) - mat(1, 0)*mat(2, 1)*mat(3, 2) - mat(1, 1)*mat(2, 2)*mat(3, 0) - mat(1, 2)*mat(2, 0)*mat(3, 1);
	retMatrix.element(3, 1) = mat(0, 0)*mat(2, 1)*mat(3, 2) + mat(0, 1)*mat(2, 2)*mat(3, 0) + mat(0, 2)*mat(2, 0)*mat(3, 1) - mat(0, 0)*mat(2, 2)*mat(3, 1) - mat(0, 1)*mat(2, 0)*mat(3, 2) - mat(0, 2)*mat(2, 1)*mat(3, 0);
	retMatrix.element(3, 2) = mat(0, 0)*mat(1, 2)*mat(3, 1) + mat(0, 1)*mat(1, 0)*mat(3, 2) + mat(0, 2)*mat(1, 1)*mat(3, 0) - mat(0, 0)*mat(1, 1)*mat(3, 2) - mat(0, 1)*mat(1, 2)*mat(3, 0) - mat(0, 2)*mat(1, 0)*mat(3, 1);
	retMatrix.element(3, 3) = mat(0, 0)*mat(1, 1)*mat(2, 2) + mat(0, 1)*mat(1, 2)*mat(2, 0) + mat(0, 2)*mat(1, 0)*mat(2, 1) - mat(0, 0)*mat(1, 2)*mat(2, 1) - mat(0, 1)*mat(1, 0)*mat(2, 2) - mat(0, 2)*mat(1, 1)*mat(2, 0);

	retMatrix *= (1.0f / matDeterminant);

Error:
	return retMatrix;
}

// TODO: Add Transpose
template <typename TMat4x4, int N>
matrix<TMat4x4, N, N> transpose(matrix<TMat4x4, N, N> mat) {
	RESULT r = R_PASS;

	matrix<TMat4x4, 4, 4> retMatrix;
	retMatrix.clear();

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			retMatrix.element(i, j) = mat.element(j, i);
		}
	}

	return retMatrix;
}

template <typename TMat, int N, int M>
matrix<TMat, M, N> transpose(matrix<TMat, N, M> mat) {
	RESULT r = R_PASS;

	matrix<TMat, M, N> retMatrix;
	retMatrix.clear();

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			retMatrix.element(i, j) = mat.element(j, i);
		}
	}

	return retMatrix;
}

template <typename TMat4x4, int N, int M>
matrix<TMat4x4, N, M> absolute(matrix<TMat4x4, N, M> mat) {
	RESULT r = R_PASS;

	matrix<TMat4x4, N, M> retMatrix;
	retMatrix.clear();

	for (int i = 0; i < N * M; i++) {
		retMatrix(i) = (TMat4x4)(std::fabs((double)(mat(i))));
	}

	return retMatrix;
}

/*
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
*/

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
