#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Cloud/Controller.h
// The base controller object

#include <map>
#include <memory>
#include <string>
#include <functional>

class CloudController;
class CloudMessage;

class ControllerMethod {
	friend class Controller;
public:
	ControllerMethod();
	ControllerMethod(std::string strMethodName, std::function<RESULT(std::shared_ptr<CloudMessage>)> m_fnOnMethod);
	~ControllerMethod();

protected:
	std::string m_strMethodName;
	std::function<RESULT(std::shared_ptr<CloudMessage>)> m_fnOnMethod;
	UID m_uid;
};

class Controller {
public:
	Controller(Controller *pParentController = nullptr, CloudController *pParentCloudController = nullptr);

	virtual ~Controller() {}

	virtual RESULT Initialize() = 0;

	friend class Controller;

protected:
	Controller* GetParentController();
	CloudController *GetCloudController();

protected:
	RESULT RegisterMethod(std::string strMethodName, std::function<RESULT(std::shared_ptr<CloudMessage>)> fnOnMethod);
	RESULT HandleOnMethodCallback(std::shared_ptr<CloudMessage> pCloudMessage);

private:
	Controller* m_pParentController;
	CloudController *m_pParentCloudController;

	// Map of Response Methods
private:
	std::map<std::string, ControllerMethod> m_methods;

private:
	UID m_uid;
};

#endif	// !CONTROLLER_H_