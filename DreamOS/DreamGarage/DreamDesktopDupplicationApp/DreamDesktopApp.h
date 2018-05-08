#ifndef DREAM_DESKTOP_H_
#define DREAM_DESKTOP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGarage/DesktopDuplicationApp
// The Desktop duplication app starts and manages
// the DreamDesktopCapture project, allowing us to
// duplicate the desktop and receive a texture from
// directx11 as a byte buffer

#include "DreamApp.h"
#include "DreamAppHandle.h"
#include "DreamUserControlArea/DreamContentSource.h"

#include "DDCIPCMessage.h"

#include "InteractionEngine/InteractionObjectEvent.h"
#include "Sense/SenseController.h"
#include "Primitives/TextEntryString.h"

#define DESKTOP_PXWIDTH 0;	// I don't think we need these anymore
#define DESKTOP_PXHEIGHT 0;

#define WSTRDREAMCAPTURELOCATION L"\\DreamDesktopCapture.exe"

class quad;
class texture;
class DreamUserControlArea;
class DreamUserHandle;
class AudioPacket;
class EnvironmentAsset;

class DreamDesktopApp :
	public DreamApp<DreamDesktopApp>,
	public DreamContentSource
{
	friend class DreamAppManager;

public:
	DreamDesktopApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamDesktopApp();

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	// Control Events
	virtual RESULT OnClick(point ptDiff, bool fMouseDown) override;
	virtual RESULT OnMouseMove(point mousePoint) override;
	virtual RESULT OnScroll(float pxXDiff, float pxYDiff, point scrollPoint) override;

	virtual RESULT OnKeyPress(char chkey, bool fkeyDown) override;

	virtual std::shared_ptr<texture> GetSourceTexture() override;

	virtual RESULT SetScope(std::string strScope) override;
	virtual RESULT SetPath(std::string strPath) override;
	virtual long GetCurrentAssetID() override;

	virtual int GetHeight() override;
	virtual int GetWidth() override;
	virtual std::string GetTitle() override;
	virtual std::string GetContentType() override;

	virtual RESULT CloseSource() override;

	// InteractionObjectEvent
	//virtual RESULT Notify(InteractionObjectEvent *pEvent) override;
	RESULT UpdateViewQuad();

	RESULT SetPosition(point ptPosition);
	RESULT SetAspectRatio(float aspectRatio);
	RESULT SetDiagonalSize(float diagonalSize);
	RESULT SetNormalVector(vector vNormal);
	RESULT SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal);
	RESULT StartDuplicationProcess();
	RESULT SendDesktopDuplicationIPCMessage(DDCIPCMessage::type msgType);

	RESULT SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	//RESULT FadeQuadToBlack();

	RESULT OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth);

	RESULT InitializeWithParent(DreamUserControlArea *pParentApp);

	float GetWidthFromAspectDiagonal();
	float GetHeightFromAspectDiagonal();
	vector GetNormal();
	point GetOrigin();

	bool IsVisible();
	RESULT SetVisible(bool fVisible);

	size_t m_frameDataBuffer_n = 0;
	unsigned char* m_pFrameDataBuffer = nullptr;

private:
	const wchar_t *k_wszLoadingScreen = L"client-loading-1366-768.png";

protected:
	static DreamDesktopApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<quad> m_pDesktopQuad = nullptr;
	std::shared_ptr<texture> m_pDesktopTexture = nullptr;
	std::shared_ptr<texture> m_pLoadingScreenTexture = nullptr;
	DreamUserHandle* m_pDreamUserHandle = nullptr;

	int m_pxDesktopWidth = DESKTOP_PXWIDTH;
	int m_pxDesktopHeight = DESKTOP_PXHEIGHT;
	float m_aspectRatio = 1.0f;
	float m_diagonalSize = 5.0f;
	vector m_vNormal;

	long m_assetID = -1;
	std::string m_strPath;
	std::string m_strScope;
	std::string m_strContentType;
	std::string m_strTitle = "Windows Desktop";

	double m_msTimeSinceLastSent = 0;
	double m_msMessageTimeDelay = 2000;
	double m_msTitleTimeDelay = 100;

	bool m_fShiftKey = false;

	bool m_fDesktopDuplicationIsRunning = false;
	DreamUserControlArea *m_pParentApp = nullptr;

	// Window
	HWND m_hwndDreamHandle = nullptr;
	HWND m_hwndDesktopHandle = nullptr;
	HANDLE m_dreamJobHandle = nullptr;
};

#endif // ! DREAM_CONTENT_VIEW_H_
