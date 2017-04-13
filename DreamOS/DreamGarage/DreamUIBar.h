#ifndef DREAM_UI_BAR_H_
#define DREAM_UI_BAR_H_

#include "DreamApp.h"

#include "UI/UIBar.h"
#include "Primitives/composite.h"

#include <functional>

#include "Cloud/Menu/MenuController.h"
#include "Cloud/Menu/MenuNode.h"
#include "Cloud/Environment/EnvironmentController.h"

class DreamUIBar : public DreamApp<DreamUIBar>, public UIBar, public MenuController::observer, public Subscriber<InteractionObjectEvent> {
	friend class DreamAppManager;

public:
	DreamUIBar(DreamOS *pDreamOS, void *pContext = nullptr);

	RESULT SetParams(
				const IconFormat& iconFormat, 
				const LabelFormat& labelFormat, 
				const RadialLayerFormat& menuFormat,
				const RadialLayerFormat& titleFormat);
	~DreamUIBar();

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	RESULT HandleTouchStart(void* pContext);
	RESULT HandleTouchMove(void* pContext);
	RESULT HandleTouchEnd(void* pContext);

	RESULT HandleMenuUp(void* pContext);
	RESULT HandleSelect(void* pContext);

	// Callback signature
	// RESULT fnEventCallback(struct (opt) pEventInfo, void* pContext)

	RESULT RegisterEvent(InteractionEventType type, std::function<RESULT(void*)> fnCallback);

	std::map<InteractionEventType, std::function<RESULT(void*)>> m_callbacks;

	virtual RESULT Notify(InteractionObjectEvent *event) override;

// Menu Controller Observer
	RESULT OnMenuData(std::shared_ptr<MenuNode> pMenuNode);

protected:
	static DreamUIBar* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	//Cloud member variables
	CloudController *m_pCloudController = nullptr;
	MenuControllerProxy *m_pMenuControllerProxy = nullptr;
	EnvironmentControllerProxy *m_pEnvironmentControllerProxy = nullptr;

	std::shared_ptr<MenuNode> m_pMenuNode = nullptr;

	std::stack<std::shared_ptr<MenuNode>> m_pathStack = {};
	std::map<MenuNode::MimeType, std::shared_ptr<texture>> m_images;
};


#endif // ! DREAM_UI_BAR_H_