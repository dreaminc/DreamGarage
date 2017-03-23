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

#include <map>
#include <vector>

class quad;
class texture;

#define ASPECT_RATIO_16_9  ((16.0f)/(9.0f))
#define ASPECT_RATIO_4_3 ((4.0f)/(3.0f))
#define DEFAULT_ASPECT_RATIO ASPECT_RATIO_16_9
#define DEFAULT_DIAGONAL_SIZE 1.0f

class DreamContentView : public DreamApp<DreamContentView>, public Subscriber<InteractionObjectEvent> {
	friend class DreamAppManager;

public:
	enum class AspectRatio {
		ASPECT_16_9,
		ASPECT_4_3,
		ASPECT_INVALID
	};

	std::map<AspectRatio, float> k_aspectRatios = {
		{AspectRatio::ASPECT_16_9, ASPECT_RATIO_16_9},
		{AspectRatio::ASPECT_4_3, ASPECT_RATIO_4_3}
	};

public:
	DreamContentView(DreamOS *pDreamOS, void *pContext = nullptr);

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;

	virtual RESULT Notify(InteractionObjectEvent *event) override;

	RESULT SetAspectRatio(float aspectRatio);
	RESULT SetDiagonalSize(float diagonalSize);
	RESULT SetNormalVector(vector vNormal);

	RESULT SetParams(point ptPosition, float diagonal, const AspectRatio aspectRatio, vector vNormal);

	float GetWidth();
	float GetHeight();
	vector GetNormal();
	point GetOrigin();

	RESULT UpdateViewQuad();

	RESULT SetScreenTexture(const std::wstring &wstrTextureFilename);
	RESULT SetScreenURI(const std::string &strURI);

private:
	RESULT SetScreenTexture(texture *pTexture);
	RESULT HandleOnFileResponse(std::shared_ptr<std::vector<uint8_t>> pBufferVector);

protected:
	static DreamContentView* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<quad> m_pScreenQuad = nullptr;
	float m_aspectRatio = DEFAULT_ASPECT_RATIO;
	float m_diagonalSize = DEFAULT_DIAGONAL_SIZE;
	vector m_vNormal;


	std::shared_ptr<std::vector<uint8_t>> m_pPendingBufferVector = nullptr;
};

#endif // ! DREAM_CONTENT_VIEW_H_