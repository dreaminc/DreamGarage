#include "UIBar.h"

#include "DreamConsole\DreamConsole.h"

#include <algorithm>

UIBar::UIBar(composite* pComposite, IconFormat& iconFormat, LabelFormat& labelFormat, UIBarFormat& barFormat) :
	UIModule(pComposite),
	m_iconFormat(iconFormat),
	m_labelFormat(labelFormat),
	m_barFormat(barFormat)
{
	// empty
}

UIBar::~UIBar() {
	// empty
}

RESULT UIBar::UpdateWithRadialLayout(size_t index) {

	auto currentMenu = m_currentUILayer->GetMenuItems();
	auto pItem = currentMenu[index];
	int size = static_cast<int>(currentMenu.size());
	bool fHeader = (index == size - 1);

	std::shared_ptr<composite> pContext = pItem->GetContext();
	std::shared_ptr<quad> pQuad = pItem->GetQuad();

	// used to center the UIBar
	int shift = size / 2;
	float odd = (size % 2 == 0) ? 0.5f : 0.0f;

	// Radial layout
	int radIndex = fHeader ? 0 : static_cast<int>(index);
	float radY = (m_barFormat.itemAngleY * M_PI / 180.0f) * -(radIndex - shift + odd);
	quaternion rotY = quaternion::MakeQuaternionWithEuler(0.0f, radY, 0.0f);
	float yPos = fHeader ? m_barFormat.headerPosY : m_barFormat.itemPosY;
	pContext->MoveTo(0.0f, yPos, 0.0f);
	pContext->SetOrientation(rotY);

	float radX = (fHeader ? m_barFormat.headerAngleX : m_barFormat.itemAngleX) * M_PI / 180.0f;
	quaternion rotX = quaternion::MakeQuaternionWithEuler(radX, 0.0f, 0.0f);
	pQuad->MoveTo(0.0f, 0.0f, m_barFormat.menuPosZ);
	pQuad->SetOrientation(rotX);

	return R_PASS;
}

RESULT UIBar::HandleMenuUp(UILayerInfo& info) {
	RESULT r = R_PASS;

	if (info.icons.size() == 0 && info.labels.size() == 0) {
		ToggleVisible();
		return r;
	}
	UpdateCurrentUILayer(info);

	return r;
}

RESULT UIBar::HandleTriggerUp(UILayerInfo& info) {
	RESULT r = R_PASS;

	if (info.icons.size() == 0 && info.labels.size() == 0) {
		return r;
	}
	UpdateCurrentUILayer(info);

	return r;
}

RESULT UIBar::UpdateSelectedItem(size_t index) {
	size_t size = (m_currentUILayer) ? m_currentUILayer->GetMenuItems().size()-1 : 0;

	if (m_selectedIndex != index) {
		// swap enlarged menu item
		if (0 <= index && index < size) {
			auto& p = m_currentUILayer->GetMenuItems()[index];
			auto& pItemContext = p->GetContext();
			auto& pItemQuad = p->GetQuad();
			if (pItemContext != nullptr) {
				pItemQuad->ScaleX(m_barFormat.itemScale.x() * m_barFormat.itemScaleSelected);
				pItemQuad->ScaleZ(m_barFormat.itemScale.z() * m_barFormat.itemScaleSelected);
			}
		}
		if (0 <= m_selectedIndex && m_selectedIndex < size) {
			auto& p = m_currentUILayer->GetMenuItems()[m_selectedIndex];
			auto& pItemContext = p->GetContext();
			auto& pItemQuad = p->GetQuad();
			if (pItemContext != nullptr) {
				pItemQuad->ScaleX(m_barFormat.itemScale.x());
				pItemQuad->ScaleZ(m_barFormat.itemScale.z());
			}
		}
		m_selectedIndex = index;
	}
	return R_PASS;
}

int UIBar::GetIndexFromRay(ray handRay) {
	point ptContext = handRay.GetOrigin() - m_pContext->GetOrigin();
	ptContext = point(ptContext.x(), 0.0f, ptContext.z());
	ray contextRay = ray(ptContext, handRay.GetVector());
	point intersect = FurthestRaySphereIntersect(contextRay, point(0.0f, 0.0f, 0.0f), m_barFormat.menuPosZ); 
	vector vIntersect = vector(intersect);
	vIntersect.Normalize();

	// size works under the assumption that there is one header object
	size_t size = (m_currentUILayer) ? m_currentUILayer->GetMenuItems().size()-1 : 0;
	size_t shift = size / 2;
	float odd = (size % 2 != 0) ? 0.5f : 0.0f;
	size_t numSections = size != 0 ? 360 / static_cast<int>(m_barFormat.itemAngleY) : 1; 
	float deg = static_cast<float>(-atan2(vIntersect.x(), vIntersect.z()) * 180.0f / M_PI);
	deg -= m_headRotationYDeg;

	// index can't be unsigned in this situation
	int index = static_cast<int>(-(deg / m_barFormat.itemAngleY - odd) + shift) % numSections; 
	OVERLAY_DEBUG_SET("index", index);
	
	return index;
}

RESULT UIBar::Update(ray handRay) {
	
	// convert ray into context's space
	UpdateSelectedItem(GetIndexFromRay(handRay));

	return R_PASS;
}

point UIBar::FurthestRaySphereIntersect(const ray &r, point center, float radius) {
//*
	vector vRayCircle = static_cast<ray>(r).ptOrigin() - center;

	float bValue = static_cast<ray>(r).vDirection().dot(vRayCircle);
	float cValue = vRayCircle.dot(vRayCircle) - pow(radius, 2.0f); 
	float resultValue = pow(bValue, 2.0f) - cValue;

	OVERLAY_DEBUG_SET("res", resultValue);

	if (resultValue >= 0.0f) {
		float term = pow(resultValue, 0.5f);
		float t1 = (-1.0f) * bValue + term;
		//this result will always be smaller
		//float t2 = (-1.0f) * bValue - term;
		return r.GetOrigin() + point(t1 * r.GetVector());
	}
	//*/
	return point(0.0f, 0.0f, 0.0f);
}

RESULT UIBar::UpdateCurrentUILayer(UILayerInfo& info) {
	RESULT r = R_PASS;

	m_pContext->ClearChildren(); //always removes all layers
	std::shared_ptr<UIMenuLayer> pLayer;
	CN(m_pContext);

	pLayer = CreateMenuLayer();
	CN(pLayer);

	size_t size = std::max(info.icons.size(), info.labels.size());

	for (size_t i = 0; i < size; i++) {
		auto iconFormat = m_iconFormat;
		auto labelFormat = m_labelFormat;
		if (i < info.icons.size()) {
			iconFormat.pTexture = info.icons[i];
			iconFormat.position = point(0.0f, 0.5f, 0.5f);
		}
		if (i < info.labels.size())
			labelFormat.label = info.labels[i];
		auto pItem = pLayer->CreateMenuItem();
		pItem->Update(iconFormat, labelFormat);

	}
	// currently, must wait until all items are created before positioning
	for(size_t i = 0; i < size; i++) {
		UpdateWithRadialLayout(i);
	}

Error:
	return R_PASS;
}

size_t UIBar::GetSelectedIndex() {
	return m_selectedIndex;
}