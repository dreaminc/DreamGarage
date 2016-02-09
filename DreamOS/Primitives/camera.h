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

#ifdef FLOAT_PRECISION
	typedef float camera_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double camera_precision;
#endif

#define DEFAULT_NEAR_PLANE 1.0f
#define DEFAULT_FAR_PLANE 100.0f

#define DEFAULT_PROJECTION_TYPE PROJECTION_MATRIX_PERSPECTIVE

class camera : public VirtualObj {
public:
	camera(point ptOrigin, vector vLook, vector vUp, camera_precision FOV, int pxScreenWidth, int pxScreenHeight) :
		m_FielfOfViewAngle(FOV),
		m_ProjectionType(DEFAULT_PROJECTION_TYPE),
		m_NearPlane(DEFAULT_NEAR_PLANE),
		m_FarPlane(DEFAULT_FAR_PLANE),
		m_pxScreenWidth(pxScreenWidth),
		m_pxScreenHeight(pxScreenHeight)
	{
		m_ptOrigin = ptOrigin;

		Update();
	}

	~camera() {
		// empty stub
	}
	
	RESULT Update() {
		RESULT r = R_PASS;

		// Update the matrices 
		m_ProjectionMatrix = ProjectionMatrix(m_ProjectionType, m_pxScreenWidth, m_pxScreenHeight, m_NearPlane, m_FarPlane, m_FielfOfViewAngle);

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
	ProjectionMatrix m_ProjectionMatrix;

	// View (origin is in the Virtual Object Parent)
	vector m_vLook;
	vector m_vUp;
	vector m_vRight;
	matrix<camera_precision, 4, 4> m_ViewMatrix;
};

#endif // ! CAMERA_H_