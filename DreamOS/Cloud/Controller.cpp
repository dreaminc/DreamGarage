#include "Controller.h"
#include "CloudMessage.h"

ControllerMethod::ControllerMethod() :
	m_strMethodName(),
	m_fnOnMethod(nullptr)
{
	// empty
}

ControllerMethod::ControllerMethod(std::string strMethodName, std::function<RESULT(std::shared_ptr<CloudMessage>)> m_fnOnMethod) :
	m_strMethodName(strMethodName),
	m_fnOnMethod(m_fnOnMethod)
{
	// empty
}

ControllerMethod::~ControllerMethod() {
	// empty
}

Controller::Controller(Controller *pParentController, CloudController *pParentCloudController) :
	m_pParentController(pParentController),
	m_pParentCloudController(pParentCloudController)
{
	// empty
}

Controller* Controller::GetParentController() {
	return m_pParentController;
}

CloudController* Controller::GetCloudController() {
	if (m_pParentCloudController != nullptr) {
		return m_pParentCloudController;
	}
	else if (m_pParentController != nullptr) {
		return m_pParentController->GetCloudController();
	}
	else {
		return nullptr;
	}
}

RESULT Controller::RegisterMethod(std::string strMethodName, std::function<RESULT(std::shared_ptr<CloudMessage>)> fnOnMethod) {
	RESULT r = R_PASS;

	auto itMethod = m_methods.find(strMethodName);
	CBM((itMethod == m_methods.end()), "Method %s already exists", strMethodName.c_str());

	m_methods[strMethodName] = ControllerMethod(strMethodName, fnOnMethod);

Error:
	return r;
}

RESULT Controller::HandleOnMethodCallback(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;
	
	std::string strMethodName = pCloudMessage->GetMethod();
	auto s = pCloudMessage->GetJSONDataString();

	auto itMethod = m_methods.find(strMethodName);
	CBM((itMethod != m_methods.end()), "Method %s not found", strMethodName.c_str());

	CR(m_methods[strMethodName].m_fnOnMethod(pCloudMessage));

Error:
	return r;
}