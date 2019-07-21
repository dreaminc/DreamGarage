#ifndef PROJECTION_MATRIX_H_
#define PROJECTION_MATRIX_H_

#include "core/ehm/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/ProjectionMatrix.h
// Projection Matrix 
// For purposes of DreamOS a projection matrix and all graphics types of dimension 4
// using float precision 

#include "matrix.h"

#define DEFAULT_NEAR_PLANE 0.1f
#define DEFAULT_FAR_PLANE 10000.0f

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



	// Perspective 
	ProjectionMatrix(projection_precision left, projection_precision right,
					 projection_precision top, projection_precision bottom,
					 projection_precision nearPlane, projection_precision farPlane);

	ProjectionMatrix(projection_precision width, projection_precision height, 
					 projection_precision nearPlane, projection_precision farPlane, 
					 projection_precision angle);

	// Orthographic
	ProjectionMatrix(projection_precision width, projection_precision height, projection_precision nearPlane, projection_precision farPlane);

	~ProjectionMatrix();

	RESULT PrintMatrix();
	const char *StringProjectionMatrixType();

	static ProjectionMatrix MakeOrtho(projection_precision width, projection_precision height, 
									  projection_precision nearPlane, projection_precision farPlane);

	static ProjectionMatrix MakeOrtho(projection_precision left, projection_precision right, 
									  projection_precision top, projection_precision bottom, 
									  projection_precision nearPlane, projection_precision farPlane);

	static ProjectionMatrix MakeOrthoYAxis(projection_precision left, projection_precision right,
										   projection_precision top, projection_precision bottom,
										   projection_precision nearPlane, projection_precision farPlane);

private:
	RESULT SetPerspective(projection_precision width, projection_precision height, projection_precision nearPlane, projection_precision farPlane, projection_precision angle);
	RESULT SetPerspective(projection_precision left, projection_precision right, projection_precision top, projection_precision bottom, projection_precision nearPlane, projection_precision farPlane);

	RESULT SetOrthographic(projection_precision width, projection_precision height, projection_precision nearPlane, projection_precision farPlane);
	RESULT SetOrthographic(projection_precision left, projection_precision right, projection_precision top, projection_precision bottom, projection_precision nearPlane, projection_precision farPlane);
	RESULT SetOrthographicYAxis(projection_precision left, projection_precision right, projection_precision top, projection_precision bottom, projection_precision nearPlane, projection_precision farPlane);

private:
	PROJECTION_MATRIX_TYPE m_type;
};

#endif // ! PROJECTION_MATRIX_H_
