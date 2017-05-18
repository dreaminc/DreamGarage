#ifndef UI_KEYBOARD_H_
#define UI_KEYBOARD_H_

#include "DreamApp.h"
#include "Primitives/TextEntryString.h"
#include "Primitives/Publisher.h"
#include "Sense/SenseKeyboard.h"

#include "UI/UIKeyboardLayout.h"
#include "UI/UIMallet.h"

#include <vector>
#include <string>

class quad;
class sphere;
class text;
class Font;
class texture;
class CollisionManifold;
class FlatContext;

class UIKeyboard : public DreamApp<UIKeyboard>, public SenseKeyboard {
	friend class DreamAppManager;

public:
	UIKeyboard(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	RESULT InitializeQuadsWithLayout(UIKeyboardLayout *pLayout);
	RESULT InitializeTexturesWithLayout(LayoutType type);

//DreamApp
public:
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static UIKeyboard* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

//Animation
public:
	RESULT ShowKeyboard();
	RESULT HideKeyboard();
	bool IsVisible();
	RESULT SetVisible(bool fVisible);

private:
	RESULT ReleaseKey(UIKey *pKey);
	RESULT HideSurface();
	UIKey* CollisionPointToKey(point ptCollision);

//SenseKeyboard
public:
	RESULT UpdateKeyStates();
	virtual RESULT UpdateKeyState(SenseVirtualKey key, uint8_t keyState) override;
	RESULT CheckKeyState(SenseVirtualKey key);

//Active Keys
private:
	bool IsActiveKey(UIKey *pKey);
	RESULT AddActiveKey(UIKey *pKey);
	RESULT RemoveActiveKey(UIKey *pKey);
	RESULT ClearActiveKeys();

//Dynamic Resizing
public:
	float GetWidth();
	RESULT SetWidth(float width);
	float GetHeight();
	RESULT SetHeight(float height);

	RESULT SetKeyTypeThreshold(float threshold);
	RESULT SetKeyReleaseThreshold(float threshold);

private:
	RESULT UIKeyboard::UpdateViewQuad();
	RESULT UIKeyboard::UpdateAppComposite();
	RESULT UIKeyboard::UpdateKeyboardLayout(LayoutType kbType);

public:
	RESULT UpdateTextBox(int chkey);

private:
	std::shared_ptr<quad> m_pSurface;
	float m_surfaceWidth;
	float m_surfaceHeight;

	point m_ptSurfaceOffset;
	quaternion m_qSurfaceOrientation;
	float m_surfaceDistance;

	UIMallet *m_pLeftMallet;
	UIMallet *m_pRightMallet;

	std::shared_ptr<quad> m_pTextBox;
	std::shared_ptr<texture> m_pTextBoxTexture;

	std::shared_ptr<composite> m_pTextBoxContainer;
	std::map<unsigned int, texture*> m_keyCharAtlas;
	std::map<unsigned int, texture*> m_keyTextureAtlas;

	FlatContext *m_pQuadTextures;

	float m_keyTypeThreshold;
	float m_keyReleaseThreshold;

	//TODO: this should be dynamic
	UIKey* m_keyObjects[2];
	std::list<UIKey*> m_activeKeys;

	std::shared_ptr<Font> m_pFont;
	std::shared_ptr<texture> m_pKeyTexture;

	LayoutType m_currentLayout;
	UIKeyboardLayout *m_pLayout;
};

#endif // ! UI_KEYBOARD_H_
