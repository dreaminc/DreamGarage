#ifndef STEREO_CAMERA_H_
#define STEREO_CAMERA_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/stereocamera.h
// Stereo Camera Primitive
// The stereo camera is a dual camera with it's own orientation and position

#include "camera.h"

#define DEFAULT_PUPILLARY_DISTANCE 55

enum EYE_TYPE {
	EYE_LEFT,
	EYE_RIGHT,
	EYE_MONO,
	EYE_INVALID
};

class viewport;

class stereocamera : public camera {
public:
	stereocamera(point ptOrigin, viewport cameraVieport);

	point GetEyePosition(EYE_TYPE eye);

	ProjectionMatrix GetProjectionMatrix(EYE_TYPE eye);

	virtual point GetOrigin(bool fAbsolute = false) override;
	virtual point GetPosition(bool fAbsolute = false) override;

	ViewMatrix GetViewMatrix(EYE_TYPE eye);

private:
	camera_precision m_pupillaryDistance;	//  Distance between eyes (in mm)
};

#endif // ! STEREO_CAMERA_H_