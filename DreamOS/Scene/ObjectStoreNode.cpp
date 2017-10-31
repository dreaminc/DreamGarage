#include "ObjectStoreNode.h"

ObjectStoreNode::ObjectStoreNode(ObjectStoreFactory::TYPE type) :
	ObjectStore(type),
	SourceNode("objectstore")
{
	// empty
}

ObjectStoreNode::ObjectStoreNode() :
	ObjectStore(),
	SourceNode("objectstore")
{
	// empty
}

RESULT ObjectStoreNode::SetupConnections() {
	RESULT r = R_PASS;

	// No inputs

	// Outputs
	CR(MakeOutput<ObjectStore>("objectstore", this));

Error:
	return r;
}

RESULT ObjectStoreNode::ProcessNode(long frameID) {
	return R_SKIPPED;
}