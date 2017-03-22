#ifndef DREAM_CONTENT_VIEW_H_
#define DREAM_CONTENT_VIEW_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGarage/DreamApp.h
// The Dream Content view is a rudimentary Dream application 
// that effectively is a single quad that can load / present 
// content of various formats 

#include "DreamApp.h"
#include "Primitives/Subscriber.h"
#include "InteractionEngine/InteractionObjectEvent.h"

class quad;

class DreamContentView : public DreamApp, public Subscriber<InteractionObjectEvent> {
public:
	DreamContentView(DreamOS *pDreamOS, void *pContext = nullptr);

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;

	virtual RESULT Notify(InteractionObjectEvent *event) override;

private:
	quad *m_pScreenQuad = nullptr;

};

#endif // ! DREAM_CONTENT_VIEW_H_