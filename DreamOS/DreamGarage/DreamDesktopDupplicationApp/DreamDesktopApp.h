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

#include "InteractionEngine/InteractionObjectEvent.h"
#include "Sense/SenseController.h"
#include "Primitives/TextEntryString.h"

#define DESKTOP_PXWIDTH 1920;
#define DESKTOP_PXHEIGHT 1080;

class quad;
class texture;

class DreamUserHandle;
class AudioPacket;

class DreamDesktopApp :
	public DreamApp<DreamDesktopApp>
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

	// InteractionObjectEvent
	//virtual RESULT Notify(InteractionObjectEvent *pEvent) override;

	RESULT SetPosition(point ptPosition);
	RESULT SetAspectRatio(float aspectRatio);
	RESULT SetDiagonalSize(float diagonalSize);
	RESULT SetNormalVector(vector vNormal);
	RESULT SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal);
	RESULT StartDuplicationProcess();
	RESULT SendStartDesktopDuplicationIPCMessage();

	//RESULT FadeQuadToBlack();

	RESULT OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth);

	float GetWidth();
	float GetHeight();
	vector GetNormal();
	point GetOrigin();
//	virtual float GetAspectRatio() override;

	RESULT UpdateViewQuad();

	bool IsVisible();
	RESULT SetVisible(bool fVisible);

	std::shared_ptr<texture> GetScreenTexture();

	size_t m_frameDataBuffer_n = 0;
	unsigned char* m_pFrameDataBuffer = nullptr;

private:
	RESULT SetScreenTexture(texture *pTexture);

protected:
	static DreamDesktopApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<quad> m_pDesktopQuad = nullptr;
	std::shared_ptr<texture> m_pDesktopTexture = nullptr;

	DreamUserHandle* m_pDreamUserHandle = nullptr;

	int m_pxDesktopWidth = DESKTOP_PXWIDTH;
	int m_pxDesktopHeight = DESKTOP_PXHEIGHT;
	float m_aspectRatio = 1.0f;
	float m_diagonalSize = 5.0f;
	vector m_vNormal;

	double m_msTimeSinceLastSent = 0;
	double m_msTimeDelay = 2000;

	bool m_fDesktopDuplicationIsRunning = false;

	// Window
	HWND m_hwndDreamHandle = nullptr;
	HWND m_hwndDesktopHandle = nullptr;
};

#endif // ! DREAM_CONTENT_VIEW_H_

