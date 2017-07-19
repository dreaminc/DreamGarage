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

#define SURFACE_WIDTH 0.5f // surface is a quad that holds the entire keyboard layout
#define SURFACE_HEIGHT 0.25f
#define SURFACE_ANGLE 30.0f

#define OFFSET_DEPTH 0.5f
#define OFFSET_HEIGHT -0.25f

#define TEXTBOX_LINE_HEIGHT 0.027f // text box records what has been typed
#define TEXTBOX_NUM_LINES 1.0f
//#define TEXTBOX_WIDTH 0.5f // textbox width matches surface width

#define KEY_TYPE_THRESHOLD 0.0f 
#define KEY_RELEASE_THRESHOLD -0.025f
#define KEY_SCALE 0.9f
#define KEY_MARGIN 0.25f

#define KEY_RELEASE_DURATION 0.1f
#define ANIMATION_DURATION 0.2f
#define ANIMATION_OFFSET_HEIGHT 1.0f

#define AMBIENT_INTENSITY 0.75f

class quad;
class sphere;
class text;
class font;
class texture;
class CollisionManifold;
class FlatContext;

class UIKeyboard : public DreamApp<UIKeyboard>, public SenseKeyboard {
	friend class DreamAppManager;

public:
	UIKeyboard(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	RESULT InitializeQuadsWithLayout(UIKeyboardLayout *pLayout);
	RESULT InitializeLayoutTexture(LayoutType type);

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
	RESULT SetSurfaceOffset(point ptOffset);

private:
	RESULT UIKeyboard::UpdateViewQuad();
	RESULT UIKeyboard::UpdateKeyboardLayout(LayoutType kbType);

public:
	RESULT UpdateTextBox(int chkey, std::string strEntered);
	RESULT UpdateTitle(texture *pIconTexture, std::string strTitle);
	RESULT UpdateComposite(); // update position/orientation

	//temp
	RESULT SetMallets(UIMallet *leftMallet, UIMallet *rightMallet);

	// TODO: Temo until better IPC
public:
	RESULT SetPath(std::string strPath);
	RESULT SetScope(std::string strScope);
	std::string GetPath();
	std::string GetScope();

	std::string m_strPath;
	std::string m_strScope;

private:
	// layout variables
	float m_surfaceWidth = SURFACE_WIDTH;
	float m_surfaceHeight = SURFACE_HEIGHT;

	float m_lineHeight = TEXTBOX_LINE_HEIGHT;
	float m_numLines = TEXTBOX_NUM_LINES;

	float m_keyScale = KEY_SCALE;
	float m_keyMargin = KEY_MARGIN;

	point m_ptSurfaceOffset = point(0.0f, OFFSET_HEIGHT, -OFFSET_DEPTH);
	quaternion m_qSurfaceOrientation;
	float m_offsetDepth = OFFSET_DEPTH;
	float m_offsetHeight = OFFSET_HEIGHT;

	float m_keyTypeThreshold = KEY_TYPE_THRESHOLD;
	float m_keyReleaseThreshold = KEY_RELEASE_THRESHOLD;

	float m_keyReleaseDuration = KEY_RELEASE_DURATION;
	float m_animationDuration = ANIMATION_DURATION;
	float m_animationOffsetHeight = ANIMATION_OFFSET_HEIGHT;

	float m_ambientIntensity = AMBIENT_INTENSITY;

	// objects
	UIMallet *m_pLeftMallet;
	UIMallet *m_pRightMallet;

	std::shared_ptr<composite> m_pSurfaceContainer;
	std::shared_ptr<quad> m_pSurface;

	std::shared_ptr<text> m_pTextBoxText;
	std::shared_ptr<quad> m_pTextBoxBackground;
	std::shared_ptr<text> m_pTitleText;
	std::shared_ptr<quad> m_pTitleIcon;
	std::shared_ptr<composite> m_pHeaderContainer;

	std::map<LayoutType, text*> m_layoutAtlas;

	//TODO: this should be dynamic
	UIKey* m_keyObjects[2];
	std::list<UIKey*> m_activeKeys;

	std::shared_ptr<font> m_pFont;
	//key textures
	std::shared_ptr<texture> m_pKeyTexture;
	std::shared_ptr<texture> m_pTextBoxTexture;
	std::shared_ptr<texture> m_pDeleteTexture;
	std::shared_ptr<texture> m_pLettersTexture;
	std::shared_ptr<texture> m_pNumbersTexture;
	std::shared_ptr<texture> m_pReturnTexture;
	std::shared_ptr<texture> m_pShiftTexture;
	std::shared_ptr<texture> m_pSpaceTexture;
	std::shared_ptr<texture> m_pSymbolsTexture;
	std::shared_ptr<texture> m_pUnshiftTexture;

	LayoutType m_currentLayout;
	UIKeyboardLayout *m_pLayout;
};

#endif // ! UI_KEYBOARD_H_
