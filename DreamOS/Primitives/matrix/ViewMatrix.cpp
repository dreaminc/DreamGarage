#include "ViewMatrix.h"

ViewMatrix::ViewMatrix() {
	clear();
}

ViewMatrix::ViewMatrix(point ptPosition) {
	clear();
	SetViewMatrixPitchYawRoll(ptPosition, 0.0f, 0.0f, 0.0f);
}

// This will start with i, j, k vectors and effectively rotate them about the appropriate axes 
// pitch is about the x axis, yaw is about the y axis and roll is about the z axis
ViewMatrix::ViewMatrix(point ptPosition, view_precision pitch, view_precision yaw, view_precision roll) {
	clear();
	SetViewMatrixPitchYawRoll(ptPosition, pitch, yaw, roll);
}

ViewMatrix::ViewMatrix(point ptPosition, quaternion qLook) {
	clear();
	SetViewMatrixPointQuaternion(ptPosition, qLook);
}

ViewMatrix::ViewMatrix(point ptPosition, vector vLook) {
	identity(1.0f);
	SetViewMatrixPointVector(ptPosition, vLook);
}

ViewMatrix::ViewMatrix(point ptPosition, BasisMatrix matBasis) {
	identity(1.0f);
	SetViewMatrixPointBasis(ptPosition, matBasis);
}

ViewMatrix::~ViewMatrix() {
	// empty stub
}

RESULT ViewMatrix::SetViewMatrixPitchYawRoll(point ptPosition, view_precision pitch, view_precision yaw, view_precision roll) {
	//m_ptPosition = ptPosition;

	/*
	m_vLook = matrixRotation * vector(0.0f, 0.0f, 1.0f);
	m_vUp = matrixRotation * vector(0.0f, 1.0f, 0.0f);
	m_vRight = matrixRotation * vector(1.0f, 1.0f, 1.0f);
	*/

	return SetMatrix(RotationMatrix(pitch, yaw, roll) * TranslationMatrix(ptPosition));
	//return SetMatrix(TranslationMatrix(ptPosition));
}

RESULT ViewMatrix::SetViewMatrixPointQuaternion(point ptPosition, quaternion qLook) {
	return SetMatrix(RotationMatrix(qLook) * TranslationMatrix(ptPosition));
}

RESULT ViewMatrix::SetViewMatrixPointVector(point ptPosition, vector vLook) {

	// Create a basis from the j vector

	vector vUp = vector::jVector(1.0f);
	vector vLeft = vLook.NormalizedCross(vUp);
	vUp = vLeft.cross(vLook);

	auto matBasis = BasisMatrix(vLeft, vUp, vLook);

	SetMatrix(matBasis * TranslationMatrix(ptPosition));

	return R_PASS;
}

RESULT ViewMatrix::SetViewMatrixPointBasis(point ptPosition, BasisMatrix matBasis) {
	SetMatrix(matBasis * TranslationMatrix(ptPosition));
	return R_PASS;
}

RotationMatrix ViewMatrix::GetRotationMatrix() {
	RotationMatrix matRotation;

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			matRotation.element(i, j) = this->element(i, j);

	return matRotation;
}

point ViewMatrix::GetPosition() {
	view_precision x = this->element(0, 3);
	view_precision y = this->element(1, 3);
	view_precision z = this->element(2, 3);
	//view_precision w = this->element(3, 3);
	view_precision w = 1.0f;

	return point(x, y, z, 1.0f);
}

quaternion ViewMatrix::GetOrientation() {
	return GetRotationMatrix().GetQuaternion();
}

RESULT ViewMatrix::PrintMatrix() {
	DEBUG_LINEOUT("View Matrix");
	return matrix<view_precision, 4, 4>::PrintMatrix();
}