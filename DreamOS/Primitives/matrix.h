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
    //virtual static int rows() = 0;
    //virtual static int cols() = 0;

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

protected:
    TMatrix m_data[N * M];

public:
    static int rows() { return N; }
    static int cols() { return M; }
	
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

protected:
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
            m_data[i] += data[i];
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
	/*
    // Copy
    matrix& matrix::operator=(const matrix<TMatrix, N, M>& arg) {
        memcpy(&m_data, arg.m_data, sizeof(T) * N * M);
        return *this;
    }

    // Move
    matrix& matrix::operator=(matrix<T, N, M>&& arg) {
        //assert((this != &arg));   // TODO: Asserts / EHM
        memcpy(this->m_data, arg.m_data, sizeof(T) * N * M);
        return *this;
    }

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

    // Arithmetic Operators
    // -------------------------------------------------------------------------
    matrix& operator+=(const matrix& rhs) {
        addData(rhs.m_data);
        return *this;
    }

    friend matrix operator+(matrix lhs, const matrix& rhs) {
        lhs += rhs; // reuse compound assignment
        return lhs;
    }

    matrix<TMatrix, N, M> operator+( const matrix<TMatrix, N, M>&arg ) const {
        return matrix<TMatrix>(*this).operator+=(arg);
    }

    matrix& operator-=(const matrix& rhs) {
        subData(rhs.m_data);
        return *this;
    }

    friend matrix operator-(matrix lhs, const matrix& rhs) {
        lhs += rhs; // reuse compound assignment
        return lhs;
    }

    matrix<TMatrix, N, M> operator-( const matrix<TMatrix, N, M>&arg ) const {
        return matrix<TMatrix, N, M>(*this).operator-=(arg);
    }

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

    // Matrix multiplication
    // -------------------------------------------------------------------------
    /* TODO: This may require manually mapping handlers for different dimensions
             in order to maintain static array design
             
    MatrixBase<T> operator*( const MatrixBase<T>& arg) const {
        ARM(checkMultDimensions(arg));

        matrix<T, N, arg.cols>

        for(int n = 0; i < N; n++) {
            for(int m = 0; i < arg.cols(); m++) {
                this->element()
            }
        }
    }

    MatrixBase<T>& operator*=( const matrix<T, N, M>& arg ) {
        return *this = *this * arg;
    }

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

#endif MATRIX_H_
