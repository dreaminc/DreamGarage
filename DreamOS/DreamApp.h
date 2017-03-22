#ifndef DREAM_APP_H_
#define DREAM_APP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamApp.h
// The base Dream App object

#include "Primitives/valid.h"
#include "Primitives/Types/UID.h"

#include <string>

class DreamOS;
class composite;

class DreamApp : public valid {
	enum class UPDATE_MODEL {
		UPDATE_ON_SIGNAL,
		UPDATE_ON_FRAME,
		UPDATE_ON_TIME,
		UPDATE_INVALID
	};
public:
	DreamApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamApp();

	RESULT Initialize();

	virtual RESULT InitializeApp(void *pContext = nullptr) = 0;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) = 0;
	virtual RESULT Update(void *pContext = nullptr) = 0;

protected:
	RESULT SetAppName(std::string strAppName);
	RESULT SetAppDescription(std::string strAppDescription);
	DreamOS *GetDOS();

private:
	composite *m_pCompositeContext;
	DreamOS *m_pDreamOS;
	void *m_pContext = nullptr;

private:
	std::string m_strAppName;
	std::string m_strAppDescription;
	UID m_uid;
};

#endif // ! DREAM_APP_H_