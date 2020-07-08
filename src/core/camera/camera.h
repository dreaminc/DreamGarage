#ifndef CAMERA_H_
#define CAMERA_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/camera/camera.h

// Camera Primitive
// The camera is not a dimension object since it does not actually have any dimensions
// instead we've defined a virtual object that it inherits from

#include "memory"                     

#include "core/primitives/VirtualObj.h"
#include "core/primitives/vector.h"
#include "core/primitives/point.h"
#include "core/primitives/ray.h"
#include "core/primitives/viewport.h"
#include "core/primitives/quaternion.h"    

#include "core/matrix/ProjectionMatrix.h"
#include "core/matrix/ViewMatrix.h"

#ifdef FLOAT_PRECISION
	typedef float camera_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double camera_precision;
#endif

class DimObj;
class HMD;
class SenseKeyboard;

class composite;

enum class EYE_TYPE : uint8_t {
	EYE_LEFT,
	EYE_RIGHT,
	EYE_MONO,
	EYE_INVALID
};

class camera : public VirtualObj
{
public:
	camera(point ptOrigin, viewport cameraVieport);
	~camera();

	int GetViewWidth();
	int GetViewHeight();

	RESULT ResizeCamera(viewport cameraViewport);
	RESULT ResizeCamera(int pxWidth, int pxHeight);

	vector GetRightVector();
	vector GetLookVector();
	vector GetUpVector();

	vector GetLookVectorXZ();

	ProjectionMatrix GetProjectionMatrix();
	ViewMatrix GetViewMatrix();
	matrix<camera_precision, 4, 4> GetProjectionViewMatrix();

	RESULT RotateCameraByDiffXY(camera_precision dx, camera_precision dy);
	RESULT MoveForward(camera_precision amt);

	RESULT Strafe(camera_precision amt);
	RESULT MoveUp(camera_precision amt);
	RESULT MoveStrafe(camera_precision amt);

	RESULT SetStrafeSpeed(camera_precision speed);
	RESULT AddStrafeSpeed(camera_precision speed);
	RESULT SetForwardSpeed(camera_precision speed);
	RESULT AddForwardSpeed(camera_precision speed);
	RESULT SetUpSpeed(camera_precision speed);
	RESULT AddUpSpeed(camera_precision speed);

	// Deviation vector is a vector of deviation from the origin point
	// So resulting point = ptOrigin + vDeviation
	// This does not affect orientation
	RESULT SetCameraPositionDeviation(vector vDeviation);

	// Non-Event driven keyboard input
	RESULT UpdateFromKeyboardState(SenseKeyboard *pSK);

	composite *GetFrameOfReferenceComposite();

	quaternion GetWorldOrientation();

	RESULT AddObjectToFrameOfReferenceComposite(std::shared_ptr<DimObj> pDimObj);
	RESULT SetFrameOfReferenceComposite(composite *pComposite);

	RESULT SetHMD(HMD *pHMD);

	ray GetRay(double xPos, double yPos, double t = 0.0f);

	// TODO: update this with a time delta / delta movement 
	ray GetRay(int xPos, int yPos);			// This is assuming an integer screen position but really just calls the one above

	bool IsAllowedMoveByKeys();

	quaternion GetOffsetOrientation();
	RESULT SetOffsetOrientation(quaternion qOffset);

	bool HasHMD();

	virtual EYE_TYPE GetCameraEye() {
		return EYE_TYPE::EYE_MONO;
	}

protected:
	HMD *m_pHMD;

	// Projection
	viewport m_viewport;

	// TODO: Should these go into view port too?
	camera_precision m_NearPlane;
	camera_precision m_FarPlane;
	ProjectionMatrix::type m_ProjectionType;

	// TODO: Move to virtual object?
	// TODO: Should this even be in here?
	camera_precision m_cameraRotateSpeed;
	camera_precision m_cameraForwardSpeed;
	camera_precision m_cameraStrafeSpeed;
	camera_precision m_cameraUpSpeed;

	vector m_vDeviation;

	// Set up HMD frame of reference 
	composite *m_pCameraFrameOfReference;

	// allow camera movements using keyboard
	bool	m_allowMoveByKeys = false;
	
	quaternion m_qOffsetOrientation;
};

#endif // ! CAMERA_H_
