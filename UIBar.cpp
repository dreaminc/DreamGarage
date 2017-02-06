#include "UIBar.h"
#include "Primitives/FlatContext.h"
#include "Primitives/camera.h"

#include "DreamConsole\DreamConsole.h"

#include <algorithm>

UIBar::UIBar(composite* c, UIBarFormat info) :
	UIModule(c),
	m_info(info)
{
	Initialize();
}

UIBar::~UIBar() {
	// empty
}

RESULT UIBar::Initialize() {

	RESULT r = R_PASS;

	std::vector<std::shared_ptr<UIMenuItem>> pItems;
	CR(CreateMenuLayer(m_info.maxNumButtons + 1)); // + 1 for header item
	pItems = m_pLayers[0]->GetMenuItems();
	CR(m_context->SetVisible(false));

Error:
	return r;
}

RESULT UIBar::UpdateWithRadialLayout(std::shared_ptr<UIMenuItem> pItem, int index) {

	std::shared_ptr<composite> pButton = pItem->GetButton();
	std::shared_ptr<quad> q = pItem->GetQuad();

	// used to center the UIBar
	int shift = m_visibleMenuItems / 2;
	float odd = (m_visibleMenuItems % 2 == 0) ? 0.5f : 0.0f;

	bool fHeader = index == m_info.maxNumButtons;

	// Radial layout
	int radIndex = fHeader ? 0 : index;
	float radY = (m_info.itemAngleY * M_PI / 180.0f) * -(radIndex - shift + odd);
	quaternion rotY = quaternion::MakeQuaternionWithEuler(0.0f, radY, 0.0f);
	float yPos = fHeader ? m_info.headerYPos : m_info.yPosition;
	pButton->MoveTo(0.0f, yPos, 0.0f);
	pButton->SetOrientation(rotY);

	float radX = (fHeader ? m_info.headerAngleX : m_info.itemAngleX) * M_PI / 180.0f;
	quaternion rotX = quaternion::MakeQuaternionWithEuler(radX, 0.0f, 0.0f);
	q->MoveTo(0.0f, 0.0f, m_info.menuDepth);
	q->SetOrientation(rotX);

	//could be wrong at first controller update
	float scaleFactor = index == m_selectedIndex ? m_info.enlargedScale : 1;
	q->ScaleX(m_info.itemScale.x() * scaleFactor);
	q->ScaleZ(m_info.itemScale.z() * scaleFactor);

	return R_PASS;
}

RESULT UIBar::DisplayFromMenuTitle(std::string title) {
	RESULT r = R_PASS;

	std::vector<std::string> items = m_info.menu[title];
	m_visibleMenuItems = std::min(m_info.maxNumButtons, (int)items.size());

	std::shared_ptr<texture> pColorTexture = m_context->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);

	// Layout currently only uses one layer
	std::vector<std::shared_ptr<UIMenuItem>> pItems = m_pLayers[0]->GetMenuItems();

	for (int i = 0; i <= m_info.maxNumButtons; i++) {

		bool fTitle = i == m_info.maxNumButtons;

		UIMenuItem::LabelFormat labelFormat = UIMenuItem::LabelFormat();
		labelFormat.label = i < items.size() ? items[i] : (fTitle ? title : "");

		UIMenuItem::IconFormat iconFormat = UIMenuItem::IconFormat();
		if (labelFormat.label != "") { // placeholder; there could be an instance where only an icon is displayed
			iconFormat.pTexture = pColorTexture;
			iconFormat.position = point(0.0f, 0.5f, 0.0f);
		}

		pItems[i]->Update(iconFormat, labelFormat);

		// can be swapped with a future layout
		UpdateWithRadialLayout(pItems[i], i);
	}

	return r;
}

RESULT UIBar::ToggleVisible() {

	RESULT r = R_PASS;

	m_context->SetVisible(!m_context->IsVisible());

	// if the context has become visible, reset its position to match the camera
	if (m_context->IsVisible()) {
		quaternion q = m_context->GetCamera()->GetOrientation();
		m_rotationY = q.ProjectedYRotationDeg();
		quaternion q2 = quaternion::MakeQuaternionWithEuler(0.0f, m_rotationY * M_PI / 180.0f, 0.0f);

		m_context->SetPosition(m_context->GetCamera()->GetPosition());
		m_context->SetOrientation(q2);

		// When the Menu becomes visible, open the root node
		m_menuPath.push("");
		DisplayFromMenuTitle("");
	}

	return r;
}

RESULT UIBar::Notify(SenseControllerEvent *event) {

	SENSE_CONTROLLER_EVENT_TYPE type = event->type;
	int cType = event->state.type;

	// Currently, only uses the right controller
	// The left controller sends the exact same kinds of events, confounding results
	if (cType == 1) {
		if (type == SENSE_CONTROLLER_MENU_DOWN) {
			if (m_UIDirty) {
				m_UIDirty = false;
				if (m_menuPath.empty()) {
					ToggleVisible();
					return R_PASS;
				}
				m_menuPath.pop();
				if (m_menuPath.empty()) {
					ToggleVisible();
					return R_PASS;
				}
				std::string str = m_menuPath.top();
				DisplayFromMenuTitle(str);
			}
		}
		else if (type == SENSE_CONTROLLER_MENU_UP) {
			m_UIDirty = true;
		}

		if (type == SENSE_CONTROLLER_TRIGGER_MOVE && event->state.triggerRange == 1.0f) {
			if (m_UISelect && !m_menuPath.empty()) {
				std::vector<std::string> currentMenu = m_info.menu[m_menuPath.top()];
				if (m_selectedIndex >= 0 && m_selectedIndex < (int)currentMenu.size() && m_info.menu.count(currentMenu[m_selectedIndex]) > 0) {
					std::string title = currentMenu[m_selectedIndex];

					m_menuPath.push(title);
					DisplayFromMenuTitle(title);
					
					m_UISelect = false;
				}
			}
		}
		else if (type == SENSE_CONTROLLER_TRIGGER_MOVE && event->state.triggerRange != 1.0f) {
			m_UISelect = true;
		}
	}

	return R_PASS;
}

RESULT UIBar::UpdateSelectedItem(int index) {

	if (m_selectedIndex != index) {
		// swap enlarged menu item
		if (0 <= index && index < m_visibleMenuItems) {
			auto p = m_context->GetChildren()[index];
			std::shared_ptr<composite> pButton = std::dynamic_pointer_cast<composite>(p);
			if (pButton != nullptr) {
				auto q = pButton->GetChildren()[0];
				q->ScaleX(m_info.itemScale.x() * m_info.enlargedScale);
				q->ScaleZ(m_info.itemScale.z() * m_info.enlargedScale);
			}
		}
		if (0 <= m_selectedIndex && m_selectedIndex < m_visibleMenuItems) {
			auto p = m_context->GetChildren()[m_selectedIndex];
			std::shared_ptr<composite> pButton = std::dynamic_pointer_cast<composite>(p);
			if (pButton != nullptr) {
				auto q = pButton->GetChildren()[0];
				q->ScaleX(m_info.itemScale.x());
				q->ScaleZ(m_info.itemScale.z());
			}
		}
		m_selectedIndex = index;
	}

	return R_PASS;
}

RESULT UIBar::Update(ray handRay) {
	
	// convert ray into context's space
	point ptContext = handRay.GetOrigin() - m_context->GetOrigin();
	ptContext = point(ptContext.x(), 0.0f, ptContext.z());
	ray contextRay = ray(ptContext, handRay.GetVector());
	point intersect = FurthestRaySphereIntersect(contextRay, point(0.0f, 0.0f, 0.0f));
	vector vIntersect = vector(intersect);
	vIntersect.Normalize();

	int shift = (int)m_visibleMenuItems / 2;
	float odd = (m_visibleMenuItems % 2 != 0) ? 0.5f : 0.0f;
	int numSections = m_info.maxNumButtons != 0 ? 360 / int(m_info.itemAngleY) : 1;
	float deg = float(-atan2(vIntersect.x(), vIntersect.z()) * 180.0f / M_PI);
	deg -= m_rotationY;
	int index = (-int((deg) / m_info.itemAngleY - odd) + shift) % numSections;
	UpdateSelectedItem(index);

	OVERLAY_DEBUG_SET("selected", m_selectedIndex);
	return R_PASS;
}

point UIBar::FurthestRaySphereIntersect(const ray &r, point center) {

	vector vRayCircle = static_cast<ray>(r).ptOrigin() - center;

	float bValue = static_cast<ray>(r).vDirection().dot(vRayCircle);
	float cValue = vRayCircle.dot(vRayCircle) - pow(m_info.menuDepth, 2.0f);
	float resultValue = pow(bValue, 2.0f) - cValue;

	OVERLAY_DEBUG_SET("res", resultValue);

	if (resultValue >= 0.0f) {
		float term = pow(resultValue, 0.5f);
		float t1 = (-1.0f) * bValue + term;
		//this result will always be smaller
		//float t2 = (-1.0f) * bValue - term;
		return r.GetOrigin() + point(t1 * r.GetVector());
	}
	return point(0.0f, 0.0f, 0.0f);
}