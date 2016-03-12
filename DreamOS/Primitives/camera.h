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

#define DEFAULT_NEAR_PLANE 1.0f
#define DEFAULT_FAR_PLANE 100.0f
#define DEFAULT_CAMERA_ROTATE_SPEED 0.002f

#define DEFAULT_PROJECTION_TYPE PROJECTION_MATRIX_PERSPECTIVE
//#define DEFAULT_PROJECTION_TYPE PROJECTION_MATRIX_ORTHOGRAPHIC

class camera : public VirtualObj {
public:
	camera(point ptOrigin, camera_precision FOV, int pxScreenWidth, int pxScreenHeight) :
		m_FielfOfViewAngle(FOV),
		m_ProjectionType(DEFAULT_PROJECTION_TYPE),
		m_NearPlane(DEFAULT_NEAR_PLANE),
		m_FarPlane(DEFAULT_FAR_PLANE),
		m_pxScreenWidth(pxScreenWidth),
		m_pxScreenHeight(pxScreenHeight),
		m_cameraRotateSpeed(DEFAULT_CAMERA_ROTATE_SPEED)
	{
		m_ptOrigin = ptOrigin;
		m_qRotation = quaternion(0.0f, 0.0f, -1.0f, 0.0f);

		Update();
	}

	~camera() {
		// empty stub
	}

	RESULT ResizeCamera(int pxWidth, int pxHeight) {
		m_pxScreenWidth = pxWidth;
		m_pxScreenHeight = pxHeight;
		return R_PASS;
	}

	matrix<camera_precision, 4, 1> GetUpVector() {
		RotationMatrix matrixRotation(m_qRotation);
		return matrixRotation * vector(0.0f, 1.0f, 0.0f);
	}

	matrix<camera_precision, 4, 1> GetRightVector() {
		RotationMatrix matrixRotation(m_qRotation);
		return matrixRotation * vector(1.0f, 0.0f, 0.0f);
	}

	vector GetLookVector() {
		return m_qRotation.GetVector();
	}

	ProjectionMatrix GetProjectionMatrix() { 
		return ProjectionMatrix(m_ProjectionType, m_pxScreenWidth, m_pxScreenHeight, m_NearPlane, m_FarPlane, m_FielfOfViewAngle);
	}

	ViewMatrix GetViewMatrix() { 
		//ViewMatrix matIdentity = ViewMatrix(m_ptOrigin, m_pitch, m_yaw, m_roll);
		ViewMatrix mat = ViewMatrix(m_ptOrigin, m_qRotation);
		
		return mat;
	}

	matrix<camera_precision, 4, 4> GetProjectionViewMatrix() { 
		return (GetProjectionMatrix() * GetViewMatrix());
	}

	/* Now done in Virtual Object
	RESULT translate(matrix <point_precision, 4, 1> v) {
		RESULT r = R_PASS;

		VirtualObj::translate(v);
		Update();

	Error:
		return r;
	}

	RESULT translate(point_precision x, point_precision y, point_precision z) {
		RESULT r = R_PASS;
		
		VirtualObj::translate(x, y, z);
		CR(Update());

	Error:
		return r;
	}

	RESULT MoveTo(point p) {
		RESULT r = R_PASS;

		VirtualObj::MoveTo(p);
		CR(Update());

	Error:
		return r;
	}

	RESULT MoveTo(point_precision x, point_precision y, point_precision z) {
		RESULT r = R_PASS;

		VirtualObj::MoveTo(x, y, z);
		CR(Update());

	Error:
		return r;
	}

	RESULT UpdatePosition() {
		RESULT r = R_PASS;

		VirtualObj::UpdatePosition();
		CR(Update());
	
	Error:
		return r;
	}
	*/

	/*  All replaced by virtual object functions
	RESULT IncrementPitch(camera_precision incPitch) {
		m_pitch += incPitch;
		return Update();
	}

	RESULT IncrementYaw(camera_precision incYaw) {
		m_yaw += incYaw;
		return Update();
	}

	RESULT IncrementRoll(camera_precision incRoll) {
		m_roll += incRoll;
		return Update();
	}

	RESULT IncrementPYR(matrix<camera_precision, 3, 1> v) {
		m_pitch += v(0);
		m_yaw   += v(1);
		m_roll  += v(2);

		return Update();
	}

	RESULT SetPYR(matrix<camera_precision, 3, 1> v) {
		m_pitch = v(0);
		m_yaw = v(1);
		m_roll = v(2);

		return R_PASS;
	}
	*/

	RESULT RotateCameraByDiffXY(camera_precision dx, camera_precision dy) {
		//m_pitch		-= dy * m_cameraRotateSpeed;
		//m_yaw		-= dx * m_cameraRotateSpeed;
		//m_roll		= 0.0f * m_cameraRotateSpeed;

		RotateYBy(dx * m_cameraRotateSpeed);
		RotateXBy(dy * m_cameraRotateSpeed);

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

	RESULT Notify(SenseKeyboardEvent *kbEvent) {
		RESULT r = R_PASS;

		DEBUG_LINEOUT("Key %d state: %x", kbEvent->KeyCode, kbEvent->KeyState);

		switch (kbEvent->KeyCode) {
			case (SK_SCAN_CODE)('A') :
			case SK_LEFT: {
				/*
				if (kbEvent->KeyState)
				m_pCamera->AddVelocity(0.1f, 0.0f, 0.0f);
				else
				m_pCamera->AddVelocity(-0.1f, 0.0f, 0.0f);
				*/
				if (kbEvent->KeyState)
					Strafe(0.1f);

			} break;

			case (SK_SCAN_CODE)('D') :
			case SK_RIGHT: {
				/*
				if (kbEvent->KeyState)
				m_pCamera->AddVelocity(-0.1f, 0.0f, 0.0f);
				else
				m_pCamera->AddVelocity(0.1f, 0.0f, 0.0f);
				*/
				if (kbEvent->KeyState)
					Strafe(-0.1f);
			} break;

			case (SK_SCAN_CODE)('W') :
			case SK_UP: {
				/*
				if (kbEvent->KeyState)
				m_pCamera->AddVelocity(0.0f, 0.0f, 0.1f);
				else
				m_pCamera->AddVelocity(0.0f, 0.0f, -0.1f);
				*/
				if (kbEvent->KeyState)
					MoveForward(0.1f);
			} break;

			case (SK_SCAN_CODE)('S') :
			case SK_DOWN: {
				/*
				if (kbEvent->KeyState)
				m_pCamera->AddVelocity(0.0f, 0.0f, -0.1f);
				else
				m_pCamera->AddVelocity(0.0f, 0.0f, 0.1f);
				*/
				if (kbEvent->KeyState)
					MoveForward(-0.1f);
			} break;
		}

	Error:
		return r;
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

		//CR(UpdatePosition());

	Error:
		return r;
	}

private:
	// Projection
	int m_pxScreenWidth;
	int m_pxScreenHeight;
	camera_precision m_NearPlane;
	camera_precision m_FarPlane;
	PROJECTION_MATRIX_TYPE m_ProjectionType;
	camera_precision m_FielfOfViewAngle;		// Note this is in degrees, not radians

	

	// View (origin point is in the Virtual Object Parent)
	// TODO: Move to virtual object?
	camera_precision m_cameraRotateSpeed;

	/*
public:
	vector m_vLook;
	vector m_vUp;
	vector m_vRight;
	*/
};

#endif // ! CAMERA_H_