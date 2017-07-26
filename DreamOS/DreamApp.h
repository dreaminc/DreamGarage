#ifndef DREAM_APP_H_
#define DREAM_APP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamApp.h
// The base Dream App object

#include "Primitives/valid.h"
#include "Primitives/Types/UID.h"

#include <string>
#include <memory>

class DreamOS;
class composite;

class DreamAppBase {
	friend class DreamAppManager;
	friend struct DreamAppBaseCompare;

public:
	virtual RESULT InitializeApp(void *pContext = nullptr) = 0;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) = 0;
	virtual RESULT Update(void *pContext = nullptr) = 0;
	virtual RESULT Shutdown(void *pContext = nullptr) = 0;

protected:
	virtual void *GetAppContext() = 0;
	virtual RESULT Print() { return R_NOT_IMPLEMENTED; }

protected:
	RESULT SetPriority(int priority);
	int GetPriority();
	RESULT ResetTimeRun();
	RESULT IncrementTimeRun(double usTimeDelta);
	float GetTimeRun();
	float GetEffectivePriorityValue() const;

private:
	double m_usTimeRun = 0.0;
	int m_priority = 0;
};


// Using Fixed-priority preemptive scheduling: https://en.wikipedia.org/wiki/Fixed-priority_pre-emptive_scheduling
struct DreamAppBaseCompare {
	bool operator()(const std::shared_ptr<DreamAppBase> &lhsApp , const std::shared_ptr<DreamAppBase> &rhsApp) const {
		// Note: This is actually returning the lowest value (not highest)
		// Since priority is inverted
		return lhsApp->GetEffectivePriorityValue() < rhsApp->GetEffectivePriorityValue();
	}
};

template<class derivedAppType>
class DreamApp : public DreamAppBase, public valid {
	enum class UPDATE_MODEL {
		UPDATE_ON_SIGNAL,
		UPDATE_ON_FRAME,
		UPDATE_ON_TIME,
		UPDATE_INVALID
	};

public:
	enum class Axes : uint16_t {
		NONE = 0,
		X = 1 << 0,
		Y = 1 << 1,
		Z = 1 << 2,
		ALL = 0x7
	};

	DreamApp(DreamOS *pDreamOS, void *pContext = nullptr) :
		m_pDreamOS(pDreamOS),
		m_pCompositeContext(nullptr),
		m_pContext(pContext)
	{
		// Empty
	}

	~DreamApp() {
		// empty
	}

	RESULT Initialize();

	virtual RESULT InitializeApp(void *pContext = nullptr) = 0;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) = 0;
	virtual RESULT Update(void *pContext = nullptr) = 0;

protected:

	//template<class derivedAppType>
	//TODO: move to the source file
	RESULT UpdateCompositeWithCameraLook(float depth, float yPos) {

		composite *pComposite = GetComposite();
		auto pCamera = pComposite->GetCamera();
		vector vLook = pCamera->GetLookVector();

		vector vLookXZ = vector(vLook.x(), 0.0f, vLook.z()).Normal();
		point lookOffset = depth * vLookXZ + point(0.0f, yPos, 0.0f);

		pComposite->SetPosition(pCamera->GetPosition() + lookOffset);
		pComposite->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), vLookXZ));

		return R_PASS;
	}

	RESULT UpdateCompositeWithHands(float yPos, Axes handAxes = Axes::ALL) {
		RESULT r = R_PASS;

		composite *pComposite = GetComposite();
		auto pCamera = pComposite->GetCamera();
		vector vLook = pCamera->GetLookVector();

		vector vLookXZ = vector(vLook.x(), 0.0f, vLook.z()).Normal();
		vector vUp = vector(0.0f, 1.0f, 0.0f);

		hand *pLeftHand = GetDOS()->GetHand(hand::HAND_LEFT);
		hand *pRightHand = GetDOS()->GetHand(hand::HAND_RIGHT);

		uint16_t axes = static_cast<uint16_t>(handAxes);

		CN(pCamera);
		CN(pLeftHand);
		CN(pRightHand);
		{
			float dist = 0.0f;

			point ptCamera = pCamera->GetPosition();

			for (auto& hand : { pLeftHand, pRightHand }) {
				float handDist = 0.0f;
				point ptHand = hand->GetPosition();
				ptHand = (point)(inverse(RotationMatrix(vLookXZ, vUp)) * (ptHand - pCamera->GetOrigin(true)));
				//ptHand = (point)(inverse(RotationMatrix(pCamera->GetOrientation(true))) * (ptHand - pCamera->GetOrigin(true)));

				if ((axes & 1) != 0)
					handDist += pow(ptHand.x(), 2);
					//handDist += pow(ptCamera.x() - ptHand.x(), 2);
				if ((axes & 2) != 0)
					handDist += pow(ptHand.y(), 2);
					//handDist += pow(ptCamera.y() - ptHand.y(), 2);
				if ((axes & 4) != 0)
					handDist += pow(ptHand.z(), 2);
					//handDist += pow(ptCamera.z() - ptHand.z(), 2);

				if (handDist > dist)
					dist = handDist;
			}

			dist = pow(dist, 0.5f);

			point lookOffset = (dist) * vLookXZ + point(0.0f, yPos, 0.0f);

			pComposite->SetPosition(pCamera->GetPosition() + lookOffset);
			pComposite->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), vLookXZ));
		}

	Error:
		return r;

	}

	void *GetAppContext() {
		return m_pContext;
	}

	composite *GetComposite() {
		return m_pCompositeContext;
	}

	RESULT SetComposite(composite *pComposite) {
		m_pCompositeContext = pComposite;
		return R_PASS;
	}

	static derivedAppType* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr) {
		return derivedAppType::SelfConstruct(pDreamOS, pContext);
	};

	RESULT SetAppName(std::string strAppName) {
		m_strAppName = strAppName;
		return R_PASS;
	}

	RESULT SetAppDescription(std::string strAppDescription) {
		m_strAppDescription = strAppDescription;
		return R_PASS;
	}

	DreamOS *GetDOS() {
		return m_pDreamOS;
	}

	virtual RESULT Print() override {
		DEBUG_LINEOUT_RETURN("%s running %fus pri: %d", (m_strAppName.length() > 0) ? m_strAppName.c_str() : "DreamApp", GetTimeRun(), GetPriority());
		return R_PASS;
	}

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