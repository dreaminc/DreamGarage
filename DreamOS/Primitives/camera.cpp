#include "camera.h"

#include "Logger/Logger.h"

#define DEFAULT_NEAR_PLANE 0.1f
#define DEFAULT_FAR_PLANE 1000.0f
#define DEFAULT_CAMERA_ROTATE_SPEED 0.002f
#define DEFAULT_CAMERA_MOVE_SPEED 2.0f

#define DEFAULT_PROJECTION_TYPE PROJECTION_MATRIX_PERSPECTIVE
//#define DEFAULT_PROJECTION_TYPE PROJECTION_MATRIX_ORTHOGRAPHIC

camera::camera(point ptOrigin, viewport cameraVieport) :
	VirtualObj(ptOrigin),
	m_viewport(cameraVieport),
	m_ProjectionType(DEFAULT_PROJECTION_TYPE),
	m_NearPlane(DEFAULT_NEAR_PLANE),
	m_FarPlane(DEFAULT_FAR_PLANE),
	m_cameraRotateSpeed(DEFAULT_CAMERA_ROTATE_SPEED),
	m_cameraForwardSpeed(0.0f),
	m_cameraStrafeSpeed(0.0f),
	m_cameraUpSpeed(0.0f),
	m_vDeviation(),
	m_pCameraFrameOfReference(nullptr)
{
	//m_ptOrigin = ptOrigin;
	//m_qRotation = quaternion();
	m_qOffsetOrientation = quaternion();
}

camera::~camera() {
	// empty stub
}

RESULT camera::ResizeCamera(viewport cameraViewport) {
	m_viewport = cameraViewport;
	return R_PASS;
}

RESULT camera::ResizeCamera(int pxWidth, int pxHeight) {
	m_viewport.ResizeViewport(pxWidth, pxHeight);
	return R_PASS;
}

vector camera::GetUpVector() {
	return vector::jVector();
}

int camera::GetViewWidth() {
	return m_viewport.Width();
}

int camera::GetViewHeight() {
	return m_viewport.Height();
}

vector camera::GetRightVector() {
	quaternion temp = GetOrientation();
	temp.Normalize();

	vector vectorRight = temp.RotateVector(vector::iVector());
	return vectorRight.Normal();
}

vector camera::GetLookVector() {
	quaternion temp = GetOrientation();
	temp.Normalize();

	vector vectorLook = temp.RotateVector(vector(0.0f, 0.0f, -1.0f));
	return vectorLook.Normal();
}

ProjectionMatrix camera::GetProjectionMatrix() {
	return ProjectionMatrix(m_ProjectionType,
		static_cast<projection_precision>(m_viewport.Width()),
		static_cast<projection_precision>(m_viewport.Height()),
		static_cast<projection_precision>(m_NearPlane),
		static_cast<projection_precision>(m_FarPlane),
		static_cast<projection_precision>(m_viewport.FOVAngle()));
}

ViewMatrix camera::GetViewMatrix() {

	// View Matrix requires the opposite of the camera's world position and rotation
	point ptOrigin = GetOrigin();
	ptOrigin.SetZeroW();
	ptOrigin.Reverse();

	quaternion q = GetOrientation();

	ptOrigin += m_vDeviation;
	ViewMatrix mat = ViewMatrix(ptOrigin, q);
	return mat;
}

matrix<camera_precision, 4, 4> camera::GetProjectionViewMatrix() {
	return (GetProjectionMatrix() * GetViewMatrix());
}

RESULT camera::RotateCameraByDiffXY(camera_precision dx, camera_precision dy) {
	quaternion qRotation = GetOrientation();

	qRotation *= quaternion(dy * m_cameraRotateSpeed, GetRightVector());
	qRotation *= quaternion(dx * m_cameraRotateSpeed, GetUpVector());
	qRotation.Normalize();

	SetOrientation(qRotation);

	vector vectorLook = GetLookVector();
	DEBUG_LINEOUT_RETURN("Camera rotating: x:%0.3f y:%0.3f z:%0.3f", vectorLook.x(), vectorLook.y(), vectorLook.z());

	return R_PASS;
}

RESULT camera::MoveForward(camera_precision amt) {
	translate(GetLookVector() * amt);
	//m_ptOrigin += GetLookVector() * amt;
	//m_ptOrigin(3) = 0.0f;
	return R_PASS;
}

RESULT camera::MoveUp(camera_precision amt) {
	translate(GetUpVector() * amt);
	//m_ptOrigin += GetUpVector() * amt;
	return R_PASS;
}

RESULT camera::MoveStrafe(camera_precision amt) {
	translate(GetRightVector() * amt);
	//m_ptOrigin += GetRightVector() * amt;
	return R_PASS;
}

RESULT camera::SetStrafeSpeed(camera_precision speed) {
	m_cameraStrafeSpeed = speed;
	return R_PASS;
}

RESULT camera::AddStrafeSpeed(camera_precision speed) {
	m_cameraStrafeSpeed += speed;
	return R_PASS;
}

RESULT camera::SetForwardSpeed(camera_precision speed) {
	m_cameraForwardSpeed = speed;
	return R_PASS;
}

RESULT camera::AddForwardSpeed(camera_precision speed) {
	m_cameraForwardSpeed += speed;
	return R_PASS;
}

RESULT camera::SetUpSpeed(camera_precision speed) {
	m_cameraUpSpeed = speed;
	return R_PASS;
}

RESULT camera::AddUpSpeed(camera_precision speed) {
	m_cameraUpSpeed += speed;
	return R_PASS;
}

/*
RESULT camera::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;

	if (!m_allowMoveByKeys)
		return r;

	//DEBUG_LINEOUT("Cam Key %d state: %x", kbEvent->KeyCode, kbEvent->KeyState);

	bool disableAWDS = DreamConsole::GetConsole()->IsInForeground();

	bool shiftKey = (kbEvent->m_pSenseKeyboard->GetKeyState(SVK_SHIFT) & 0x80) > 0;

	bool leftKey = (kbEvent->KeyCode == SVK_LEFT) || (!disableAWDS && kbEvent->KeyCode == (SenseVirtualKey)('A'));
	bool rightKey = (kbEvent->KeyCode == SVK_RIGHT) || (!disableAWDS && kbEvent->KeyCode == (SenseVirtualKey)('D'));
	bool forwardKey = (kbEvent->KeyCode == SVK_UP && !shiftKey) || (!disableAWDS && kbEvent->KeyCode == (SenseVirtualKey)('W'));
	bool backwardKey = (kbEvent->KeyCode == SVK_DOWN && !shiftKey) || (!disableAWDS && kbEvent->KeyCode == (SenseVirtualKey)('S'));
	bool upKey = (kbEvent->KeyCode == SVK_UP   && shiftKey) || (!disableAWDS && kbEvent->KeyCode == (SenseVirtualKey)('Q'));
	bool downKey = (kbEvent->KeyCode == SVK_DOWN && shiftKey) || (!disableAWDS && kbEvent->KeyCode == (SenseVirtualKey)('E'));

#define	MOVE_DIRECTION ((kbEvent->KeyState) ? 1.0f : -1.0f)

	// applies movement along vectors described by Get(Right/Look/Up)Vector
	if (leftKey) AddStrafeSpeed(-DEFAULT_CAMERA_MOVE_SPEED * MOVE_DIRECTION);
	else if (rightKey) AddStrafeSpeed(DEFAULT_CAMERA_MOVE_SPEED * MOVE_DIRECTION);
	else if (forwardKey) AddForwardSpeed(DEFAULT_CAMERA_MOVE_SPEED * MOVE_DIRECTION);
	else if (backwardKey) AddForwardSpeed(-DEFAULT_CAMERA_MOVE_SPEED * MOVE_DIRECTION);
	else if (upKey) AddUpSpeed(DEFAULT_CAMERA_MOVE_SPEED * MOVE_DIRECTION);
	else if (downKey) AddUpSpeed(-DEFAULT_CAMERA_MOVE_SPEED* MOVE_DIRECTION);

	return r;
}
*/

/*
RESULT camera::Notify(TimeEvent *event) {
	//LOG(INFO) << "time";

	double dt = event->m_deltaTime;

	vector lookMove = GetLookVector();
	lookMove.Normalize();

	vector rightMove = GetRightVector();
	rightMove.y() = 0.0f;
	rightMove.Normalize();

	//m_ptOrigin += lookMove * m_cameraForwardSpeed * dt;
	//m_ptOrigin += rightMove * m_cameraStrafeSpeed * dt;
	//m_ptOrigin += GetUpVector() * m_cameraUpSpeed * dt;
	//m_ptOrigin.SetZeroW();

	translate(lookMove * m_cameraForwardSpeed * dt);
	translate(rightMove * m_cameraStrafeSpeed * dt);
	translate(GetUpVector() * m_cameraUpSpeed * dt);

	// Update frame of reference
	quaternion qRotation = GetOrientation();
	qRotation.Reverse(); 

	point ptOrigin;
	ptOrigin = GetOrigin();
	ptOrigin.SetZeroW();

	m_pCameraFrameOfReference->SetPosition(ptOrigin);
	m_pCameraFrameOfReference->SetOrientation(qRotation);

	return R_PASS;
}
*/

// Deviation vector is a vector of deviation from the origin point
// So resulting point = ptOrigin + vDeviation
// This does not affect orientation
RESULT camera::SetCameraPositionDeviation(vector vDeviation) {
	m_vDeviation = vDeviation;
	return R_PASS;
}

// Non-Event driven keyboard input
RESULT camera::UpdateFromKeyboardState(SenseKeyboard *pSK) {
	RESULT r = R_PASS;

	uint8_t state = pSK->GetKeyState(SVK_LEFT);
	if (state) {
		DEBUG_LINEOUT("strafe");
		MoveStrafe(0.1f);
	}

	state = pSK->GetKeyState(SVK_RIGHT);
	if (state) {
		MoveStrafe(-0.1f);
	}

	state = pSK->GetKeyState(SVK_UP);
	if (state) {
		MoveForward(0.1f);
	}

	state = pSK->GetKeyState(SVK_DOWN);
	if (state) {
		MoveForward(-0.1f);
	}

	return r;
}

composite* camera::GetFrameOfReferenceComposite() {
	return m_pCameraFrameOfReference;
}

// Potentially could be removed if orientations from headsets, mouse inputs,
// and in GetViewMatrix are also reversed
quaternion camera::GetWorldOrientation() {
	quaternion q = GetOrientation();
	q.Reverse();
	return q;
}

RESULT camera::AddObjectToFrameOfReferenceComposite(std::shared_ptr<DimObj> pDimObj) {
	RESULT r = R_PASS;

	CN(m_pCameraFrameOfReference);
	CR(m_pCameraFrameOfReference->AddObject(pDimObj));

Error:
	return r;
}

RESULT camera::SetFrameOfReferenceComposite(composite *pComposite) {
	RESULT r = R_PASS;

	m_pCameraFrameOfReference = pComposite;
	CN(m_pCameraFrameOfReference);

Error:
	return r;
}

RESULT camera::SetHMD(HMD *pHMD) {
	m_pHMD = pHMD;
	return R_PASS;
}

ray camera::GetRay(double xPos, double yPos, double t) {
	ray retRay;

	double x = ((2.0f * xPos) / m_viewport.Width()) - 1.0f;
	double y = 1.0f - ((2.0f * yPos) / m_viewport.Height());
	double z = 1.0f;

	retRay.ptOrigin() = point(GetOrigin());
	retRay.vDirection() = point(x, y, -1.0f);
	
	if (t != 0.0f) {
		retRay.ptOrigin() += retRay.vDirection() * t;
	}

	point ptOrigin = GetOrigin();
	ptOrigin.SetZeroW();
	ptOrigin += m_vDeviation;
	ViewMatrix matView = ViewMatrix(ptOrigin, GetOrientation());

	matrix<camera_precision, 4, 4> matProjection = GetProjectionMatrix();
	//matrix<camera_precision, 4, 4> matView = GetViewMatrix();

	matrix<camera_precision, 4, 4> matProjectionInverse = inverse(matProjection);
	matrix<camera_precision, 4, 4> matViewInverse = inverse(matView);

	retRay.vDirection() = matProjectionInverse * retRay.vDirection();
	retRay.vDirection().z() = -1.0f;
	retRay.vDirection().w() = 0.0f;

	retRay.vDirection() = matViewInverse * retRay.vDirection();
	retRay.vDirection().Normalize();

	//retRay.Print();

	return retRay;
}

ray camera::GetRay(int xPos, int yPos) {
	return GetRay(static_cast<double>(xPos), static_cast<double>(yPos));
}
	
bool camera::IsAllowedMoveByKeys() {
	return m_allowMoveByKeys;
}

quaternion camera::GetOffsetOrientation() {
	return m_qOffsetOrientation;
}

RESULT camera::SetOffsetOrientation(quaternion qOffset) {
	m_qOffsetOrientation = qOffset;
	return R_PASS;
}

bool camera::HasHMD() {
	return m_pHMD != nullptr;
}