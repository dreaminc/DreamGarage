#include "VirtualObj.h"

VirtualObj::VirtualObj() :
	m_objectState(),
	m_objectDerivative(),
	m_vScale(1.0, 1.0, 1.0)
{
	/* stub */
}

VirtualObj::VirtualObj(point ptOrigin) : 
	m_objectState(ptOrigin),
	m_objectDerivative(),
	m_vScale(1.0, 1.0, 1.0)
{
	// stub 
}

VirtualObj::~VirtualObj() {
	// Empty Stub
}

// State
ObjectState VirtualObj::GetState() {
	return m_objectState;
}

RESULT VirtualObj::SetState(ObjectState virtualObjState) {
	m_objectState = virtualObjState;
	return R_SUCCESS;
}

// Derivative
ObjectDerivative VirtualObj::GetDerivative() {
	return m_objectDerivative;
}

RESULT VirtualObj::SetDerivative(ObjectDerivative virtualObjDerivative) {
	m_objectDerivative = virtualObjDerivative;
	return R_SUCCESS;
}

template <ObjectState::IntegrationType IT>
RESULT VirtualObj::IntegrateState(float timeStart, float timeDelta, const std::list<ForceGenerator*> &externalForceGenerators) {
	RESULT r = R_SUCCESS;

	CR(m_objectState.Integrate<IT>(timeStart, timeDelta, externalForceGenerators));
	OnManipulation();	// TODO: we might want to skip this if there is no manipulation

Error:
	return r;
}

// Meta-Template Requirement
template RESULT VirtualObj::IntegrateState<ObjectState::IntegrationType::RK4>(float timeStart, float timeDelta, const std::list<ForceGenerator*> &externalForceGenerators);
template RESULT VirtualObj::IntegrateState<ObjectState::IntegrationType::EUCLID>(float timeStart, float timeDelta, const std::list<ForceGenerator*> &externalForceGenerators);

// Position
point VirtualObj::GetOrigin() {
	return m_objectState.m_ptOrigin;
}

point VirtualObj::GetPosition() {
	return m_objectState.m_ptOrigin;
}

VirtualObj* VirtualObj::translate(matrix <point_precision, 4, 1> v) {
	m_objectState.m_ptOrigin.translate(v);
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::translate(point_precision x, point_precision y, point_precision z) {
	m_objectState.m_ptOrigin.translate(x, y, z);
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::translateX(point_precision x) {
	m_objectState.m_ptOrigin.translateX(x);
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::translateY(point_precision y) {
	m_objectState.m_ptOrigin.translateY(y);
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::translateZ(point_precision z) {
	m_objectState.m_ptOrigin.translateZ(z);
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::SetOrigin(point p) {
	m_objectState.m_ptOrigin = p;
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::SetPosition(point p) {
	m_objectState.m_ptOrigin = p;
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::MoveTo(point p) {
	m_objectState.m_ptOrigin = p;
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::Scale(point_precision scale) {
	m_vScale.x() = m_vScale.y() = m_vScale.z() = scale;
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::MoveTo(point_precision x, point_precision y, point_precision z) {
	m_objectState.m_ptOrigin.x() = x;
	m_objectState.m_ptOrigin.y() = y;
	m_objectState.m_ptOrigin.z() = z;
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
VirtualObj* VirtualObj::AddVelocity(matrix <point_precision, 4, 1> vVelocity) {
	m_objectState.SetVelocity(m_objectState.GetVelocity() + vVelocity);
	return this;
}

VirtualObj* VirtualObj::AddVelocity(point_precision x, point_precision y, point_precision z) {
	m_objectState.SetVelocity(m_objectState.GetVelocity() + vector(x, y, z));
	return this;
}

VirtualObj* VirtualObj::SetVelocity(matrix <point_precision, 4, 1> vVelocity) {
	m_objectState.SetVelocity(vVelocity);
	return this;
}

VirtualObj* VirtualObj::SetVelocity(point_precision x, point_precision y, point_precision z) {
	m_objectState.SetVelocity(vector(x, y, z));
	return this;
}

/*
VirtualObj* VirtualObj::AddAcceleration(matrix <point_precision, 4, 1> vAccel) {
	m_objectState.m_vAcceleration += vAccel;
	return this;
}

VirtualObj* VirtualObj::AddAcceleration(point_precision x, point_precision y, point_precision z) {
	m_objectState.m_vAcceleration.x() += x;
	m_objectState.m_vAcceleration.y() += y;
	m_objectState.m_vAcceleration.z() += z;

	return this;
}

// Acceleration
VirtualObj* VirtualObj::SetAcceleration(matrix <point_precision, 4, 1> vAccel) {
	m_objectState.m_vAcceleration = vAccel;
	return this;
}

VirtualObj* VirtualObj::SetAcceleration(point_precision x, point_precision y, point_precision z) {
	m_objectState.m_vAcceleration.x() = x;
	m_objectState.m_vAcceleration.y() = y;
	m_objectState.m_vAcceleration.z() = z;

	return this;
}
*/

// Rotation
VirtualObj* VirtualObj::RotateBy(quaternion q) {
	m_objectState.m_qRotation *= q;
	m_objectState.m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::RotateBy(vector v, quaternion_precision theta) {
	m_objectState.m_qRotation.RotateByVector(v, theta);
	m_objectState.m_qRotation.Normalize();
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
	m_objectState.m_qRotation.RotateByVector(vector::iVector(), theta);
	m_objectState.m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::RotateYBy(quaternion_precision theta) {
	m_objectState.m_qRotation.RotateByVector(vector::jVector(), theta);
	m_objectState.m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::RotateZBy(quaternion_precision theta) {
	m_objectState.m_qRotation.RotateByVector(vector::kVector(), theta);
	m_objectState.m_qRotation.Normalize();
	OnManipulation();
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
	m_objectState.m_qRotation = q;
	m_objectState.m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::SetOrientation(quaternion qOrientation) {
	m_objectState.m_qRotation = qOrientation;
	m_objectState.m_qRotation.Normalize();
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::SetRotate(quaternion_precision x, quaternion_precision y, quaternion_precision z) {
	m_objectState.m_qRotation = quaternion::MakeQuaternionWithEuler(x, y, z);
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
	m_objectState.m_qRotation.SetValues(1.0f, 0.0f, 0.0f, 0.0f);
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
	return m_objectState.m_qRotation;
}

matrix<virtual_precision, 4, 4> VirtualObj::GetOrientationMatrix() {
	matrix<virtual_precision, 4, 4> retMatrix = RotationMatrix(m_objectState.m_qRotation);
	return retMatrix;
}

// Angular Momentum
VirtualObj* VirtualObj::AddAngularMomentum(quaternion q) {
	m_objectState.m_qAngularMomentum *= q;
	return this;
}

VirtualObj* VirtualObj::SetAngularMomentum(quaternion am) {
	m_objectState.m_qAngularMomentum = am;
	return this;
}

RESULT VirtualObj::SetMass(double kgMass) {
	// Negative mass not allowed
	if (kgMass < 0.0f)
		return R_FAIL;

	m_objectState.SetMass(kgMass);
	return R_PASS;
}

double VirtualObj::GetMass() {
	return m_objectState.GetMass();
}

// Update Functions 
// TODO: These should be removed in lieu of physics engine
VirtualObj* VirtualObj::UpdatePosition() {
	m_objectState.m_ptOrigin += m_objectState.m_vVelocity;
	OnManipulation();
	return this;
}

VirtualObj* VirtualObj::UpdateRotation() {
	m_objectState.m_qRotation *= m_objectState.m_qAngularMomentum;
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
	return (TranslationMatrix(m_objectState.m_ptOrigin, m_ptPivot) * RotationMatrix(m_objectState.m_qRotation) * ScalingMatrix(m_vScale.x(), m_vScale.y(), m_vScale.z()) * childMat);
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
