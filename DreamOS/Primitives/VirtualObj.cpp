#include "VirtualObj.h"

VirtualObj::VirtualObj() :
	m_ptOrigin(),
	m_vVelocity(),
	m_vScale(1.0, 1.0, 1.0),
	m_qRotation(),
	m_qAngularMomentum()
{
	/* stub */
}

VirtualObj::VirtualObj(point ptOrigin) : 
	m_ptOrigin(ptOrigin),
	m_vVelocity(),
	m_vScale(1.0, 1.0, 1.0),
	m_qRotation(),
	m_qAngularMomentum()
{
	// stub 
}

VirtualObj::~VirtualObj() {
	// Empty Stub
}

// Position
point VirtualObj::GetOrigin() {
	return m_ptOrigin;
}

VirtualObj VirtualObj::translate(matrix <point_precision, 4, 1> v) {
	m_ptOrigin.translate(v);
	return *this;
}

VirtualObj VirtualObj::translate(point_precision x, point_precision y, point_precision z) {
	m_ptOrigin.translate(x, y, z);
	return (*this);
}

VirtualObj VirtualObj::translateX(point_precision x) {
	m_ptOrigin.translateX(x);
	return (*this);
}

VirtualObj VirtualObj::translateY(point_precision y) {
	m_ptOrigin.translateY(y);
	return (*this);
}

VirtualObj VirtualObj::translateZ(point_precision z) {
	m_ptOrigin.translateZ(z);
	return (*this);
}

VirtualObj VirtualObj::MoveTo(point p) {
	m_ptOrigin = p;
	return (*this);
}

VirtualObj VirtualObj::Scale(point_precision scale) {
	m_vScale.x() = m_vScale.y() = m_vScale.z() = scale;
	return (*this);
}

VirtualObj VirtualObj::MoveTo(point_precision x, point_precision y, point_precision z) {
	m_ptOrigin.x() = x;
	m_ptOrigin.y() = y;
	m_ptOrigin.z() = z;

	return (*this);
}

// Velocity
VirtualObj VirtualObj::AddVelocity(matrix <point_precision, 4, 1> v) {
	m_vVelocity += v;
	return (*this);
}

VirtualObj VirtualObj::AddVelocity(point_precision x, point_precision y, point_precision z) {
	m_vVelocity.x() += x;
	m_vVelocity.y() += y;
	m_vVelocity.z() += z;

	return (*this);
}

VirtualObj VirtualObj::SetVelocity(matrix <point_precision, 4, 1> v) {
	m_vVelocity = v;
	return (*this);
}

VirtualObj VirtualObj::SetVelocity(point_precision x, point_precision y, point_precision z) {
	m_vVelocity.x() = x;
	m_vVelocity.y() = y;
	m_vVelocity.z() = z;
	return (*this);
}

// Rotation
VirtualObj VirtualObj::RotateBy(quaternion q) {
	m_qRotation *= q;
	return (*this);
}

// TODO: This is not working
VirtualObj VirtualObj::RotateBy(quaternion_precision x, quaternion_precision y, quaternion_precision z) {
	m_qRotation *= quaternion::iQuaternion(x);
	m_qRotation *= quaternion::jQuaternion(y);
	m_qRotation *= quaternion::kQuaternion(z);
	
	return (*this);
}

VirtualObj VirtualObj::RotateXBy(quaternion_precision theta) {
	m_qRotation *= quaternion::iQuaternion(theta);
	return (*this);
}

VirtualObj VirtualObj::RotateYBy(quaternion_precision theta) {
	m_qRotation *= quaternion::jQuaternion(theta);
	return (*this);
}

VirtualObj VirtualObj::RotateZBy(quaternion_precision theta) {
	m_qRotation *= quaternion::kQuaternion(theta);
	return (*this);
}
		   
// TODO: This is reproduced, choose one
VirtualObj VirtualObj::SetRotate(quaternion q) {
	m_qRotation = q;
	return (*this);
}

VirtualObj VirtualObj::SetOrientation(quaternion qOrientation) {
	m_qRotation = qOrientation;
	return (*this);
}

VirtualObj VirtualObj::SetRotate(quaternion_precision x, quaternion_precision y, quaternion_precision z) {
	m_qRotation = quaternion::iQuaternion(x) * quaternion::jQuaternion(y) * quaternion::kQuaternion(z);
	return (*this);
}

VirtualObj VirtualObj::SetRotateX(quaternion_precision theta) {
	m_qRotation = quaternion::iQuaternion(theta) * quaternion::jQuaternion(m_qRotation.GetEulerAngelY()) * quaternion::kQuaternion(m_qRotation.GetEulerAngelZ());
	return (*this);
}

VirtualObj VirtualObj::SetRotateY(quaternion_precision theta) {
	m_qRotation = quaternion::iQuaternion(m_qRotation.GetEulerAngelX()) * quaternion::jQuaternion(theta) * quaternion::kQuaternion(m_qRotation.GetEulerAngelZ());
	return (*this);
}

VirtualObj VirtualObj::SetRotateZ(quaternion_precision theta) {
	m_qRotation = quaternion::iQuaternion(m_qRotation.GetEulerAngelX()) * quaternion::jQuaternion(m_qRotation.GetEulerAngelY()) * quaternion::kQuaternion(theta);
	return (*this);
}

quaternion VirtualObj::GetOrientation() {
	return m_qRotation;
}

matrix<virtual_precision, 4, 4> VirtualObj::GetOrientationMatrix() {
	matrix<virtual_precision, 4, 4> retMatrix = RotationMatrix(m_qRotation);
	return retMatrix;
}

// Angular Momentum
VirtualObj VirtualObj::AddAngularMomentum(quaternion q) {
	m_qAngularMomentum *= q;
	return (*this);
}

VirtualObj VirtualObj::SetAngularMomentum(quaternion am) {
	m_qAngularMomentum = am;
	return (*this);
}

// Update Functions
VirtualObj VirtualObj::UpdatePosition() {
	m_ptOrigin += m_vVelocity;
	return (*this);
}

VirtualObj VirtualObj::UpdateRotation() {
	m_qRotation *= m_qAngularMomentum;
	return (*this);
}

VirtualObj VirtualObj::Update() {
	return UpdatePosition().UpdateRotation();
}

// Matrix Functions
matrix<virtual_precision, 4, 4> VirtualObj::GetModelMatrix() {
	matrix<virtual_precision, 4, 4> retMatrix = TranslationMatrix(m_ptOrigin) * RotationMatrix(m_qRotation) * ScalingMatrix(m_vScale.x(), m_vScale.y(), m_vScale.z());
	return retMatrix;
}