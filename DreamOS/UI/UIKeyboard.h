#ifndef UI_KEYBOARD_H_
#define UI_KEYBOARD_H_

#include "DreamApp.h"
#include "Primitives/Subscriber.h"
#include "InteractionEngine/InteractionObjectEvent.h"
#include "InteractionEngine/ActiveObject.h"

#include "UI/UIKeyboardLayout.h"
#include "UI/UIMallet.h"

#include <vector>

class quad;
class sphere;
class text;
class Font;
class texture;
class CollisionManifold;

class UIKeyboard : public DreamApp<UIKeyboard>, public Subscriber<InteractionObjectEvent> {
	friend class DreamAppManager;

public:
	UIKeyboard(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	RESULT InitializeQuadsWithLayout();

public:
	//DreamApp
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	//InteractionEngine
	virtual RESULT Notify(InteractionObjectEvent *oEvent) override;

//Animation
public:
	RESULT ShowKeyboard();
	RESULT HideKeyboard();
	RESULT HideSurface();
	bool IsVisible();
	RESULT SetVisible(bool fVisible);

	UIKey* CollisionPointToKey(CollisionManifold& manifold);

//Dynamic Resizing
public:
	float GetWidth();
	RESULT SetWidth(float width);
	float GetHeight();
	RESULT SetHeight(float height);

private:
	RESULT UIKeyboard::UpdateViewQuad();

protected:
	static UIKeyboard* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<quad> m_pSurface;
	float m_surfaceWidth;
	float m_surfaceHeight;
	point m_ptSurface;

	UIMallet *m_pLeftMallet;
	UIMallet *m_pRightMallet;

	std::string m_typed;

	ActiveObject::state m_keyStates[2];
	std::shared_ptr<quad> m_keyObjects[2];

	std::shared_ptr<Font> m_pFont;
	std::shared_ptr<texture> m_pKeyTexture;

	UIKeyboardLayout *m_pLayout;
};

#endif // ! UI_KEYBOARD_H_