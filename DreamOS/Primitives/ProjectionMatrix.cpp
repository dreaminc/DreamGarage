#include "ProjectionMatrix.h"

ProjectionMatrix::ProjectionMatrix(PROJECTION_MATRIX_TYPE type, projection_precision width,
																projection_precision height,
																projection_precision nearPlane,
																projection_precision farPlane,
																projection_precision angle
) :
	m_type(type)
{
	switch (type) {
		case PROJECTION_MATRIX_PERSPECTIVE: {
			ACRM(SetPerspective(width, height, nearPlane, farPlane, angle), "Failed to set perspective matrix");
		} break;

		case PROJECTION_MATRIX_ORTHOGRAPHIC: {
			ACRM(SetOrthographic(width, height, nearPlane, farPlane), "Failed to set perspective matrix");
		} break;
	}
}

ProjectionMatrix::ProjectionMatrix(projection_precision left, projection_precision right,
								   projection_precision top, projection_precision bottom,
								   projection_precision nearPlane, projection_precision farPlane)
{
	ACRM(SetPerspective(left, right, top, bottom, nearPlane, farPlane), "Failed to set perspective matrix");
}

ProjectionMatrix::~ProjectionMatrix() {
	// Empty Stub
}

// https://solarianprogrammer.com/2013/05/22/opengl-101-matrices-projection-view-model/
// https://unspecified.wordpress.com/2012/06/21/calculating-the-gluperspective-matrix-and-other-opengl-matrix-maths/
RESULT ProjectionMatrix::SetPerspective(projection_precision width, 
										projection_precision height, 
										projection_precision nearPlane, 
										projection_precision farPlane, 
										projection_precision angle) 
{
	RESULT r = R_PASS;

	this->clear();

	projection_precision ratio = width / height;
	projection_precision top = nearPlane * static_cast<projection_precision>(((M_PI / 180.0f) * (angle / 2.0f)));
	projection_precision bottom = -top;
	projection_precision right = top * ratio;
	projection_precision left = -right;

	projection_precision f = 1.0f / static_cast<projection_precision>((tan((M_PI / 180.0f) * (angle / 2.0f))));

	/*
	this->element(0, 0) = (2 * nearPlane) / (right - left);
	this->element(1, 1) = (2 * nearPlane) / (top - bottom);
	//*/

	///*
	this->element(0, 0) = f / ratio;
	
	this->element(1, 1) = f;
	//*/

	this->element(2, 2) = -((farPlane + nearPlane) / (farPlane - nearPlane));
	this->element(2, 3) = -((2.0f*farPlane*nearPlane) / (farPlane - nearPlane));

	this->element(3, 2) = (-1.0f);

	m_type = PROJECTION_MATRIX_PERSPECTIVE;

	return r;
}

//http://www.songho.ca/opengl/gl_projectionmatrix.html
RESULT ProjectionMatrix::SetPerspective(projection_precision left, projection_precision right,
										projection_precision top, projection_precision bottom,
										projection_precision nearPlane,
										projection_precision farPlane)
{
	RESULT r = R_PASS;
	
	this->clear();

	
	this->element(0, 0) = (2.0f * nearPlane) / (right - left);
	this->element(2, 0) = (right + left) / (right - left);

	this->element(1, 1) = (2.0f * nearPlane) / (top - bottom);
	this->element(1, 2) = (top + bottom) / (top - bottom);

	this->element(2, 2) = -((farPlane + nearPlane) / (farPlane - nearPlane));
	this->element(2, 3) = -((2.0f*farPlane*nearPlane) / (farPlane - nearPlane));

	this->element(3, 2) = (-1.0f);

	m_type = PROJECTION_MATRIX_PERSPECTIVE;

	return r;
}

// http://www.songho.ca/opengl/gl_projectionmatrix.html
RESULT ProjectionMatrix::SetOrthographic(projection_precision width, 
										 projection_precision height, 
										 projection_precision nearPlane, 
										 projection_precision farPlane) 
{
	RESULT r = R_PASS;

	m_type = PROJECTION_MATRIX_ORTHOGRAPHIC;

	this->clear();

	projection_precision ratio = width / height;
	

	this->element(0, 0) = 1.0f / ratio;
	
	this->element(1, 1) = 1.0f;

	this->element(2, 2) = -2.0f / (farPlane - nearPlane);
	this->element(2, 3) = (-(farPlane + nearPlane)) / (farPlane - nearPlane);

	this->element(3, 3) = 1.0f;

	return r;
}

const char *ProjectionMatrix::StringProjectionMatrixType() {
	switch (m_type) {
		case PROJECTION_MATRIX_PERSPECTIVE: return "Perspective"; break;
		case PROJECTION_MATRIX_ORTHOGRAPHIC: return "Orthographic"; break;
	}

	return "Invalid";
}

RESULT ProjectionMatrix::PrintMatrix() {
	DEBUG_LINEOUT("%s Projection Matrix", StringProjectionMatrixType());
	return matrix<projection_precision, 4, 4>::PrintMatrix();
}