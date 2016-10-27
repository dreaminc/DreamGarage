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

point VirtualObj::GetPosition() {
	return m_ptOrigin;
}

VirtualObj* VirtualObj::translate(matrix <point_precision, 4, 1> v) {
	m_ptOrigin.translate(v);
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::translate(point_precision x, point_precision y, point_precision z) {
	m_ptOrigin.translate(x, y, z);
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::translateX(point_precision x) {
	m_ptOrigin.translateX(x);
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::translateY(point_precision y) {
	m_ptOrigin.translateY(y);
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::translateZ(point_precision z) {
	m_ptOrigin.translateZ(z);
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::SetPosition(point p) {
	m_ptOrigin = p;
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::MoveTo(point p) {
	m_ptOrigin = p;
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::Scale(point_precision scale) {
	m_vScale.x() = m_vScale.y() = m_vScale.z() = scale;
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::MoveTo(point_precision x, point_precision y, point_precision z) {
	m_ptOrigin.x() = x;
	m_ptOrigin.y() = y;
	m_ptOrigin.z() = z;
	OnManipulation();
	return this;
}

// Pivot Point
RESULT VirtualObj::SetPivotPoint(point ptPivot) {
	m_ptPivot = ptPivot;
	OnManipulation();
	return R_PASS;
}

RESULT VirtualObj::SetPivotPoint(point_precision x, point_precision y, point_precision z) {
	m_ptPivot = point(x, y, z);
	OnManipulation();
	return R_PASS;
}

// Velocity
VirtualObj* VirtualObj::AddVelocity(matrix <point_precision, 4, 1> v) {
	m_vVelocity += v;
	return this;
}

VirtualObj* VirtualObj::AddVelocity(point_precision x, point_precision y, point_precision z) {
	m_vVelocity.x() += x;
	m_vVelocity.y() += y;
	m_vVelocity.z() += z;

	return this;
}

VirtualObj* VirtualObj::SetVelocity(matrix <point_precision, 4, 1> v) {
	m_vVelocity = v;
	return this;
}

VirtualObj* VirtualObj::SetVelocity(point_precision x, point_precision y, point_precision z) {
	m_vVelocity.x() = x;
	m_vVelocity.y() = y;
	m_vVelocity.z() = z;
	return this;
}

// Rotation
VirtualObj* VirtualObj::RotateBy(quaternion q) {
	m_qRotation *= q;
	m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::RotateBy(quaternion_precision thetaX, quaternion_precision thetaY, quaternion_precision thetaZ) {
//	m_qRotation.RotateXYZ(thetaX, thetaY, thetaZ);
	RotateXBy(thetaX);
	RotateYBy(thetaY);
	RotateZBy(thetaZ);
	OnManipulation();

	return this;
}

VirtualObj* VirtualObj::RotateXBy(quaternion_precision theta) {
	m_qRotation.RotateByVector(vector::iVector(), theta);
	m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::RotateYBy(quaternion_precision theta) {
	m_qRotation.RotateByVector(vector::jVector(), theta);
	m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::RotateZBy(quaternion_precision theta) {
	m_qRotation.RotateByVector(vector::kVector(), theta);
	m_qRotation.Normalize();
	return this;
}

VirtualObj* VirtualObj::RotateByDeg(quaternion_precision degX, quaternion_precision degY, quaternion_precision degZ) {
	quaternion_precision thetaX = degX * (M_PI / 180.f);
	quaternion_precision thetaY = degY * (M_PI / 180.f);
	quaternion_precision thetaZ = degZ * (M_PI / 180.f);

	return RotateBy(thetaX, thetaY, thetaZ);
}

VirtualObj* VirtualObj::RotateXByDeg(quaternion_precision deg) {
	return RotateXBy(deg * (M_PI / 180.f));
}

VirtualObj* VirtualObj::RotateYByDeg(quaternion_precision deg) {
	return RotateYBy(deg * (M_PI / 180.f));
}

VirtualObj* VirtualObj::RotateZByDeg(quaternion_precision deg) {
	return RotateZBy(deg * (M_PI / 180.f));
}
		   
VirtualObj* VirtualObj::SetRotate(quaternion q) {
	m_qRotation = q;
	m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::SetOrientation(quaternion qOrientation) {
	m_qRotation = qOrientation;
	m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::SetRotate(quaternion_precision x, quaternion_precision y, quaternion_precision z) {
	m_qRotation = quaternion::MakeQuaternionWithEuler(x, y, z);
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::SetRotateDeg(quaternion_precision degX, quaternion_precision degY, quaternion_precision degZ) {
	quaternion_precision thetaX = degX * (M_PI / 180.f);
	quaternion_precision thetaY = degY * (M_PI / 180.f);
	quaternion_precision thetaZ = degZ * (M_PI / 180.f);

	return SetRotate(degX, degY, degZ);
}

VirtualObj* VirtualObj::ResetRotation() {
	m_qRotation.SetValues(1.0f, 0.0f, 0.0f, 0.0f);
	return this;
}

/* Unsupported
VirtualObj* VirtualObj::SetRotateX(quaternion_precision theta) {
	//m_qRotation = quaternion::iQuaternion(theta) * quaternion::jQuaternion(m_qRotation.GetEulerAngleY()) * quaternion::kQuaternion(m_qRotation.GetEulerAngleZ());
	quaternion_precision x, y, z;
	m_qRotation.GetEulerAngles(&x, &y, &z);
	m_qRotation = quaternion::MakeQuaternionWithEuler(theta, y, z);
	m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::SetRotateY(quaternion_precision theta) {
//	m_qRotation = quaternion::iQuaternion(m_qRotation.GetEulerAngleX()) * quaternion::jQuaternion(theta) * quaternion::kQuaternion(m_qRotation.GetEulerAngleZ());
	quaternion_precision x, y, z;
	m_qRotation.GetEulerAngles(&x, &y, &z);
	m_qRotation = quaternion::MakeQuaternionWithEuler(x, theta, z);
	m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::SetRotateZ(quaternion_precision theta) {
	//m_qRotation = quaternion::iQuaternion(m_qRotation.GetEulerAngleX()) * quaternion::jQuaternion(m_qRotation.GetEulerAngleY()) * quaternion::kQuaternion(theta);
	quaternion_precision x, y, z;
	m_qRotation.GetEulerAngles(&x, &y, &z);
	m_qRotation = quaternion::MakeQuaternionWithEuler(x, y, theta);
	m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::SetRotateXDeg(quaternion_precision deg) {
	return SetRotateX(deg * (M_PI / 180.f));
}

VirtualObj* VirtualObj::SetRotateYDeg(quaternion_precision deg) {
	return SetRotateY(deg * (M_PI / 180.f));
}

VirtualObj* VirtualObj::SetRotateZDeg(quaternion_precision deg) {
	return SetRotateZ(deg * (M_PI / 180.f));
}
//*/

quaternion VirtualObj::GetOrientation() {
	return m_qRotation;
}

matrix<virtual_precision, 4, 4> VirtualObj::GetOrientationMatrix() {
	matrix<virtual_precision, 4, 4> retMatrix = RotationMatrix(m_qRotation);
	return retMatrix;
}

// Angular Momentum
VirtualObj* VirtualObj::AddAngularMomentum(quaternion q) {
	m_qAngularMomentum *= q;
	return this;
}

VirtualObj* VirtualObj::SetAngularMomentum(quaternion am) {
	m_qAngularMomentum = am;
	return this;
}

// Update Functions
VirtualObj* VirtualObj::UpdatePosition() {
	m_ptOrigin += m_vVelocity;
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::UpdateRotation() {
	m_qRotation *= m_qAngularMomentum;
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::Update() {
	return UpdatePosition()->UpdateRotation();
}

RESULT VirtualObj::OnManipulation() {
	return R_SKIPPED;
}

// Matrix Functions

// TODO: Fix naming on scaling matrix + add vector function
matrix<virtual_precision, 4, 4> VirtualObj::GetModelMatrix(matrix<virtual_precision, 4, 4> childMat) {
	return (TranslationMatrix(m_ptOrigin, m_ptPivot) * RotationMatrix(m_qRotation) * ScalingMatrix(m_vScale.x(), m_vScale.y(), m_vScale.z()) * childMat);
	//return (TranslationMatrix(m_ptOrigin, m_ptPivot) * RotationMatrix(m_qRotation) * ScalingMatrix(m_vScale.x(), m_vScale.y(), m_vScale.z()));
	/*
	if (m_ptPivot.IsZero()) {
		return (TranslationMatrix(m_ptOrigin) * RotationMatrix(m_qRotation) * ScalingMatrix(m_vScale.x(), m_vScale.y(), m_vScale.z()) * childMat);
	}
	else {
		return (TranslationMatrix(m_ptOrigin, m_ptPivot) * RotationMatrix(m_qRotation) * ScalingMatrix(m_vScale.x(), m_vScale.y(), m_vScale.z()) * childMat);
	}
	*/
}
