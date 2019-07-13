#ifndef OBJECT_STORE_NODE_H_
#define OBJECT_STORE_NODE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Scene/ObjectStoreNode.h
// A Source Node extension of ObjectStore

#include "HAL/Pipeline/SourceNode.h"
#include "ObjectStore.h"

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