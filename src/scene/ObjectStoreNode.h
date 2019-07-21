#ifndef OBJECT_STORE_NODE_H_
#define OBJECT_STORE_NODE_H_

#include "core/ehm/EHM.h"

// Dream Scene
// dos/src/scene/ObjectStoreNode.h

// A Source Node extension of ObjectStore

#include "ObjectStore.h"

#include "pipeline/SourceNode.h"

class ray;

// The different types of stores should be added here 

class ObjectStoreNode : public ObjectStore, public SourceNode {
public:

	ObjectStoreNode(ObjectStoreFactory::TYPE type);
	ObjectStoreNode();


	RESULT SetupConnections() override;
	RESULT ProcessNode(long frameID = 0) override;
};

#endif // ! OBJECT_STORE_NODE_H_