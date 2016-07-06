#ifndef CAMERA_H_
#define CAMERA_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/camera.h
// Camera Primitive
// The camera is not a dimension object since it does not actually have any dimensions
// instead we've defined a virtual object that it inherits from

//#include "DimObj.h"
#include "VirtualObj.h"
#include "vector.h"
#include "point.h"
#include "color.h"

#include "matrix.h"
#include "ProjectionMatrix.h"
#include "ViewMatrix.h"

#ifdef FLOAT_PRECISION
	typedef float camera_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double camera_precision;
#endif

#define DEFAULT_NEAR_PLANE 0.1f
#define DEFAULT_FAR_PLANE 1000.0f
#define DEFAULT_CAMERA_ROTATE_SPEED 0.002f
#define DEFAULT_CAMERA_MOVE_SPEED 0.05f

#define DEFAULT_PROJECTION_TYPE PROJECTION_MATRIX_PERSPECTIVE
//#define DEFAULT_PROJECTION_TYPE PROJECTION_MATRIX_ORTHOGRAPHIC

#include "Primitives/Subscriber.h"
#include "Sense/SenseKeyboard.h"
#include "HMD/HMD.h"

class camera : public VirtualObj, public Subscriber<SenseKeyboardEvent>, public Subscriber<HMDEvent> {
public:
	camera(point ptOrigin, camera_precision FOV, int pxScreenWidth, int pxScreenHeight) :
		m_FielfOfViewAngle(FOV),
		m_ProjectionType(DEFAULT_PROJECTION_TYPE),
		m_NearPlane(DEFAULT_NEAR_PLANE),
		m_FarPlane(DEFAULT_FAR_PLANE),
		m_pxScreenWidth(pxScreenWidth),
		m_pxScreenHeight(pxScreenHeight),
		m_cameraRotateSpeed(DEFAULT_CAMERA_ROTATE_SPEED),
		m_cameraForwardSpeed(0.0f),
		m_cameraStrafeSpeed(0.0f),
		m_cameraUpSpeed(0.0f),
		m_vDeviation()
	{
		m_ptOrigin = ptOrigin;
		m_qRotation = quaternion(0.0f, 0.0f, 0.0f, 0.0f);
	}

	~camera() {
		// empty stub
	}

	RESULT ResizeCamera(int pxWidth, int pxHeight) {
		m_pxScreenWidth = pxWidth;
		m_pxScreenHeight = pxHeight;
		return R_PASS;
	}

	vector GetUpVector() {
		return vector::jVector();
	}

	int GetPXWidth() {
		return m_pxScreenWidth;
	}

	int GetPXHeight() {
		return m_pxScreenHeight;
	}

	vector GetRightVector() {
		quaternion temp = m_qRotation;
		temp.Normalize();

		vector vectorRight = temp.RotateVector(vector::iVector());
		return vectorRight.Normal();
	}

	vector GetLookVector() {
		quaternion temp = m_qRotation;
		temp.Normalize();

		vector vectorLook = temp.RotateVector(vector::kVector());
		return vectorLook.Normal();
	}

	ProjectionMatrix GetProjectionMatrix() { 
		return ProjectionMatrix(m_ProjectionType,
								static_cast<projection_precision>(m_pxScreenWidth),
								static_cast<projection_precision>(m_pxScreenHeight),
								static_cast<projection_precision>(m_NearPlane),
								static_cast<projection_precision>(m_FarPlane),
								static_cast<projection_precision>(m_FielfOfViewAngle));
	}

	ViewMatrix GetViewMatrix() { 
		point ptOrigin = m_ptOrigin + m_vDeviation;
		ViewMatrix mat = ViewMatrix(ptOrigin, m_qRotation);
		return mat;
	}

	matrix<camera_precision, 4, 4> GetProjectionViewMatrix() { 
		return (GetProjectionMatrix() * GetViewMatrix());
	}

	RESULT RotateCameraByDiffXY(camera_precision dx, camera_precision dy) {	
		m_qRotation.RotateByVector(GetRightVector(), dy * m_cameraRotateSpeed);
		m_qRotation.RotateByVector(GetUpVector(), dx * m_cameraRotateSpeed);
		m_qRotation.Normalize();

		vector vectorLook = GetLookVector();
		DEBUG_LINEOUT_RETURN("Camera rotating: x:%0.3f y:%0.3f z:%0.3f", vectorLook.x(), vectorLook.y(), vectorLook.z());

		return R_PASS;
	}

	RESULT MoveForward(camera_precision amt) {
		m_ptOrigin += GetLookVector() * amt;
		m_ptOrigin(3) = 0.0f;
		return R_PASS;
	}

	RESULT Strafe(camera_precision amt) {
		m_ptOrigin += GetRightVector() * amt;
		return R_PASS;
	}

	RESULT SetStrafeSpeed(camera_precision speed) {
		m_cameraStrafeSpeed = speed;
		return R_PASS;
	}

	RESULT AddStrafeSpeed(camera_precision speed) {
		m_cameraStrafeSpeed += speed;
		return R_PASS;
	}

	RESULT SetForwardSpeed(camera_precision speed) {
		m_cameraForwardSpeed = speed;
		return R_PASS;
	}

	RESULT AddForwardSpeed(camera_precision speed) {
		m_cameraForwardSpeed += speed;
		return R_PASS;
	}

	RESULT SetUpSpeed(camera_precision speed) {
		m_cameraUpSpeed = speed;
		return R_PASS;
	}

	RESULT AddUpSpeed(camera_precision speed) {
		m_cameraUpSpeed += speed;
		return R_PASS;
	}

	RESULT Notify(HMDEvent *hmdEvent) {
		RESULT r = R_PASS;

		DEBUG_LINEOUT("Cam hmd event");//, kbEvent->KeyCode, kbEvent->KeyState);

	Error:
		return r;
	}

	RESULT Notify(SenseKeyboardEvent *kbEvent) {
		RESULT r = R_PASS;

		DEBUG_LINEOUT("Cam Key %d state: %x", kbEvent->KeyCode, kbEvent->KeyState);

		switch (kbEvent->KeyCode) {
			case (SK_SCAN_CODE)('A') :
			case SK_LEFT: {
				if (kbEvent->KeyState)
					AddStrafeSpeed(DEFAULT_CAMERA_MOVE_SPEED);
				else
					AddStrafeSpeed(-DEFAULT_CAMERA_MOVE_SPEED);
			} break;

			case (SK_SCAN_CODE)('D') :
			case SK_RIGHT: {
				if (kbEvent->KeyState)
					AddStrafeSpeed(-DEFAULT_CAMERA_MOVE_SPEED);
				else
					AddStrafeSpeed(DEFAULT_CAMERA_MOVE_SPEED);
			} break;

			case (SK_SCAN_CODE)('W') :
			case SK_UP: {
				if (kbEvent->KeyState)
					AddForwardSpeed(DEFAULT_CAMERA_MOVE_SPEED);
				else
					AddForwardSpeed(-DEFAULT_CAMERA_MOVE_SPEED);
			} break;

			case (SK_SCAN_CODE)('S') :
			case SK_DOWN: {
				if (kbEvent->KeyState)
					AddForwardSpeed(-DEFAULT_CAMERA_MOVE_SPEED);
				else
					AddForwardSpeed(DEFAULT_CAMERA_MOVE_SPEED);
			} break;

			case SK_SPACE: {
				if (kbEvent->KeyState)
					AddUpSpeed(-DEFAULT_CAMERA_MOVE_SPEED);
				else
					AddUpSpeed(DEFAULT_CAMERA_MOVE_SPEED);
			} break;
		}

		return r;
	}

	// Update Functions
	camera UpdateCameraPosition() {
		camera_precision x, y, z;
		m_qRotation.GetEulerAngles(&x, &y, &z);
				
		m_ptOrigin += GetLookVector() * m_cameraForwardSpeed;
		m_ptOrigin += GetRightVector() * m_cameraStrafeSpeed;
		m_ptOrigin += GetUpVector() * m_cameraUpSpeed;

		return (*this);
	}

	// Deviation vector is a vector of deviation from the origin point
	// So resulting point = ptOrigin + vDeviation
	// This does not affect orientation
	RESULT SetCameraPositionDeviation(vector vDeviation) {
		m_vDeviation = vDeviation;
		return R_PASS;
	}

	// Non-Event driven keyboard input
	RESULT UpdateFromKeyboardState(SenseKeyboard *pSK) {
		RESULT r = R_PASS;

		uint8_t state = pSK->GetKeyState(SK_LEFT);
		if (state) {
			DEBUG_LINEOUT("strafe");
			Strafe(0.1f);
		}

		state = pSK->GetKeyState(SK_RIGHT);
		//pSK->CheckKeyState((SK_SCAN_CODE)('D'))
		if (state) {
			Strafe(-0.1f);
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

protected:
	// Projection
	int m_pxScreenWidth;
	int m_pxScreenHeight;
	camera_precision m_NearPlane;
	camera_precision m_FarPlane;
	PROJECTION_MATRIX_TYPE m_ProjectionType;
	camera_precision m_FielfOfViewAngle;		// Note this is in degrees, not radians

	// TODO: Move to virtual object?
	camera_precision m_cameraRotateSpeed;
	camera_precision m_cameraForwardSpeed;
	camera_precision m_cameraStrafeSpeed;
	camera_precision m_cameraUpSpeed;

	vector m_vDeviation;
};

#endif // ! CAMERA_H_