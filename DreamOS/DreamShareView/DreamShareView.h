#ifndef DREAM_SHARE_VIEW_H_
#define DREAM_SHARE_VIEW_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"
#include "DreamAppHandle.h"

#define PX_WIDTH 1366
#define PX_HEIGHT 768

class quad;
class texture;

class DreamShareViewHandle : public DreamAppHandle {
public:
	// switch between loading screen and casted texture
	RESULT SendLoadingEvent();
	RESULT SendCastingEvent();

	RESULT SendShowEvent();
	RESULT SendHideEvent();

	// idea here is to set the texture being cast here while the 
	// texture is updated elsewhere
	RESULT SendCastTexture(std::shared_ptr<texture> pNewCastTexture);
private:
	virtual RESULT ShowLoadingTexture() = 0;
	virtual RESULT ShowCastingTexture() = 0;
	virtual RESULT Show() = 0;
	virtual RESULT Hide() = 0;
	virtual RESULT SetCastingTexture(std::shared_ptr<texture> pNewCastTexture) = 0;
};

class DreamShareView :
	public DreamApp<DreamShareView>,
	public DreamShareViewHandle
{
	friend class DreamAppManager;

public:
	DreamShareView(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamShareView();

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	// Handle Interface
	virtual RESULT ShowLoadingTexture() override;
	virtual RESULT ShowCastingTexture() override;
	virtual RESULT SetCastingTexture(std::shared_ptr<texture> pNewCastTexture) override;
	virtual RESULT Show() override;
	virtual RESULT Hide() override;

protected:
	static DreamShareView* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	const wchar_t *k_wszLoadingScreen = L"client-loading-1366-768.png";

private:
	std::shared_ptr<quad> m_pCastQuad = nullptr;
	std::shared_ptr<texture> m_pCastTexture = nullptr;
	std::shared_ptr<texture> m_pLoadingTexture = nullptr;

	int m_castpxWidth = PX_WIDTH;
	int m_castpxHeight = PX_HEIGHT;
	float m_aspectRatio;
	float m_diagonalSize = 9.0f;
};

#endif // ! DREAM_SHARE_VIEW_H_ 
