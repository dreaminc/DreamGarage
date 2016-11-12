#include "camera.h"

#include "Logger/Logger.h"

#define DEFAULT_NEAR_PLANE 0.1f
#define DEFAULT_FAR_PLANE 1000.0f
#define DEFAULT_CAMERA_ROTATE_SPEED 0.002f
#define DEFAULT_CAMERA_MOVE_SPEED 2.0f

#define DEFAULT_PROJECTION_TYPE PROJECTION_MATRIX_PERSPECTIVE
//#define DEFAULT_PROJECTION_TYPE PROJECTION_MATRIX_ORTHOGRAPHIC


camera::camera(point ptOrigin, camera_precision FOV, int pxScreenWidth, int pxScreenHeight) :
	m_FieldOfViewAngle(FOV),
	m_ProjectionType(DEFAULT_PROJECTION_TYPE),
	m_NearPlane(DEFAULT_NEAR_PLANE),
	m_FarPlane(DEFAULT_FAR_PLANE),
	m_pxScreenWidth(pxScreenWidth),
	m_pxScreenHeight(pxScreenHeight),
	m_cameraRotateSpeed(DEFAULT_CAMERA_ROTATE_SPEED),
	m_cameraForwardSpeed(0.0f),
	m_cameraStrafeSpeed(0.0f),
	m_cameraUpSpeed(0.0f),
	m_vDeviation(),
	m_pCameraFrameOfReference(nullptr)
{
	m_ptOrigin = ptOrigin;
	m_qRotation = quaternion();
	m_qOffsetOrientation = quaternion();
}

camera::~camera() {
	// empty stub
}

RESULT camera::ResizeCamera(int pxWidth, int pxHeight) {
	m_pxScreenWidth = pxWidth;
	m_pxScreenHeight = pxHeight;
	return R_PASS;
}

vector camera::GetUpVector() {
	return vector::jVector();
}

int camera::GetPXWidth() {
	return m_pxScreenWidth;
}

int camera::GetPXHeight() {
	return m_pxScreenHeight;
}

vector camera::GetRightVector() {
	quaternion temp = m_qRotation;
	temp.Normalize();

	vector vectorRight = temp.RotateVector(vector::iVector());
	return vectorRight.Normal();
}

vector camera::GetLookVector() {
	quaternion temp = m_qRotation;
	temp.Normalize();

	vector vectorLook = temp.RotateVector(vector(0.0f, 0.0f, -1.0f));
	return vectorLook.Normal();
}

ProjectionMatrix camera::GetProjectionMatrix() {
	return ProjectionMatrix(m_ProjectionType,
		static_cast<projection_precision>(m_pxScreenWidth),
		static_cast<projection_precision>(m_pxScreenHeight),
		static_cast<projection_precision>(m_NearPlane),
		static_cast<projection_precision>(m_FarPlane),
		static_cast<projection_precision>(m_FieldOfViewAngle));
}

ViewMatrix camera::GetViewMatrix() {

	// View Matrix requires the opposite of the camera's world position and rotation
	point ptOrigin = m_ptOrigin;
	ptOrigin.SetZeroW();
	ptOrigin.Reverse();

	quaternion q = m_qRotation;

	ptOrigin += m_vDeviation;
	ViewMatrix mat = ViewMatrix(ptOrigin, q);
	return mat;
}

matrix<camera_precision, 4, 4> camera::GetProjectionViewMatrix() {
	return (GetProjectionMatrix() * GetViewMatrix());
}

RESULT camera::RotateCameraByDiffXY(camera_precision dx, camera_precision dy) {

	m_qRotation *= quaternion(dy * m_cameraRotateSpeed, GetRightVector());
	m_qRotation *= quaternion(dx * m_cameraRotateSpeed, GetUpVector());

	m_qRotation.Normalize();

	vector vectorLook = GetLookVector();
	DEBUG_LINEOUT_RETURN("Camera rotating: x:%0.3f y:%0.3f z:%0.3f", vectorLook.x(), vectorLook.y(), vectorLook.z());

	return R_PASS;
}

RESULT camera::MoveForward(camera_precision amt) {
	m_ptOrigin += GetLookVector() * amt;
	m_ptOrigin(3) = 0.0f;
	return R_PASS;
}

RESULT camera::MoveUp(camera_precision amt) {
	m_ptOrigin += GetUpVector() * amt;
	return R_PASS;
}

RESULT camera::MoveStrafe(camera_precision amt) {
	m_ptOrigin += GetRightVector() * amt;
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

RESULT camera::Notify(HMDEvent *hmdEvent) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Cam hmd event");//, kbEvent->KeyCode, kbEvent->KeyState);

//	Error:
	return r;
}

RESULT camera::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;

	if (!m_allowMoveByKeys)
		return r;

	//DEBUG_LINEOUT("Cam Key %d state: %x", kbEvent->KeyCode, kbEvent->KeyState);

	bool disableAWDS = DreamConsole::GetConsole()->IsInForeground();

	bool shiftKey = (kbEvent->m_pSenseKeyboard->GetKeyState(SK_SHIFT) & 0x80) > 0;

	bool leftKey = (kbEvent->KeyCode == SK_LEFT) || (!disableAWDS && kbEvent->KeyCode == (SK_SCAN_CODE)('A'));
	bool rightKey = (kbEvent->KeyCode == SK_RIGHT) || (!disableAWDS && kbEvent->KeyCode == (SK_SCAN_CODE)('D'));
	bool forwardKey = (kbEvent->KeyCode == SK_UP && !shiftKey) || (!disableAWDS && kbEvent->KeyCode == (SK_SCAN_CODE)('W'));
	bool backwardKey = (kbEvent->KeyCode == SK_DOWN && !shiftKey) || (!disableAWDS && kbEvent->KeyCode == (SK_SCAN_CODE)('S'));
	bool upKey = (kbEvent->KeyCode == SK_UP   && shiftKey) || (!disableAWDS && kbEvent->KeyCode == (SK_SCAN_CODE)('Q'));
	bool downKey = (kbEvent->KeyCode == SK_DOWN && shiftKey) || (!disableAWDS && kbEvent->KeyCode == (SK_SCAN_CODE)('E'));

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

RESULT camera::Notify(TimeEvent *event) {
	//LOG(INFO) << "time";

	double dt = event->m_deltaTime;

	vector lookMove = GetLookVector();
	lookMove.Normalize();

	vector rightMove = GetRightVector();
	rightMove.y() = 0.0f;
	rightMove.Normalize();

	m_ptOrigin += lookMove * m_cameraForwardSpeed * dt;
	m_ptOrigin += rightMove * m_cameraStrafeSpeed * dt;
	m_ptOrigin += GetUpVector() * m_cameraUpSpeed * dt;
	m_ptOrigin.SetZeroW();

	// Update frame of reference
	quaternion qRotation = m_qRotation;
	qRotation.Reverse(); 

	point ptOrigin;
	ptOrigin = m_ptOrigin;
	if(m_pHMD != nullptr)
		ptOrigin += m_pHMD->GetHeadPointOrigin();
	ptOrigin.SetZeroW();

	m_pCameraFrameOfReference->SetPosition(ptOrigin);
	m_pCameraFrameOfReference->SetOrientation(qRotation);

	return R_PASS;
}

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

	uint8_t state = pSK->GetKeyState(SK_LEFT);
	if (state) {
		DEBUG_LINEOUT("strafe");
		MoveStrafe(0.1f);
	}

	state = pSK->GetKeyState(SK_RIGHT);
	//pSK->CheckKeyState((SK_SCAN_CODE)('D'))
	if (state) {
		MoveStrafe(-0.1f);
	}

	state = pSK->GetKeyState(SK_UP);
	if (state) {
		MoveForward(0.1f);
	}

	state = pSK->GetKeyState(SK_DOWN);
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
	quaternion q = m_qRotation;
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

int camera::GetScreenWidth() {
	return m_pxScreenWidth;
}

int camera::GetScreenHeight() {
	return m_pxScreenHeight;
}

bool camera::IsAllowedMoveByKeys() {
	return m_allowMoveByKeys;
}

RESULT camera::Notify(CmdPromptEvent *event) {
	RESULT r = R_PASS;

	if (event->GetArg(1).compare("move") == 0) {
		m_allowMoveByKeys = !m_allowMoveByKeys;
		HUD_OUT((std::string("allow move by keys <- ") + ((m_allowMoveByKeys) ? "on" : "off")).c_str());
	}

	return r;
}

quaternion camera::GetOffsetOrientation() {
	return m_qOffsetOrientation;
}

RESULT camera::SetOffsetOrientation(quaternion qOffset) {
	m_qOffsetOrientation = qOffset;
	return R_PASS;
}