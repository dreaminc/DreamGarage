#ifndef PERSPECTIVE_MATRIX_H_
#define PERSPECTIVE_MATRIX_H_

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

// TODO: Template the type of the matrix?

class ProjectionMatrix : public matrix<float, 4, 4> {
public:
	ProjectionMatrix(PROJECTION_MATRIX_TYPE type, float width, float height, float nearPlane, float farPlane, float angle);
	~ProjectionMatrix();

	RESULT PrintMatrix();
	const char *StringProjectionMatrixType();

private:
	RESULT SetPerspective(float width, float height, float nearPlane, float farPlane, float angle);
	RESULT SetOrthographic(float width, float height, float nearPlane, float farPlane);

private:
	PROJECTION_MATRIX_TYPE m_type;
};

#endif // ! PERSPECTIVE_MATRIX_H_
