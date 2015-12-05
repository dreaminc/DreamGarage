#ifndef MATRIX_H_
#define MATRIX_H_

// DREAM OS
// DreamOS/Dimension/Primitives/matrix.h
// Matrix Primitive Object

#include "EHM.h"

#define RANGE_CHECK

template <typename T>
virtual class MatrixBase {
public:
    virtual int rows() = 0;
    virtual int cols() = 0;

    virtual T& operator()(unsigned i, unsigned j) = 0;
    virtual const T& operator()( unsigned i, unsigned j ) const = 0;
    virtual const T& element(unsigned i, unsigned j) const = 0;
    virtual T& element(unsigned i, unsigned j) = 0;

protected:
    virtual T* getData() = 0;
};

template <typename T, int N = 4, int M = 4>
class matrix : public MatrixBase<T>
{

private:
    T m_data[N * M];

public:
    int rows() { return N; }
    int cols() { return M; }


protected:
    T* getData() { return m_data; }

    RESULT rangeCheck( unsigned i, unsigned j ) const {
        if( rows() <= i )
            return E_MATRIX_ROW_OUT_OF_RANGE;
        if( cols() <= j )
            return E_MATRIX_COL_OUT_OF_RANGE;

        return R_OK;
    }

    // For a [N1 x M1] * [N2 x M2] matrix multiplication M1 needs to equal N2
    inline RESULT checkMultDimensions(MatrixBase<T>& arg) {
        if(cols() != arg.rows())
            return E_MATRIX_MULT_DIMENSION_MISMATCH;

        return R_OK;
    }

    inline void copyData(T *data) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] = data[i];
    }

    inline void addData(T *data) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] += data[i];
    }

    inline void subData(T *data) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] += data[i];
    }

    inline void multData(const T& a) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] *= a;
    }

    inline void divData(const T& a) {
        for(int i = 0; i < (N * M); i++)
            m_data[i] /= a;
    }

public:
    // Constructors
    matrix() {
        /* stub */
    }

    matrix( const matrix<T, N, M> &cp ) {
        copyData(cp.m_data);
    }

    // Destructor
    ~matrix();

    // Look up
    // -------------------------------------------------------------------------
    T& operator()(unsigned i, unsigned j) {
        #ifdef RANGE_CHECK
            rangeCheck(i,j);
        #endif
        return m_data[i * N + M];
     }

     const T& operator()( unsigned i, unsigned j ) const {
        #ifdef RANGE_CHECK
            rangeCheck(i,j);
        #endif
        return m_data[i * N + M];
     }

     const T& element(unsigned i, unsigned j) const {
        #ifdef RANGE_CHECK
            rangeCheck(i,j);
        #endif
        return m_data[i * N + M];
     }

     T& element(unsigned i, unsigned j) {
        #ifdef RANGE_CHECK
            rangeCheck(i,j);
        #endif
        return m_data[i * N + M];
     }

    // Assignment Operators
    // -------------------------------------------------------------------------

    // Copy
    matrix& matrix::operator=(const matrix<T, N, M>& arg) {
        memcpy(&m_data, arg.m_data, sizeof(T) * N * M);
        return *this;
    }

    // Move
    matrix& matrix::operator=(matrix<T, N, M>&& arg) {
        assert(this != &arg);   // TODO: Asserts / EHM
        memcpy(&m_data, arg.m_data, sizeof(T) * N * M);
        return *this;
    }

    // comparison
    bool operator==( const matrix<T, N, M>& arg) const {
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

    matrix<T, N, M> operator+( const matrix<T, N, M>&arg ) const {
        return matrix<T>(*this).operator+=(arg);
    }

    matrix& operator-=(const matrix& rhs) {
        subData(rhs.m_data);
        return *this;
    }

    friend matrix operator-(matrix lhs, const matrix& rhs) {
        lhs += rhs; // reuse compound assignment
        return lhs;
    }

    matrix<T, N, M> operator-( const matrix<T, N, M>&arg ) const {
        return matrix<T, N, M>(*this).operator-=(arg);
    }

    // Scalar multiplication / Division
    // -------------------------------------------------------------------------
    matrix& operator*=( const T& a ) {
        multData(a);
        return *this;
    }

    matrix operator*( const T& a ) const {
        return matrix<T, N, M>(*this).operator*=(a);
    }

    matrix& operator/=( const T& a ) {
        divData(a);
        return *this;
    }

    matrix<T> operator/( const T& a ) {
        return matrix<T, N, M>(*this).operator/=(a);
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
    */

    // Determinants
    // -------------------------------------------------------------------------
    matrix<T> minor( unsigned i, unsigned j ) const {
        // TODO:
    }

    T Determinant() const {
        // TODO:
    }

    T MinorDeteterminant( unsigned i, unsigned j ) const {
        // TODO:
    }

    // Only valid for squares
    matrix<T> inverse() const {
        // TODO:
    }

    matrix<T> pow(int exp) const {
        // TODO:
    }

    matrix<T> identity() const {
        // TODO:
    }

    bool isIdentity() const {
        // TODO:
    }

    // Vector operations
    matrix<T> getRow(unsigned j) const {
        // TODO:
    }

    matrix<T> getCol(unsigned i) const {
        // TODO:
    }

    matrix<T>& setCol(unsigned j, const matrix<T>& arg) {
        // TODO:
    }

    matrix<T>& setRow(unsigned i, const matrix<T>& arg) {
        // TODO:
    }

    matrix<T> deleteRow(unsigned i) const {
        // TODO:
    }

    matrix<T> deleteCol(unsigned j) const {
        // TODO:
    }

    matrix<T, N, M> transpose() const {
        // TODO:
    }

    matrix<T, N, M> operator~() const {
        return transpose();
    }
}

#endif MATRIX_H_
