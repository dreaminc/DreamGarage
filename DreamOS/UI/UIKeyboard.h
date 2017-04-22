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

// TODO: As these classes expand, separate into different files
class UIKey {
public:
	UIKey();
	UIKey(float left, float width, std::string& letter);
public:
	float m_left;	// left side of the key
	float m_width;  // width of the key
	std::string m_letter; // letter on the key
	std::shared_ptr<quad> m_pQuad; //key surface TODO this sux
};

class UIMallet {
public:
	UIMallet(DreamOS *pDreamOS);
	sphere *m_pHead;
};

class UIKeyboard : public DreamApp<UIKeyboard>, public Subscriber<InteractionObjectEvent> {
	friend class DreamAppManager;

public:
	UIKeyboard(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	RESULT InitializeQuadsWithLayout(std::vector<std::vector<UIKey*>> layout);

public:
	//DreamApp
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	//InteractionEngine
	virtual RESULT Notify(InteractionObjectEvent *oEvent) override;

public:

	RESULT ShowKeyboard();
	RESULT HideKeyboard();
	RESULT HideSurface();
	RESULT HideMallets();
	RESULT ShowMallets();
	bool IsVisible();
	RESULT SetVisible(bool fVisible);

	UIKey* CollisionPointToKey(CollisionManifold& manifold);

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

	std::vector<std::vector<UIKey*>> m_QWERTY;
};

#endif // ! UI_KEYBOARD_H_