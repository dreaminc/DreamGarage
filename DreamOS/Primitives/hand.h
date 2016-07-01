#ifndef HAND_H_
#define HAND_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/composite.h
// Composite Primitive
// The composite object is a collection of other objects when these are constructed within the object itself 
// this is really a convenience object that should be used to derive others rather than off of DimObj directly

#include "composite.h"

typedef enum HandType {
	HAND_LEFT,
	HAND_RIGHT,
	HAND_INVALID
} HAND_TYPE;

class thumb : public composite {
public:
	thumb(HALImp* pHALImp) :
		composite(pHALImp)
	{
		Initialize();
	}

	RESULT Initialize() {
		RESULT r = R_PASS;

		float width = 0.25f;
		float height = 0.25f;
		float length = 1.0f;

		m_pMetacarpal = AddVolume(width, height, length);
		m_pMetacarpal->SetColor(color(COLOR_RED));
		m_pMetacarpal->UpdateBuffers();

		m_pProximalPhalanx = MakeVolume(0.25f, 0.25f, 1.0f);
		m_pProximalPhalanx->SetPivotPoint(0, length, 0);
		m_pProximalPhalanx->SetColor(color(COLOR_BLUE));
		m_pProximalPhalanx->UpdateBuffers();
		m_pMetacarpal->AddChild(m_pProximalPhalanx);

		m_pDistalPhalanx = MakeVolume(0.25f, 0.25f, 1.0f);
		m_pDistalPhalanx->SetPivotPoint(0, length, 0);
		m_pDistalPhalanx->SetColor(color(COLOR_YELLOW));
		m_pDistalPhalanx->UpdateBuffers();
		m_pProximalPhalanx->AddChild(m_pDistalPhalanx);

	Error:
		return r;
	}

private:
	std::shared_ptr<volume> m_pMetacarpal;
	std::shared_ptr<volume> m_pProximalPhalanx;
	std::shared_ptr<volume> m_pDistalPhalanx;
};

class finger : public composite {
public:
	finger(HALImp* pHALImp) :
		composite(pHALImp)
	{
		Initialize();
	}

	RESULT Initialize() {
		RESULT r = R_PASS;

		float width = 0.25f;
		float height = 0.25f;
		float length = 1.0f;

		m_pMetacarpal = AddVolume(width, height, length);
		m_pMetacarpal->SetColor(color(COLOR_RED));
		m_pMetacarpal->UpdateBuffers();

		m_pProximalPhalanx = MakeVolume(0.25f, 0.25f, 1.0f);
		m_pProximalPhalanx->SetPivotPoint(0, length, 0);
		m_pProximalPhalanx->SetColor(color(COLOR_BLUE));
		m_pProximalPhalanx->UpdateBuffers();
		m_pMetacarpal->AddChild(m_pProximalPhalanx);

		m_pIntermediatePhalanx = MakeVolume(0.25f, 0.25f, 1.0f);
		m_pIntermediatePhalanx->SetPivotPoint(0, length, 0);
		m_pIntermediatePhalanx->SetColor(color(COLOR_GREEN));
		m_pIntermediatePhalanx->UpdateBuffers();
		m_pProximalPhalanx->AddChild(m_pIntermediatePhalanx);

		m_pDistalPhalanx = MakeVolume(0.25f, 0.25f, 1.0f);
		m_pDistalPhalanx->SetPivotPoint(0, length, 0);
		m_pDistalPhalanx->SetColor(color(COLOR_YELLOW));
		m_pDistalPhalanx->UpdateBuffers();
		m_pIntermediatePhalanx->AddChild(m_pDistalPhalanx);

	Error:
		return r;
	}

private:
	std::shared_ptr<volume> m_pMetacarpal;
	std::shared_ptr<volume> m_pProximalPhalanx;
	std::shared_ptr<volume> m_pIntermediatePhalanx;
	std::shared_ptr<volume> m_pDistalPhalanx;
};

class hand : public composite {
public:
	hand(HALImp* pHALImp) :
		composite(pHALImp)
	{
		Initialize();
	}

	RESULT Initialize() {
		RESULT r = R_PASS;

		m_pPalm = AddVolume(1.0f, 0.5f, 0.25);

		m_pIndexFinger = std::shared_ptr<finger>(new finger(m_pHALImp));
		AddObject(m_pIndexFinger);

	Error:
		return r;
	}

private:
	std::shared_ptr<volume> m_pPalm;

	std::shared_ptr<finger> m_pIndexFinger;
	std::shared_ptr<finger> m_pMiddleFinger;
	std::shared_ptr<finger> m_pRingFinger;
	std::shared_ptr<finger> m_pPinkyFinger;
};

#endif	// ! HAND_H_