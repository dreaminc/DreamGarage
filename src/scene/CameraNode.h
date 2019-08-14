#ifndef CAMERA_NODE_H_
#define CAMERA_NODE_H_

#include "core/ehm/EHM.h"

// Dream Sandbox
// dos/src/scene/CameraNode.h

// A Source node extension of stereo camera (for now)

#include "core/camera/stereocamera.h"

#include "pipeline/SourceNode.h"

class ray;

// The different types of stores should be added here 

class CameraNode : public stereocamera, public SourceNode {
public:
	CameraNode(point ptOrigin, viewport cameraVieport);

	RESULT SetupConnections() override;
	RESULT ProcessNode(long frameID = 0) override;
};

#endif // ! OBJECT_STORE_NODE_H_