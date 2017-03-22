#include "UIBar.h"

#include "DreamConsole\DreamConsole.h"
#include "DreamOS.h"

#include <algorithm>

UIBar::UIBar(DreamOS *pDreamOS, IconFormat& iconFormat, LabelFormat& labelFormat, UIBarFormat& barFormat) :
	UIModule(pDreamOS),
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

	auto& currentMenu = m_pCurrentUILayer->GetMenuItems();
	auto& pItem = currentMenu[index];
	int size = static_cast<int>(currentMenu.size());
	bool fHeader = (index == size - 1);

	std::shared_ptr<composite> pContext = pItem->GetContext();
	std::shared_ptr<quad> pQuad = pItem->GetQuad();

	// Radial layout
	int radIndex = fHeader ? 0 : static_cast<int>(index);
	float radY = (m_barFormat.itemAngleY * M_PI / 180.0f) * -(radIndex);
	radY -= m_barFormat.itemStartAngleY * M_PI / 180.0f;
	quaternion rotY = quaternion::MakeQuaternionWithEuler(0.0f, radY, 0.0f);
	float yPos = fHeader ? m_barFormat.headerPosY : m_barFormat.itemPosY;
	float zPos = fHeader ? m_barFormat.menuPosZ * 1.15f : m_barFormat.menuPosZ;
	pContext->MoveTo(sin(radY) * zPos, yPos, cos(radY) * zPos);
	
	pContext->SetOrientation(rotY);

	float radX = (fHeader ? m_barFormat.headerAngleX : m_barFormat.itemAngleX) * M_PI / 180.0f;
	quaternion rotX = quaternion::MakeQuaternionWithEuler(radX, 0.0f, 0.0f);

	if (pQuad != nullptr) {
		pQuad->MoveTo(0.0f, 0.0f, 0.0f);
		pQuad->SetOrientation(rotX);
	}

	return R_PASS;
}

RESULT UIBar::UpdateCurrentUILayer(UILayerInfo& info) {
	RESULT r = R_PASS;

	m_pCompositeContext->ClearChildren(); //always removes all layers
	std::shared_ptr<UIMenuLayer> pLayer = nullptr;

	pLayer = CreateMenuLayer();
	CN(pLayer);

	size_t size = std::max(info.icons.size(), info.labels.size());

	for (size_t i = 0; i < size; i++) {
		auto iconFormat = m_iconFormat;
		auto labelFormat = m_labelFormat;
		
		if (i < info.icons.size()) {
			iconFormat.pTexture = info.icons[i];
			iconFormat.ptPosition = point(0.0f, 0.25f, 0.0f);
		}

		if (i < info.labels.size()) {
			labelFormat.strLabel = info.labels[i];
			labelFormat.ptPosition = point(0.0f, -0.9f, 0.0f);
		}

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

float UIBar::GetLargeItemScale() {
	return m_barFormat.itemScaleSelected;
}