#include "ProjectionMatrix.h"

ProjectionMatrix::ProjectionMatrix(PROJECTION_MATRIX_TYPE type, float width, float height, float nearPlane, float farPlane, float angle) :
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

ProjectionMatrix::~ProjectionMatrix() {
	// Empty Stub
}

RESULT ProjectionMatrix::SetPerspective(float width, float height, float nearPlane, float farPlane, float angle) {
	RESULT r = R_PASS;

	float ratio = width / height;
	this->clear();
	this->element(0, 0) = 1.0f / (ratio * tan(angle / 2.0f));
	this->element(1, 1) = 1.0f / tan(angle / 2.0f);
	this->element(2, 2) = (-nearPlane - farPlane) / (nearPlane -farPlane);
	this->element(2, 3) = (1.0f);
	this->element(3, 2) = 2.0f * nearPlane *farPlane / (nearPlane - farPlane);

	m_type = PROJECTION_MATRIX_PERSPECTIVE;

Error:
	return r;
}

RESULT ProjectionMatrix::SetOrthographic(float width, float height, float nearPlane, float farPlane) {
	RESULT r = R_PASS;

	float ratio = width / height;
	this->clear();

	this->element(0, 0) = 2.0f / width;
	this->element(1, 1) = 2.0f / height;
	this->element(2, 2) = 1.0f / (farPlane - nearPlane);
	this->element(3, 2) = -nearPlane / (farPlane - nearPlane);
	this->element(3, 3) = 1.0f;

	m_type = PROJECTION_MATRIX_ORTHOGRAPHIC;

Error:
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
	return matrix<float, 4, 4>::PrintMatrix();
}