#include "UIBar.h"
#include "Primitives/FlatContext.h"
#include "Primitives/camera.h"

#include "DreamConsole\DreamConsole.h"

#include <algorithm>

UIBar::UIBar(composite* c) :
	m_context(c)
{
	//VARIABLES FOR DOUG
	m_info.maxNumButtons = 5;			// number of buttons in the arc 

	m_info.yPosition = -0.5f;	// y position of buttons on the screen, 0.0f is the center
	m_info.menuDepth = -1.5f;	// distance between the screen and a button (z position)

	m_info.itemAngleY = 20.0f;	// angle between buttons on the arc
	m_info.itemAngleX = 60.0f;	// upward tilt of the buttons

	m_info.itemScale = vector(0.5f, 1.0f, 0.25f); // 
	m_info.enlargedScale = 1.25f;	// this value is multiplied to the scale when the menu item is selected

	m_info.headerAngleX = 75.0f;	// values for menu item header
	m_info.headerYPos = -0.25f;

	m_info.menu[""] = { "Watch", "Listen", "Play", "Whisper", "Present" }; //words display from left to right
	Initialize();
}

UIBar::UIBar(composite* c, UIBarInfo info) :
	m_context(c),
	m_info(info)
{
	Initialize();
}

UIBar::~UIBar() {
	// empty
}

RESULT UIBar::Initialize() {

	RESULT r = R_PASS;

	// menu items
	for (int i = 0; i < m_info.maxNumButtons; i++) {
		std::shared_ptr<composite> pButton = m_context->AddComposite();
		std::shared_ptr<quad> q = pButton->AddQuad(1.0f, 1.0f);
		q->RotateXByDeg(m_info.itemAngleX);

		m_buttons.emplace_back(pButton);
	}

	// title
	std::shared_ptr<composite> pButton = m_context->AddComposite();
	std::shared_ptr<quad> q = pButton->AddQuad(1.0f, 1.0f);
	q->RotateXByDeg(m_info.headerAngleX);
	m_buttons.emplace_back(pButton);
	
	m_context->SetVisible(false);

	return r;
}

RESULT UIBar::DisplayFromMenuTitle(std::string title) {
	RESULT r = R_PASS;

	std::vector<std::string> items = m_info.menu[title];
	m_visibleMenuItems = std::min(m_info.maxNumButtons, (int)items.size());

	//other ones
	int shift = m_visibleMenuItems / 2;
	float odd = (m_visibleMenuItems % 2 == 0) ? 0.5f : 0.0f;
	std::shared_ptr<texture> pColorTexture = m_context->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);

	for (int i = 0; i <= m_info.maxNumButtons; i++) {

		bool fTitle = i == m_info.maxNumButtons;

		std::shared_ptr<composite> pButton = m_buttons[i];
		std::shared_ptr<quad> q = std::dynamic_pointer_cast<quad>(pButton->GetChildren()[0]);

		// TODO: could it be possible to reuse this object?
		// Currently, if the object is reused, the new texture is composed with the old texture
		std::shared_ptr<FlatContext> pContext = m_context->MakeFlatContext();
		const std::string str = i < items.size() ? items[i] : (fTitle ? title : "");
		std::shared_ptr<text> pText = pContext->AddText(L"ArialDistance.fnt", str, 0.2f, true);

		if (str != "") { // placeholder; there could be an instance where we only want an icon displayed
			std::shared_ptr<quad> pQuad2 = pContext->AddQuad(0.5f, 0.5f, point(0.0f, 0.5f, 0.0f));
			pQuad2->SetColorTexture(pColorTexture.get());
		}

		m_context->RenderToTexture(pContext);

		q->SetColorTexture(pContext->GetFramebuffer()->GetTexture());
		pContext->GetFramebuffer();

		int radIndex = fTitle ? 0 : i;
		float rad = (m_info.itemAngleY * M_PI / 180.0f) * -(radIndex - shift + odd);
		quaternion rot = quaternion::MakeQuaternionWithEuler(0.0f, rad, 0.0f);

		float yPos = fTitle ? m_info.headerYPos : m_info.yPosition;
		pButton->MoveTo(0.0f, yPos, 0.0f);
		pButton->SetOrientation(rot);

		q->MoveTo(0.0f, 0.0f, m_info.menuDepth);
		// hack
		float scaleFactor = i == m_selectedIndex ? m_info.enlargedScale : 1;
		q->ScaleX(m_info.itemScale.x() * scaleFactor);
		q->ScaleZ(m_info.itemScale.z() * scaleFactor);
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