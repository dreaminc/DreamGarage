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
#include "DreamControlView/UIControlBar.h"

#include <vector>
#include <string>

#define SURFACE_WIDTH 0.75 // surface is a quad that holds the entire keyboard layout
#define SURFACE_HEIGHT 0.294f // 0.25f
#define SURFACE_ANGLE 30.0f

#define TEXTBOX_LINE_HEIGHT 0.027f // text box records what has been typed
#define TEXTBOX_NUM_LINES 1.0f
#define TEXTBOX_MARGIN 0.02f

#define KEY_TYPE_THRESHOLD 0.0f 
#define KEY_RELEASE_THRESHOLD -0.025f
#define KEY_SCALE (5.0f / 6.0f)
#define KEY_MARGIN 0.25f

#define KEY_RELEASE_DURATION 0.1f
#define ANIMATION_DURATION 0.1f
#define ANIMATION_OFFSET_HEIGHT 0.25f

#define AMBIENT_INTENSITY 0.75f

// hack to allow the control bar buttons associated with the keyboard to send key codes
#define SVK_SHIFTTAB 0x02
#define SVK_CLOSE 0x03

class quad;
class sphere;
class text;
class font;
class texture;
class CollisionManifold;
class FlatContext;
class SoundFile;


class UIKeyboard : public DreamApp<UIKeyboard>,
	public SenseKeyboard {
	friend class DreamAppManager;
	friend class DreamUserControlArea;

public:
	UIKeyboard(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	RESULT InitializeQuadsWithLayout(UIKeyboardLayout *pLayout);
	RESULT InitializeLayoutTexture(LayoutType type);
	RESULT InitializeControlBar();

public:
	RESULT InitializeWithParent(DreamUserControlArea *pParent);

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
	virtual RESULT Show();
	virtual RESULT Hide();

	bool IsVisible();
	RESULT SetVisible(bool fVisible);

private:
	RESULT PressKey(UIKey *pKey, ControllerType type);
	RESULT ReleaseKey(UIKey *pKey);
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

public:
	RESULT HandleTabPressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleBackTabPressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleDonePressed(UIButton* pButtonContext, void* pContext);

	RESULT UpdateTabNextTexture(bool fCanTabNext);
	RESULT UpdateTabPreviousTexture(bool fCanTabPrevious);

	std::shared_ptr<UIControlBar> GetControlBar();
private:
	std::shared_ptr<UIControlBar> m_pUIControlBar = nullptr;

private:
	RESULT UpdateViewQuad();
	RESULT UpdateKeyboardLayout(LayoutType kbType);

public:
	RESULT UpdateTextBox(int chkey);
	RESULT PopulateKeyboardTextBox(std::string strText);
	RESULT UpdateTitleView(texture *pIconTexture, std::string strTitle);
	RESULT ShowTitleView();
	RESULT ShowBrowserButtons();
	RESULT UpdateComposite(float depth, point ptOrigin, quaternion qOrigin);
	RESULT UpdateComposite(float depth); // update position/orientation

	RESULT SetPasswordFlag(bool fIsPassword);

private:
	std::shared_ptr<SoundFile> m_pDefaultPressSound = nullptr;
	std::shared_ptr<SoundFile> m_pDeletePressSound = nullptr;
	std::shared_ptr<SoundFile> m_pReturnPressSound = nullptr;
	std::shared_ptr<SoundFile> m_pSpacePressSound = nullptr;

	std::map<unsigned int, std::shared_ptr<SoundFile>> m_keyPressSounds;

	// control bar textures
private:
	const wchar_t *k_wszTab = L"key-tab-next.png";
	const wchar_t *k_wszCantTab = L"key-tab-next-disabled.png";
	const wchar_t *k_wszBackTab = L"key-tab-previous.png";
	const wchar_t *k_wszCantBackTab = L"key-tab-previous-disabled.png";
	const wchar_t *k_wszDone = L"key-done.png";

private:
	std::shared_ptr<UIButton> m_pNextButton = nullptr;
	std::shared_ptr<UIButton> m_pPreviousButton = nullptr;
	std::shared_ptr<UIButton> m_pDoneButton = nullptr;

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
	std::shared_ptr<texture> m_pDefaultIconTexture;

	LayoutType m_currentLayout;
	UIKeyboardLayout *m_pLayout;

	DreamUserControlArea *m_pParentApp = nullptr;
};

#endif // ! UI_KEYBOARD_H_
