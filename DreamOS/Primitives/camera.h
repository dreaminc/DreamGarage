#ifndef CAMERA_H_
#define CAMERA_H_

#include "RESULT/EHM.h"
#include "DreamConsole/DreamConsole.h"

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


#include "Primitives/Subscriber.h"
#include "Sense/SenseKeyboard.h"
#include "HMD/HMD.h"

class camera : public VirtualObj, public Subscriber<CmdPromptEvent>, public Subscriber<SenseKeyboardEvent>, public Subscriber<HMDEvent>, public Subscriber<TimeEvent> {
public:
	camera(point ptOrigin, camera_precision FOV, int pxScreenWidth, int pxScreenHeight);

	~camera();

	RESULT ResizeCamera(int pxWidth, int pxHeight);

	vector GetUpVector();

	int GetPXWidth();

	int GetPXHeight();

	vector GetRightVector();

	vector GetLookVector();

	ProjectionMatrix GetProjectionMatrix();

	ViewMatrix GetViewMatrix();

	matrix<camera_precision, 4, 4> GetProjectionViewMatrix();

	RESULT RotateCameraByDiffXY(camera_precision dx, camera_precision dy);

	RESULT MoveForward(camera_precision amt);

	RESULT MoveUp(camera_precision amt);

	RESULT MoveStrafe(camera_precision amt);

	RESULT SetStrafeSpeed(camera_precision speed);

	RESULT AddStrafeSpeed(camera_precision speed);

	RESULT SetForwardSpeed(camera_precision speed);

	RESULT AddForwardSpeed(camera_precision speed);

	RESULT SetUpSpeed(camera_precision speed);

	RESULT AddUpSpeed(camera_precision speed);

	RESULT Notify(HMDEvent *hmdEvent);

	RESULT Notify(SenseKeyboardEvent *kbEvent);

	RESULT Notify(TimeEvent *event);

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

	int GetScreenWidth();

	int GetScreenHeight();

	bool	IsAllowedMoveByKeys();

	// CmdPromptEventSubscriber
	virtual RESULT Notify(CmdPromptEvent *event) override;

protected:
	HMD *m_pHMD;

	// Projection
	int m_pxScreenWidth;
	int m_pxScreenHeight;
	camera_precision m_NearPlane;
	camera_precision m_FarPlane;
	PROJECTION_MATRIX_TYPE m_ProjectionType;
	camera_precision m_FieldOfViewAngle;		// Note this is in degrees, not radians

	// TODO: Move to virtual object?
	camera_precision m_cameraRotateSpeed;
	camera_precision m_cameraForwardSpeed;
	camera_precision m_cameraStrafeSpeed;
	camera_precision m_cameraUpSpeed;

	vector m_vDeviation;

	// Set up HMD frame of reference 
	composite *m_pCameraFrameOfReference;

	// allow camera movements using keyboard
	bool	m_allowMoveByKeys = false;
};

#endif // ! CAMERA_H_