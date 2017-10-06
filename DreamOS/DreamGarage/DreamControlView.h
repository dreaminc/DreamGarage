#ifndef DREAM_CONTROL_VIEW_H_
#define DREAM_CONTROL_VIEW_H_

#include "RESULT/EHM.h"
#include "DreamApp.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include "Primitives/Subscriber.h"
#include <functional>
#include <stack>

class quad; 
class sphere;
class UIView;
class UIMallet;
class UIScrollView;
class texture;

class DreamControlView : public DreamApp<DreamControlView>, 
						 public Subscriber<InteractionObjectEvent> {
	friend class DreamAppManager;

public:
	DreamControlView(DreamOS *pDreamOS, void *pContext = nullptr);

	enum class State {
		HIDDEN,
		HIDE,
		VISIBLE,
		SHOW
	};

// DreamApp
public:
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	virtual RESULT Notify(InteractionObjectEvent *pInteractionEvent) override;

protected:
	static DreamControlView *SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

// Animations
private:
	// Called based entirely on head orientation.  Show is called if the user is 'looking down'
	RESULT Show();
	RESULT Hide();

// View Context
public:
	//When update is called the screen texture is updated to the texture of this object
	RESULT SetSharedViewContext();

	std::shared_ptr<quad> GetViewQuad();
	RESULT SetViewState(State state);

private:
	std::shared_ptr<quad> m_pViewQuad;
	vector m_vNormal;
	std::shared_ptr<texture> m_pViewTexture;
	std::shared_ptr<UIView> m_pView;
	std::shared_ptr<UIScrollView> m_pScrollView;

	State m_viewState;

	UIMallet *m_pLeftMallet;
	UIMallet *m_pRightMallet;

	float m_hiddenScale; 
	float m_visibleScale;

	point m_ptHiddenPosition;
	point m_ptVisiblePosition;

	float m_showThreshold;
	float m_hideThreshold;
};

#endif // ! DREAM_CONTROL_VIEW_H_