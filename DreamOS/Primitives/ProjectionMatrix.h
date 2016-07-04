#ifndef PROJECTION_MATRIX_H_
#define PROJECTION_MATRIX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/ProjectionMatrix.h
// Projection Matrix 
// For purposes of DreamOS a projection matrix and all graphics types of dimension 4
// using float precision 

#include "matrix.h"

typedef enum {
	PROJECTION_MATRIX_PERSPECTIVE,
	PROJECTION_MATRIX_ORTHOGRAPHIC,
	PROJECTION_MATRIX_INVALID
} PROJECTION_MATRIX_TYPE;

#ifdef FLOAT_PRECISION
	typedef float projection_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double projection_precision;
#endif

class ProjectionMatrix : public matrix<projection_precision, 4, 4> {
public:
	ProjectionMatrix() :
		m_type(PROJECTION_MATRIX_INVALID)
	{
		clear();
	}

	ProjectionMatrix(PROJECTION_MATRIX_TYPE type, projection_precision width, 
												  projection_precision height, 
												  projection_precision nearPlane, 
												  projection_precision farPlane, 
												  projection_precision angle);

	ProjectionMatrix(projection_precision left, projection_precision right,
					 projection_precision top, projection_precision bottom,
					 projection_precision nearPlane, projection_precision farPlane);

	~ProjectionMatrix();

	RESULT PrintMatrix();
	const char *StringProjectionMatrixType();

private:
	RESULT SetPerspective(projection_precision width, projection_precision height, projection_precision nearPlane, projection_precision farPlane, projection_precision angle);
	RESULT SetPerspective(projection_precision left, projection_precision right, projection_precision top, projection_precision bottom, projection_precision nearPlane, projection_precision farPlane);
	RESULT SetOrthographic(projection_precision width, projection_precision height, projection_precision nearPlane, projection_precision farPlane);

private:
	PROJECTION_MATRIX_TYPE m_type;
};

#endif // ! PROJECTION_MATRIX_H_
