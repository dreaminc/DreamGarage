#include "ReflectionMatrix.h"

#include "core/primitives/point.h"
#include "core/primitives/vector.h"
#include "core/primitives/plane.h"

ReflectionMatrix::ReflectionMatrix(point pt, vector vNormal) {

	CalculateMatrix(pt, vNormal);
}

ReflectionMatrix::ReflectionMatrix(plane reflectionPlane) {

	CalculateMatrix(reflectionPlane.GetPosition(), reflectionPlane.GetNormal());
}

ReflectionMatrix::~ReflectionMatrix() {
	// empty stub
}

RESULT ReflectionMatrix::CalculateMatrix(point pt, vector vNormal) {
	RESULT r = R_PASS;

	this->identity();
	
	vNormal = vNormal.Normal();

	//reflect_precision Dvalue = plane::GetDValue(pt, vNormal);
	reflect_precision d = vector(pt).dot(vNormal);

	this->element(0, 0) = 1.0f - 2.0f * vNormal.x() * vNormal.x();
	this->element(0, 1) = -2.0f * vNormal.x() * vNormal.y();
	this->element(0, 2) = -2.0f * vNormal.x() * vNormal.z();
	this->element(0, 3) = 2.0f * vNormal.x() * d;

	
	this->element(1, 0) = -2.0f * vNormal.y() * vNormal.x();
	this->element(1, 1) = 1.0f - 2.0f * vNormal.y() * vNormal.y();
	this->element(1, 2) = -2.0f * vNormal.y() * vNormal.z();
	this->element(1, 3) = 2.0f * vNormal.y() * d;

	this->element(2, 0) = -2.0f * vNormal.z() * vNormal.x();
	this->element(2, 1) = -2.0f * vNormal.z() * vNormal.y();
	this->element(2, 2) = 1.0f - 2.0f * vNormal.z() * vNormal.z();
	this->element(2, 3) = 2.0f * vNormal.z() * d;

	// Done by identity 
	this->element(3, 0) = 0.0f;
	this->element(3, 1) = 0.0f;
	this->element(3, 2) = 0.0f;
	this->element(3, 3) = 1.0f;

	//this->element(3, 3) = 0.0f;

//Error:	
	return r;
}

RESULT ReflectionMatrix::PrintMatrix() {
	DEBUG_LINEOUT("Reflection Matrix");
	return matrix<reflect_precision, 4, 4>::PrintMatrix();
}