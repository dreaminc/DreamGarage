#ifndef DREAM_2D_MOUSE_APP_H_
#define DREAM_2D_MOUSE_APP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dream2DMouseApp.h

#include "DreamApp.h"
#include "DreamAppHandle.h"

#include <map>
#include <vector>

class DimRay;

class Dream2DMouseAppHandle : public DreamAppHandle {
public:
//	virtual RESULT;
};

class Dream2DMouseApp : public DreamApp<Dream2DMouseApp>, public Dream2DMouseAppHandle {
	friend class DreamAppManager;

public:
	Dream2DMouseApp(DreamOS *pDreamOS, void *pContext = nullptr);

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	//RESULT SetVisible(bool fVisible);

	RESULT SetTestingValue(int val);
	int GetTestingValue();

protected:
	static Dream2DMouseApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	// Member vars go here
	DimRay *m_pMouseRay = nullptr;
};

#endif // ! DREAM_2D_MOUSE_APP_H_