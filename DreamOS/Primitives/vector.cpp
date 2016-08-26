#include "vector.h"

#include "point.h"

vector::vector() {
	clear();
}

vector::vector(matrix <vector_precision, 4, 1> &rhs) {
	this->clear();
	this->element(0, 0) = rhs.element(0, 0);
	this->element(1, 0) = rhs.element(1, 0);
	this->element(2, 0) = rhs.element(2, 0);
	this->element(3, 0) = 1.0f;
}

vector::vector(vector_precision x, vector_precision y, vector_precision z) {
	this->clear();
	this->element(0, 0) = x;
	this->element(1, 0) = y;
	this->element(2, 0) = z;
	this->element(3, 0) = 1.0f;
}

// Calculate the cross product
// Not assumed to be normalized
vector::vector(vector rhs, vector lhs) {
	clear();

	x((rhs(1) * lhs(2)) - (rhs(2) * lhs(1)));
	y((rhs(2) * lhs(0)) - (rhs(0) * lhs(2)));
	z((rhs(0) * lhs(1)) - (rhs(1) * lhs(0)));

	// For good measure
	w(1);
}

vector::vector(const point& pt) {
	for (int i = 0; i < 4; i++)
		m_data[i] = pt.m_data[i];
}

double vector::magnitude() {
	double sqaureSum = pow(x(), 2) + pow(y(), 2) + pow(z(), 2);
	return sqrt(sqaureSum);
}

// Will normalize this vector
RESULT vector::Normalize() {
	vector_precision denom = 0;

	for (int i = 0; i < 3; i++)
		denom += static_cast<vector_precision>(pow(element(i, 0), 2));

	denom = static_cast<vector_precision>(sqrt(denom));

	for (int i = 0; i < 3; i++)
		element(i, 0) = element(i, 0) / denom;

	element(3, 0) = 1.0f;

	return R_PASS;
}

// Return a normalized version of this vector
vector vector::Normal() {
	vector result = *this;
	result.Normalize();
	return result;
}

RESULT vector::Print(char *pszOptName) {
	DEBUG_LINEOUT("%s(%f, %f, %f, %f)", (pszOptName != nullptr) ? pszOptName : "v", x(), y(), z(), w());
	return R_PASS;
}

// Dot Product
// This calculates the dot product as if it is a R3 vector (ignores the w parameter) 
vector_precision vector::dot(vector& rhs) {
	vector_precision result = 0;

	for (int i = 0; i < 3; i++)
		result += element(i, 0) * rhs.element(i, 0);

	return result;
}

vector_precision vector::dot(point& rhs) {
	vector_precision result = 0;

	for (int i = 0; i < 3; i++)
		result += element(i, 0) * rhs.element(i, 0);

	return result;
}

// Cross Product
vector vector::cross(vector rhs) {
	return vector(*this, rhs);
}

// Normalized Cross Product
vector vector::NormalizedCross(vector rhs) {
	return vector(this->Normal(), rhs.Normal());
}

// Explicitly specializing the assignment operator
vector& vector::operator=(const matrix<vector_precision, 4, 1> &arg) {
	if (this == &arg)      // Same object?
		return *this;        // Yes, so skip assignment, and just return *this.

	memcpy(this->m_data, arg.m_data, sizeof(vector_precision) * 4 * 1);

	return *this;
}


vector& vector::operator*=(const vector_precision& a) {
	for (int i = 0; i < 4; i++)
		m_data[i] *= a;

	return *this;
}

vector vector::operator*(const vector_precision& a) const {
	return vector(*this).operator*=(a);
}