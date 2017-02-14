#include "vector.h"

#include "point.h"
#include "quaternion.h"

vector::vector() {
	clear();
}

// This sets each element of the vector to val
vector::vector(vector_precision val) {
	this->clear();
	this->element(0, 0) = val;
	this->element(1, 0) = val;
	this->element(2, 0) = val;
	this->element(3, 0) = 1.0f;
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
	double sqaureSum = std::pow(x(), 2.0f) + std::pow(y(), 2.0f) + std::pow(z(), 2.0f);
	return sqrt(sqaureSum);
}

double vector::magnitudeSquared() {
	double sqaureSum = std::pow(x(), 2.0f) + std::pow(y(), 2.0f) + std::pow(z(), 2.0f);
	return sqaureSum;
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
vector_precision vector::dot(const vector& rhs) const {
	vector_precision result = 0;

	for (int i = 0; i < 3; i++)
		result += element(i, 0) * rhs.element(i, 0);

	return result;
}

vector_precision vector::dot(const point& rhs) const {
	vector_precision result = 0;

	for (int i = 0; i < 3; i++)
		result += element(i, 0) * rhs.element(i, 0);

	return result;
}

// If vectors are parallel then x1 = k * x2, y1 = k * y2, z1 = k * z2
// and so x1/x2 == y1/y2 == z1/z2
bool vector::IsParallel(const vector &rhs) {
	vector vNormalA = Normal();
	vector vNormalB = static_cast<vector>(rhs).Normal();
	if (vNormalA == vNormalB || vNormalA == (vNormalB * -1.0f)) {
		return true;
	}

	return false;
}

bool vector::IsValid() {
	if (std::isnan(x()) || std::isnan(y()) || std::isnan(z()) || std::isnan(w()))
		return false;

	return true;
}

bool vector::IsZero() {
	if (m_data[0] == 0.0f && m_data[1] == 0.0f && m_data[2] == 0.0f)
		return true;

	return false;
}

// Cross Product
vector vector::cross(vector rhs) {
	return vector(*this, rhs);
}

// Normalized Cross Product
vector vector::NormalizedCross(vector rhs) {
	return vector(this->Normal(), rhs.Normal());
}

// http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
vector vector::RotateByQuaternion(quaternion q) {
	return q.RotateVector(*this);
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