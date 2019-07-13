#include "CameraNode.h"

CameraNode::CameraNode(point ptOrigin, viewport cameraVieport) : 
	stereocamera(ptOrigin, cameraVieport),
	SourceNode("camera")
{
	// empty
}

RESULT CameraNode::SetupConnections() {
	RESULT r = R_PASS;

	// No inputs

	// Outputs
	CR(MakeOutput<stereocamera>("stereocamera", this));

Error:
	return r;
}

RESULT CameraNode::ProcessNode(long frameID) {
	return R_SKIPPED;
}