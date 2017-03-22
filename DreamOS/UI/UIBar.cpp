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
	m_pIconTexture = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"icon_jpg_300.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	//m_pIconTexture = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR));
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

	// used to center the UIBar
	int shift = (size-1) / 2;
	float odd = ((size-1) % 2 == 0) ? 0.5f : 0.0f;

	// Radial layout
	int radIndex = fHeader ? 0 : static_cast<int>(index);
	float radY = (m_barFormat.itemAngleY * M_PI / 180.0f) * -(radIndex - shift + odd);
	quaternion rotY = quaternion::MakeQuaternionWithEuler(0.0f, radY, 0.0f);
	float yPos = fHeader ? m_barFormat.headerPosY : m_barFormat.itemPosY;
	pContext->MoveTo(sin(radY) * m_barFormat.menuPosZ, yPos, cos(radY) * m_barFormat.menuPosZ);
	
	pContext->SetOrientation(rotY);

	float radX = (fHeader ? m_barFormat.headerAngleX : m_barFormat.itemAngleX) * M_PI / 180.0f;
	quaternion rotX = quaternion::MakeQuaternionWithEuler(radX, 0.0f, 0.0f);

	if (pQuad != nullptr) {
		pQuad->MoveTo(0.0f, 0.0f, 0.0f);
		pQuad->SetOrientation(rotX);
	}

	return R_PASS;
}

RESULT UIBar::HandleMenuUp(std::map<std::string, std::vector<std::string>>& menu, std::stack<std::string>& path) {
	RESULT r = R_PASS;

	// pull up menu
	UILayerInfo info;
	// go back
	if (!path.empty()) {
		path.pop();
		if (!path.empty()) {
			std::string& str = path.top();
			info.labels = menu[str];
			info.labels.emplace_back(str);
			for (int i = 0; i < info.labels.size(); i++) {
				info.icons.emplace_back(m_pIconTexture);
			}
		}
	}
	// open menu
	else {
		info.labels = menu[""];
		info.labels.emplace_back(""); // fake header for root menu
		for (int i = 0; i < info.labels.size(); i++) {
			info.icons.emplace_back(m_pIconTexture);
		}
		ToggleVisible();
		path.push("");
	}
	if (info.icons.size() == 0 && info.labels.size() == 0) {
		CR(ToggleVisible());
		return r;
	}

	CR(UpdateCurrentUILayer(info));

Error:
	return r;
}

RESULT UIBar::HandleTriggerUp(std::map<std::string, std::vector<std::string>>& menu, std::stack<std::string>& path) {
	RESULT r = R_PASS;

	UILayerInfo info;
	std::shared_ptr<UIMenuItem> pSelected = GetCurrentItem();
	CB(!path.empty());
	CBR(pSelected, R_OBJECT_NOT_FOUND);

	CB(menu.count(pSelected->GetName()) > 0);

	const std::string& strTitle = pSelected->GetName();
	//TODO: seperate title object into different layer
	//currently title object is selectable
	CB(strTitle != path.top()); 
	path.push(strTitle);
	info.labels = menu[strTitle];
	info.labels.emplace_back(strTitle);
	for (size_t i = 0; i < info.labels.size(); i++) {
		info.icons.emplace_back(m_pIconTexture);
	}

	CB((info.icons.size() != 0)); 
	CB((info.labels.size() != 0));

	CR(UpdateCurrentUILayer(info));

Error:
	return r;
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