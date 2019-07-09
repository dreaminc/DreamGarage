#ifndef CAMERA_NODE_H_
#define CAMERA_NODE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Scene/CameraNode.h
// A Source node extension of stereo camera (for now)

#include "HAL/Pipeline/SourceNode.h"
#include "Primitives/stereocamera.h"

class ray;

// The different types of stores should be added here 

class CameraNode : public stereocamera, public SourceNode {
public:
	CameraNode(point ptOrigin, viewport cameraVieport);

	RESULT SetupConnections() override;
	RESULT ProcessNode(long frameID = 0) override;
};

#endif // ! OBJECT_STORE_NODE_H_