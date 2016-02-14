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
#define DEFAULT_CAMERA_ROTATE_SPEED 0.01f

#define DEFAULT_PROJECTION_TYPE PROJECTION_MATRIX_PERSPECTIVE

class camera : public VirtualObj {
public:
	camera(point ptOrigin, camera_precision FOV, int pxScreenWidth, int pxScreenHeight) :
		m_FielfOfViewAngle(FOV),
		m_ProjectionType(DEFAULT_PROJECTION_TYPE),
		m_NearPlane(DEFAULT_NEAR_PLANE),
		m_FarPlane(DEFAULT_FAR_PLANE),
		m_pxScreenWidth(pxScreenWidth),
		m_pxScreenHeight(pxScreenHeight),
		m_pitch(0.0f),
		m_yaw(0.0f),
		m_roll(0.0f),
		m_cameraRotateSpeed(DEFAULT_CAMERA_ROTATE_SPEED)
	{
		m_ptOrigin = ptOrigin;
		Update();
	}

	~camera() {
		// empty stub
	}

	RESULT ResizeCamera(int pxWidth, int pxHeight) {
		m_pxScreenWidth = pxWidth;
		m_pxScreenHeight = pxHeight;

		return Update();
	}
	
	RESULT Update() {
		RESULT r = R_PASS;

		// Update the matrices 
		m_ProjectionMatrix = ProjectionMatrix(m_ProjectionType, m_pxScreenWidth, m_pxScreenHeight, m_NearPlane, m_FarPlane, m_FielfOfViewAngle);
		m_ViewMatrix = ViewMatrix(m_ptOrigin, m_pitch, m_yaw, m_roll);

		// For later access, might want to rethink for performance
		RotationMatrix matrixRotation(m_pitch, m_yaw, m_roll);

		m_vLook = matrixRotation * vector(0.0f, 0.0f, 1.0f);
		m_vUp = matrixRotation * vector(0.0f, 1.0f, 0.0f);
		m_vRight = matrixRotation * vector(1.0f, 1.0f, 1.0f);

	Error:
		return r;
	}

	ProjectionMatrix GetProjectionMatrix() { return m_ProjectionMatrix;  }
	ViewMatrix GetViewMatrix() { return m_ViewMatrix; }
	matrix<camera_precision, 4, 4> GetProjectionViewMatrix() { return (m_ProjectionMatrix * m_ViewMatrix); }

	RESULT translate(matrix <point_precision, 4, 1> v) {
		RESULT r = R_PASS;

		CR(VirtualObj::translate(v));
		CR(Update());

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

		return Update();
	}

	RESULT RotateCameraByDiffXY(camera_precision dx, camera_precision dy) {
		m_pitch		-= dy * m_cameraRotateSpeed;
		m_yaw		-= dx * m_cameraRotateSpeed;
		//m_roll		= 0.0f * m_cameraRotateSpeed;

		return Update();
	}

private:
	// Projection
	int m_pxScreenWidth;
	int m_pxScreenHeight;
	camera_precision m_NearPlane;
	camera_precision m_FarPlane;
	PROJECTION_MATRIX_TYPE m_ProjectionType;
	camera_precision m_FielfOfViewAngle;		// Note this is in degrees, not radians
	ProjectionMatrix m_ProjectionMatrix;

	// View (origin point is in the Virtual Object Parent)
	// TODO: Move to virtual object?
	camera_precision m_cameraRotateSpeed;
	camera_precision m_pitch;	// x
	camera_precision m_yaw;		// y
	camera_precision m_roll;	// z
	ViewMatrix m_ViewMatrix;

public:
	vector m_vLook;
	vector m_vUp;
	vector m_vRight;
};

#endif // ! CAMERA_H_