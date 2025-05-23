#ifndef STEREO_CAMERA_H_
#define STEREO_CAMERA_H_

#include "core/ehm/RESULT.h"                       // for RESULT

// Dream Core
// DreamOS/Dimension/Primitives/stereocamera.h

// Stereo Camera Primitive
// The stereo camera is a dual camera with it's own orientation and position

#include "camera.h"

#include "core/primitives/point.h"                    // for point
#include "core/primitives/quaternion.h"               // for quaternion

#include "core/matrix/ProjectionMatrix.h"  // for ProjectionMatrix
#include "core/matrix/ViewMatrix.h"        // for ViewMatrix

#define DEFAULT_PUPILLARY_DISTANCE 55

class viewport;

// TODO: Revisit the two paths (consolidate given the new sink design)
class stereocamera : public camera {
public:
	stereocamera(point ptOrigin, viewport cameraVieport);

	point GetEyePosition(EYE_TYPE eye);

	ProjectionMatrix GetProjectionMatrix(EYE_TYPE eye);

	virtual point GetOrigin(bool fAbsolute = false) override;
	virtual point GetPosition(bool fAbsolute = false) override;

	ViewMatrix GetViewMatrix(EYE_TYPE eye);
	ViewMatrix GetViewMatrix(EYE_TYPE eye, quaternion qAdjust);

	virtual EYE_TYPE GetCameraEye() override;
	RESULT SetCameraEye(EYE_TYPE eye);
	RESULT SetHMDAdjustedPosition(point ptPosition);

private:
	EYE_TYPE m_eye;
	camera_precision m_pupillaryDistance;	//  Distance between eyes (in mm)

	bool m_fProjEyeInit[2] = {false, false};
	ProjectionMatrix m_projEye[2];
};

#endif // ! STEREO_CAMERA_H_