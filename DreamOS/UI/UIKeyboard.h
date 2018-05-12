#ifndef UI_KEYBOARD_H_
#define UI_KEYBOARD_H_

#include "DreamApp.h"
#include "DreamAppHandle.h"
#include "Primitives/TextEntryString.h"
#include "Primitives/Publisher.h"
#include "Sense/SenseKeyboard.h"

#include "UI/UIKeyboardLayout.h"
#include "UI/UIMallet.h"
#include "DreamUserApp.h"
#include "DreamUserControlArea/DreamUserControlArea.h"

#include <vector>
#include <string>

#define SURFACE_WIDTH 0.523f //0.5f // surface is a quad that holds the entire keyboard layout
#define SURFACE_HEIGHT 0.294f // 0.25f
#define SURFACE_ANGLE 30.0f

#define TEXTBOX_LINE_HEIGHT 0.027f // text box records what has been typed
#define TEXTBOX_NUM_LINES 1.0f
#define TEXTBOX_MARGIN 0.02f

#define TITLE_LINE_HEIGHT 0.05f
#define	TITLE_ICON_WIDTH 0.068f
#define TITLE_ICON_HEIGHT (TITLE_ICON_WIDTH * (3.0f / 4.0f))

#define KEY_TYPE_THRESHOLD 0.0f 
#define KEY_RELEASE_THRESHOLD -0.025f
#define KEY_SCALE (5.0f / 6.0f)
#define KEY_MARGIN 0.25f

#define KEY_RELEASE_DURATION 0.1f
#define ANIMATION_DURATION 0.1f
#define ANIMATION_OFFSET_HEIGHT 0.25f

#define AMBIENT_INTENSITY 0.75f

class quad;
class sphere;
class text;
class font;
class texture;
class CollisionManifold;
class FlatContext;

class UIKeyboardHandle : public DreamAppHandle {
public:
	RESULT Show();
	RESULT Hide();
	RESULT SendUpdateComposite(float depth);
	RESULT SendUpdateComposite(float depth, point ptOrigin, quaternion qOrigin);
	bool IsVisible();
	RESULT UpdateTitleView(texture *pIconTexture, std::string strTitle);
	RESULT ShowTitleView();
	RESULT PopulateTextBox(std::string strText);
	RESULT SendPasswordFlag(bool fIsPassword);

private:
	virtual RESULT ShowKeyboard() = 0;
	virtual RESULT HideKeyboard() = 0;
	virtual RESULT UpdateComposite(float depth) = 0;
	virtual RESULT UpdateComposite(float depth, point ptOrigin, quaternion qOrigin) = 0;
	virtual bool IsKeyboardVisible() = 0;
	virtual RESULT UpdateKeyboardTitleView(texture *pIconTexture, std::string strTitle) = 0;
	virtual RESULT ShowKeyboardTitleView() = 0;
	virtual RESULT PopulateKeyboardTextBox(std::string strText) = 0;
	virtual RESULT SetPasswordFlag(bool fIsPassword) = 0;
};

class UIKeyboard :	public DreamApp<UIKeyboard>, 
					public UIKeyboardHandle, 
					public SenseKeyboard {
	friend class DreamAppManager;
	friend class DreamUserControlArea;

public:
	UIKeyboard(DreamOS *pDreamOS, void *pContext = nullptr);

	enum class state {	// For tracking if keyboard is animating or not
		HIDDEN,
		ANIMATING,
		VISIBLE
	};

private:
	RESULT InitializeQuadsWithLayout(UIKeyboardLayout *pLayout);
	RESULT InitializeLayoutTexture(LayoutType type);

public:
	RESULT InitializeWithParent(DreamUserControlArea *pParent);

//DreamApp
public:
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;

	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	virtual DreamAppHandle* GetAppHandle() override;

protected:
	static UIKeyboard* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

//Animation
public:
	virtual RESULT ShowKeyboard() override;
	virtual RESULT HideKeyboard() override;

	virtual bool IsKeyboardVisible() override;

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
	float GetAngle();
	RESULT SetSurfaceAngle(float angle);

	RESULT SetKeyTypeThreshold(float threshold);
	RESULT SetKeyReleaseThreshold(float threshold);
	RESULT SetSurfaceOffset(point ptOffset);

private:
	RESULT UIKeyboard::UpdateViewQuad();
	RESULT UIKeyboard::UpdateKeyboardLayout(LayoutType kbType);

public:
	RESULT SetAnimatingState(UIKeyboard::state keyboardState);
	RESULT UpdateTextBox(int chkey);
	virtual RESULT PopulateKeyboardTextBox(std::string strText) override;
	virtual RESULT UpdateKeyboardTitleView(texture *pIconTexture, std::string strTitle) override;
	virtual RESULT ShowKeyboardTitleView() override;
	RESULT UpdateComposite(float depth, point ptOrigin, quaternion qOrigin) override;
	RESULT UpdateComposite(float depth); // update position/orientation

	virtual RESULT SetPasswordFlag(bool fIsPassword) override;

private:
	// layout variables
	float m_surfaceWidth = SURFACE_WIDTH;
	float m_surfaceHeight = SURFACE_HEIGHT;
	float m_surfaceAngle = SURFACE_ANGLE;

	float m_lineHeight = TEXTBOX_LINE_HEIGHT;
	float m_numLines = TEXTBOX_NUM_LINES;

	float m_keyScale = KEY_SCALE;
	float m_keyMargin = KEY_MARGIN;

	point m_ptComposite;
	quaternion m_qSurfaceOrientation;

	float m_keyTypeThreshold = KEY_TYPE_THRESHOLD;
	float m_keyReleaseThreshold = KEY_RELEASE_THRESHOLD;

	float m_keyReleaseDuration = KEY_RELEASE_DURATION;
	float m_animationDuration = ANIMATION_DURATION;
	float m_animationOffsetHeight = ANIMATION_OFFSET_HEIGHT;

	float m_ambientIntensity = AMBIENT_INTENSITY;
	float m_textboxMargin = TEXTBOX_MARGIN;

	float m_titleLineHeight = TITLE_LINE_HEIGHT;
	float m_titleIconWidth = TITLE_ICON_WIDTH;
	float m_titleIconHeight = TITLE_ICON_HEIGHT;

	std::shared_ptr<composite> m_pSurfaceContainer;
	std::shared_ptr<quad> m_pSurface;

	std::shared_ptr<text> m_pTextBoxText;
	std::shared_ptr<quad> m_pTextBoxBackground;
	std::shared_ptr<text> m_pTitleText;
	std::shared_ptr<quad> m_pTitleIcon;
	std::shared_ptr<composite> m_pHeaderContainer;

	std::map<LayoutType, text*> m_layoutAtlas;

	UIKeyboard::state m_keyboardState;

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
	std::shared_ptr<texture> m_pDefaultIconTexture;

	LayoutType m_currentLayout;
	UIKeyboardLayout *m_pLayout;

	DreamUserHandle *m_pUserHandle = nullptr;
	UID m_userAppUID;

	DreamUserControlArea *m_pParentApp = nullptr;
};

#endif // ! UI_KEYBOARD_H_
