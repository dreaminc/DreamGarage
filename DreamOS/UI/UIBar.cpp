#include "UIBar.h"

#include "DreamConsole\DreamConsole.h"
#include "DreamOS.h"

#include <algorithm>

UIBar::UIBar(DreamOS *pDreamOS, const IconFormat& iconFormat, const LabelFormat& labelFormat, const RadialLayerFormat& menuFormat, const RadialLayerFormat& titleFormat) :
	UIModule(pDreamOS),
	m_iconFormat(iconFormat),
	m_labelFormat(labelFormat),
	m_menuFormat(menuFormat),
	m_titleFormat(titleFormat)
{
	// empty
}

UIBar::~UIBar() {
	// empty
}

RESULT UIBar::UpdateWithRadialLayout(size_t index, const RadialLayerFormat& layerFormat) {

	auto& currentMenu = m_pCurrentUILayer->GetMenuItems();
	auto& pItem = currentMenu[index];

	std::shared_ptr<composite> pContext = pItem->GetContext();
	std::shared_ptr<quad> pQuad = pItem->GetQuad();

	// Radial layout
	int radIndex = static_cast<int>(index);
	float radY = (layerFormat.itemAngleY * M_PI / 180.0f) * -(radIndex);
	radY -= layerFormat.itemStartAngleY * M_PI / 180.0f;
	quaternion qContext = quaternion::MakeQuaternionWithEuler(0.0f, radY, 0.0f);

	float yPos = layerFormat.itemPosY;
	float zPos = layerFormat.menuPosZ;
	point ptContext = point(sin(radY) * zPos, yPos, cos(radY) * zPos);
	
	float radX = layerFormat.itemAngleX * M_PI / 180.0f;
	quaternion qQuad = quaternion::MakeQuaternionWithEuler(radX, 0.0f, 0.0f);

	point ptQuad = point(0.0f, 0.0f, 0.0f);

	pItem->SetObjectParams(ptQuad, qQuad, ptContext, qContext);

	return R_PASS;
}

// currently destroys and recreates the menu based off of the new information
//
RESULT UIBar::UpdateUILayers(const UILayerInfo& currentInfo, const UILayerInfo& titleInfo) {
	RESULT r = R_PASS;
	CR(m_pCompositeContext->ClearChildren()); //always removes all layers

	CR(UpdateCurrentUILayer(titleInfo, m_titleFormat));
	CR(UpdateCurrentUILayer(currentInfo, m_menuFormat));

Error:
	return r;
}

RESULT UIBar::UpdateCurrentUILayer(const UILayerInfo& info, const RadialLayerFormat& layerFormat) {
	RESULT r = R_PASS;

	std::shared_ptr<UIMenuLayer> pLayer = nullptr;
	pLayer = CreateMenuLayer();
	CN(pLayer);

	size_t size = std::max(info.icons.size(), info.labels.size());

	for (size_t i = 0; i < size; i++) {
		auto iconFormat = m_iconFormat;
		auto labelFormat = m_labelFormat;
		
		if (i < info.icons.size()) iconFormat.pTexture = info.icons[i];
		if (i < info.labels.size()) labelFormat.strLabel = info.labels[i];

		auto pItem = pLayer->CreateMenuItem();
		CN(pItem);
		CR(pItem->Update(iconFormat, labelFormat));
	}

	// currently, must wait until all items are created before positioning
	for(size_t i = 0; i < size; i++) {
		CR(UpdateWithRadialLayout(i, layerFormat));
	}

Error:
	return R_PASS;
}

float UIBar::GetLargeItemScale() {
	return m_menuFormat.itemScaleSelected;
}