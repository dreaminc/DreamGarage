#ifndef REFLECTION_MATRIX_H_
#define REFLECTION_MATRIX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/matrix/ReflectionMatrix.h
// Reflection Matrix 
// A general purpose reflection matrix

#include "matrix.h"

// TODO: MOVE ALL OF THESE TO CONFIG - Use central type
#ifdef FLOAT_PRECISION
	typedef float reflect_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double reflect_precision;
#endif

class point;
class vector;
class plane;

class ReflectionMatrix : public matrix<reflect_precision, 4, 4> {

public:
	
	ReflectionMatrix(point pt, vector vNormal);
	ReflectionMatrix(plane reflectionPlane);

	~ReflectionMatrix();

	RESULT PrintMatrix();	
	
private:
	RESULT CalculateMatrix(point pt, vector vNormal);
};

#endif // ! REFLECTION_MATRIX_H_
