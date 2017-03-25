#ifndef DREAM_BROWSER_VIEW_H_
#define DREAM_BROWSER_VIEW_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGarage/DreamApp.h
// The Dream Content view is a rudimentary Dream application 
// that effectively is a single quad that can load / present 
// content of various formats 

#include "DreamApp.h"
#include "Primitives/Subscriber.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include <map>
#include <vector>

class quad;
class texture;

class EnvironmentAsset;

class DreamBrowser : public DreamApp<DreamBrowser>, public Subscriber<InteractionObjectEvent> {
	friend class DreamAppManager;

public:
	DreamBrowser(DreamOS *pDreamOS, void *pContext = nullptr);

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;

	// InteractionObjectEvent
	virtual RESULT Notify(InteractionObjectEvent *event) override;


	RESULT SetAspectRatio(float aspectRatio);
	RESULT SetDiagonalSize(float diagonalSize);
	RESULT SetNormalVector(vector vNormal);
	RESULT SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal);

	float GetWidth();
	float GetHeight();
	vector GetNormal();
	point GetOrigin();

	RESULT UpdateViewQuad();

	RESULT SetVisible(bool fVisible);

	RESULT SetURI(std::string strURI);

private:
	RESULT SetScreenTexture(texture *pTexture);

protected:
	static DreamBrowser* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<quad> m_pBrowserQuad = nullptr;
	float m_aspectRatio = 1.0f;
	float m_diagonalSize = 5.0f;
	vector m_vNormal;
};

#endif // ! DREAM_CONTENT_VIEW_H_