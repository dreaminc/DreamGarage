#ifndef UI_KEYBOARD_H_
#define UI_KEYBOARD_H_

#include "DreamApp.h"
#include "Primitives/Subscriber.h"
#include "InteractionEngine/InteractionObjectEvent.h"
#include "InteractionEngine/ActiveObject.h"

#include <vector>

class quad;
class sphere;
class text;
class Font;
class texture;
class CollisionManifold;
//class InteractionObjectEvent;

struct UIMallet {
	sphere *pHead;
};

class UIKeyboard : public DreamApp<UIKeyboard>, public Subscriber<InteractionObjectEvent> {
	friend class DreamAppManager;

public:
	UIKeyboard(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	RESULT InitializeQuadsWithLayout(std::vector<std::string> layout);

public:
	//DreamApp
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	//InteractionEngine
	virtual RESULT Notify(InteractionObjectEvent *oEvent) override;

public:
	int CollisionPointToIndex(CollisionManifold& manifold);

protected:
	static UIKeyboard* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

//	RESULT UpdateKeys();

private:
	std::shared_ptr<quad> m_pSurface;
	float m_surfaceWidth;
	float m_surfaceHeight;

	UIMallet *m_pLeftMallet;
	UIMallet *m_pRightMallet;

	std::string m_typed;

	std::vector<ActiveObject::state> m_keyStates;
	std::vector<std::shared_ptr<quad>> m_keyObjects;

	std::vector<std::shared_ptr<quad>> m_keys;
	std::shared_ptr<Font> m_pFont;
	std::shared_ptr<texture> m_pKeyTexture;

	std::vector<std::string> m_QWERTY;
};

#endif // ! UI_KEYBOARD_H_